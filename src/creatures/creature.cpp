#include <iostream>

#include "../3rdparty/random.hpp"
#include "../dice.h"
#include "../items/utils.h"
#include "creature.h"

using namespace std;
using namespace effolkronium;

static Creature::CreatureId ids = static_cast<Creature::CreatureId>(0);

BOOST_CLASS_EXPORT(Creature)

constexpr int cDisableTick = 2;
constexpr int cFatigueDivisor = 10;

Creature::Creature(int naturalWeaponId)
    : m_BTN(cBaseBTN)
    , m_pain(0)
    , m_strength(1)
    , m_agility(1)
    , m_intuition(1)
    , m_perception(1)
    , m_willpower(1)
    , m_naturalWeaponId(naturalWeaponId)
    , m_primaryWeaponId(naturalWeaponId)
    , m_secondaryWeaponId(naturalWeaponId)
    , m_disableWeaponId(naturalWeaponId)
    , m_primaryWeaponDisabled(false)
    , m_secondaryWeaponDisabled(false)
    , m_combatPool(0)
    , m_disarm(0)
    , m_currentState(eCreatureState::Idle)
    , m_bonusDice(0)
    , m_bloodLoss(0)
    , m_hasOffense(false)
    , m_hasDefense(false)
    , m_hasPosition(false)
    , m_hasPreResolution(false)
    , m_flagInitiative(false)
    , m_bleeding(false)
    , m_hasPrecombat(false)
    , m_currentGrip(eGrips::Standard)
    , m_currentStance(eCreatureStance::Standing)
    , m_id(ids++)
{
    m_fatigue[eCreatureFatigue::Stamina] = 0;
}

const Weapon* Creature::getPrimaryWeapon() const
{
    return WeaponTable::getSingleton()->get(m_primaryWeaponId);
}

const Weapon* Creature::getSecondaryWeapon() const
{
    if (getGrip() == eGrips::Staff && getPrimaryWeapon()->getType() == eWeaponTypes::Polearms) {
        return getPrimaryWeapon()->getSecondaryWeapon();
    }
    if (getGrip() == eGrips::HalfSword
        && getPrimaryWeapon()->getType() == eWeaponTypes::Longswords) {
        return getPrimaryWeapon()->getSecondaryWeapon();
    }
    return WeaponTable::getSingleton()->get(m_secondaryWeaponId);
}

const Weapon* Creature::getNaturalWeapon() const
{
    return WeaponTable::getSingleton()->get(m_naturalWeaponId);
}

eLength Creature::getCurrentReach() const
{
    const Weapon* weapon = getPrimaryWeapon();
    // ugly
    int reach = static_cast<int>(weapon->getLength()) - gripReachDifference(m_currentGrip);
    if (m_currentStance == eCreatureStance::Prone) {
        reach -= 2;
    }
    reach = max(0, reach);
    return static_cast<eLength>(reach);
}

void Creature::removeQuickdrawItem(int id)
{
    for (unsigned i = 0; i < m_quickDrawItems.size(); ++i) {
        if (m_quickDrawItems[i] == id) {
            m_quickDrawItems.erase(m_quickDrawItems.begin() + i);
            return;
        }
    }
}

bool Creature::findInQuickdraw(int id)
{
    for (unsigned i = 0; i < m_quickDrawItems.size(); ++i) {
        if (m_quickDrawItems[i] == id) {
            return true;
        }
    }
    return false;
}

int Creature::getNumEquipped(int id)
{
    int count = 0;
    if (m_primaryWeaponId == id) {
        count++;
    }
    if (m_secondaryWeaponId == id) {
        count++;
    }

    for (auto i : m_quickDrawItems) {
        if (i == id) {
            count++;
        }
    }

    for (auto i : m_armor) {
        if (i == id) {
            count++;
        }
    }
    return count;
}

eLength Creature::getSecondaryWeaponReach() const
{
    // if in staff grip or halfsword, we have alternate secondary
    // so we should not override the reach

    if (getGrip() == eGrips::Staff || getGrip() == eGrips::HalfSword) {
        return static_cast<eLength>(max(getCurrentReach() - static_cast<eLength>(1), 0));
    }

    const Weapon* weapon = getSecondaryWeapon();
    // ugly
    int reach = static_cast<int>(weapon->getLength()) - gripReachDifference(m_currentGrip);
    if (m_currentStance == eCreatureStance::Prone) {
        reach -= 2;
    }
    reach = max(0, reach);
    return static_cast<eLength>(reach);
}

std::vector<const Armor*> Creature::getArmor() const
{
    std::vector<const Armor*> ret;
    for (unsigned i = 0; i < m_armor.size(); ++i) {
        ret.push_back(ArmorTable::getSingleton()->get(m_armor[i]));
    }
    return ret;
}

void Creature::setPrimaryWeapon(int idx) { m_primaryWeaponId = idx; }

void Creature::setSecondaryWeapon(int idx) { m_secondaryWeaponId = idx; }

void Creature::inflictImpact(int impact)
{
    m_currentOffense.dice -= impact;

    if (m_currentOffense.dice < 0) {
        int diff = abs(m_currentOffense.dice);
        m_currentOffense.dice = 0;
        m_currentPosition.dice -= diff;
        if (m_currentPosition.dice < 0) {
            m_combatPool -= -m_currentPosition.dice;
            m_currentPosition.dice = 0;
        }
    }
}

void Creature::inflictWound(Wound* wound)
{
    // m_fatigue[eCreatureFatigue::Stamina]++;
    int impact = wound->getImpact();
    if (getHasOffense() == true) {
        m_currentOffense.dice -= impact;
        if (m_currentOffense.dice < 0) {
            impact = abs(m_currentOffense.dice);
            m_currentOffense.dice = 0;
        } else {
            impact = 0;
        }
    }

    if (getHasPosition() == true) {
        m_currentPosition.dice -= impact;
        if (m_currentPosition.dice < 0) {
            impact = abs(m_currentPosition.dice);
            m_currentPosition.dice = 0;
        } else {
            impact = 0;
        }
    }

    m_combatPool -= impact;

    // m_wounds.push_back(wound);
    m_wounds[wound->getLocation()] = wound->getLevel();

    if (wound->causesDeath() == true) {
        m_currentState = eCreatureState::Dead;
    }
    if (wound->immediateKO() == true) {
        m_currentState = eCreatureState::Unconscious;
    }
    set<eEffects> effects = wound->getEffects();
    auto BL1 = effects.find(eEffects::BL1);
    auto BL2 = effects.find(eEffects::BL2);
    auto BL3 = effects.find(eEffects::BL3);
    if (BL1 != effects.end()) {
        m_bloodLoss++;
        m_bleeding = true;
    }
    if (BL2 != effects.end()) {
        m_bloodLoss += 2;
        m_bleeding = true;
    }
    if (BL3 != effects.end()) {
        m_bloodLoss += 3;
        m_bleeding = true;
    }
    auto KO1 = effects.find(eEffects::KO1);
    auto KO2 = effects.find(eEffects::KO2);
    auto KO3 = effects.find(eEffects::KO3);
    auto KO = effects.find(eEffects::KO);
    if (KO1 != effects.end()) {
        if (DiceRoller::rollGetSuccess(m_BTN, getGrit()) < 1) {
            m_currentState = eCreatureState::Unconscious;
        }
    }
    if (KO2 != effects.end()) {
        if (DiceRoller::rollGetSuccess(m_BTN, getGrit()) < 2) {
            m_currentState = eCreatureState::Unconscious;
        }
    }
    if (KO3 != effects.end()) {
        if (DiceRoller::rollGetSuccess(m_BTN, getGrit()) < 3) {
            m_currentState = eCreatureState::Unconscious;
        }
    }
    if (KO != effects.end()) {
        m_currentState = eCreatureState::Unconscious;
    }
    auto KD1 = effects.find(eEffects::KD1);
    auto KD2 = effects.find(eEffects::KD2);
    auto KD3 = effects.find(eEffects::KD3);
    auto KD = effects.find(eEffects::KD);
    if (KD1 != effects.end()) {
        if (DiceRoller::rollGetSuccess(m_BTN, getReflex()) < 1) {
            m_currentStance = eCreatureStance::Prone;
        }
    }
    if (KD2 != effects.end()) {
        if (DiceRoller::rollGetSuccess(m_BTN, getReflex()) < 2) {
            m_currentStance = eCreatureStance::Prone;
        }
    }
    if (KD3 != effects.end()) {
        if (DiceRoller::rollGetSuccess(m_BTN, getReflex()) < 3) {
            m_currentStance = eCreatureStance::Prone;
        }
    }
    if (KD != effects.end()) {
        m_currentStance = eCreatureStance::Prone;
    }
    if (m_bloodLoss >= cBaseBloodLoss) {
        m_currentState = eCreatureState::Unconscious;
    }

    auto drop1 = effects.find(eEffects::drop1);
    auto drop2 = effects.find(eEffects::drop2);
    auto drop3 = effects.find(eEffects::drop3);
    auto drop = effects.find(eEffects::drop);
    if (drop1 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getReflex()) < 1) {
            dropWeapon();
        }
    }
    if (drop2 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getReflex()) < 2) {
            dropWeapon();
        }
    }
    if (drop3 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getReflex()) < 3) {
            dropWeapon();
        }
    }
    if (drop != effects.end()) {
        dropWeapon();
    }

    // m_BTN = max(m_BTN, wound->getBTN());
    m_pain += wound->getPain();
}

int Creature::getSuccessRate() const
{
    float sides = static_cast<float>(DiceRoller::cDiceSides);
    float btn = static_cast<float>(DiceRoller::cDiceSides - getBTN()) + 1.f;

    float val = btn / sides;
    val *= 100;
    return static_cast<int>(val);
}

ArmorSegment Creature::getArmorAtPart(eBodyParts part) const
{
    auto it = m_armorValues.find(part);
    if (it != m_armorValues.end()) {
        return it->second;
    }
    return ArmorSegment();
}

vector<ArmorSegment> Creature::getArmorAtLocation(eHitLocations location) const
{
    vector<ArmorSegment> armors;
    auto parts = WoundTable::getSingleton()->getUniqueParts(location);
    for (auto it : parts) {
        if (it != eBodyParts::SecondLocationArm && it != eBodyParts::SecondLocationHead) {
            armors.push_back(getArmorAtPart(it));
        }
    }
    return armors;
}

ArmorSegment Creature::getMedianArmor(eHitLocations location, bool swing) const
{
    // sorting slower than worth
    // median of medians slower than worth
    // since O(N) < 10, always
    ArmorSegment segment;
    unordered_map<int, int> hash;
    int metalCount = 0;

    auto parts = WoundTable::getSingleton()->getAllLocations(location).m_swing;
    if (swing == false) {
        // slow
        parts = WoundTable::getSingleton()->getAllLocations(location).m_thrust;
    }
	int total = 0;
    for (unsigned i = 0; i < WoundTable::cPartsPerLocation; ++i) {
        auto it = parts[i];
        if (it != eBodyParts::SecondLocationArm && it != eBodyParts::SecondLocationHead) {
            ArmorSegment seg = getArmorAtPart(it);
            hash[seg.AV]++;
            if (seg.isMetal) {
                metalCount++;
            }
			total++;
        }
    }
    int key = hash.begin()->first;
    int median = hash.begin()->second;
    for (auto it : hash) {
        if (it.second > median) {
            median = it.second;
            key = it.first;
        }
    }
    segment.AV = key;
    if (metalCount > (total / 2)) {
        segment.isMetal = true;
    }
    return segment;
}

void Creature::equipArmor(int id)
{
    const Armor* armor = ArmorTable::getSingleton()->get(id);
    assert(armor != nullptr);

    // make sure it doesnt overlap with another armor
    for (int i : m_armor) {
        const Armor* equippedArmor = ArmorTable::getSingleton()->get(i);
        assert(armor->isOverlapping(equippedArmor) == false);
    }

    m_armor.push_back(id);
    applyArmor();
}

bool Creature::canEquipArmor(int id)
{
    const Armor* armor = ArmorTable::getSingleton()->get(id);
    assert(armor != nullptr);
    for (int i : m_armor) {
        const Armor* equippedArmor = ArmorTable::getSingleton()->get(i);
        if (armor->isOverlapping(equippedArmor) == true) {
            return false;
        }
    }
    return true;
}

void Creature::removeArmor(int id)
{
    m_armor.erase(remove(m_armor.begin(), m_armor.end(), id), m_armor.end());
    applyArmor();
}

void Creature::resetCombatPool()
{
    // carryover impact damage across tempos
    const Weapon* weapon = getPrimaryWeapon();
    int carry = m_combatPool;
    carry = min(0, carry);
    m_combatPool = getProficiency(weapon->getType()) + getReflex() + carry;
    m_combatPool -= static_cast<int>(m_AP);
    m_combatPool -= getPain();
    // apply fatigue
    /*
    for (auto it : m_fatigue) {
        m_combatPool -= it.second;
        }*/
    m_combatPool -= m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor;

    // prone gives us less CP
    /* too imbalanced right now
    if (m_currentStance == eCreatureStance::Prone) {
        m_combatPool -= 2;
    }
    */

    // cant go below 0 for CP even if impact took out a lot of dice
    m_combatPool = max(0, m_combatPool);
    // cout << getName() << m_combatPool << endl;
}

int Creature::getMaxCombatPool()
{
    // carryover impact damage across tempos
    const Weapon* weapon = getPrimaryWeapon();
    int combatPool = getProficiency(weapon->getType()) + getReflex();
    combatPool -= static_cast<int>(m_AP);
    combatPool -= getPain();
    combatPool -= m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor;

    return max(combatPool, 0);
}

void Creature::addAndResetBonusDice()
{
    m_currentOffense.dice += m_bonusDice;
    m_bonusDice = 0;
}

const std::vector<eHitLocations> Creature::getHitLocations() const
{
    std::vector<eHitLocations> ret;
    for (auto it : m_hitLocations) {
        if (m_favoredLocations.find(it) == m_favoredLocations.end()) {
            ret.push_back(it);
        }
    }
    return ret;
}

bool Creature::rollFatigue()
{
    int requiredSuccesses = 1;
    requiredSuccesses += static_cast<int>(m_AP);

    // int successes = DiceRoller::rollGetSuccess(getBTN(), getGrit());
    int temp = m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor;
    m_fatigue[eCreatureFatigue::Stamina] += requiredSuccesses;
    cout << m_fatigue[eCreatureFatigue::Stamina] << endl;
    if (temp < m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor) {
        return true;
    }
    return false;
}

void Creature::resetFatigue() { m_fatigue.at(eCreatureFatigue::Stamina) = 0; }

void Creature::disableWeapon()
{
    // remove all dice from offense and defense pools when this happens
    // so impact gets transferred directly to remainig CP
    m_currentOffense.dice = 0;
    m_currentDefense.dice = 0;

    // do nothing, since we cannot drop fists
    // or natural weapon
    if (m_primaryWeaponId == m_naturalWeaponId) {
        return;
    }
    m_primaryWeaponDisabled = true;
    m_disarm = cDisableTick;
}

void Creature::dropWeapon()
{
    if (getPrimaryWeaponId() != m_naturalWeaponId) {
        // make sure secondary weapons go away
        setGrip(eGrips::Standard);
        m_droppedWeapons.push_back(getPrimaryWeaponId());
        setPrimaryWeapon(m_naturalWeaponId);
    }
}

void Creature::dropSecondaryWeapon()
{
    if (getSecondaryWeaponId() != m_naturalWeaponId) {
        // make sure secondary weapons go away
        setGrip(eGrips::Standard);
        m_droppedWeapons.push_back(getSecondaryWeaponId());
        setSecondaryWeapon(m_naturalWeaponId);
    }
}

void Creature::enableWeapon() { m_primaryWeaponDisabled = false; }

void Creature::pickupWeapon()
{
    assert(m_droppedWeapons.size() > 0);
    dropWeapon();
    int weaponId = m_droppedWeapons[m_droppedWeapons.size() - 1];
    m_droppedWeapons.pop_back();
}

bool Creature::canPerformManuever(eOffensiveManuevers manuever)
{
    const Weapon* weapon = getPrimaryWeapon();
    switch (manuever) {
    case eOffensiveManuevers::Hook:
        return (weapon->canHook() == true || getGrip() == eGrips::HalfSword
            || getGrip() == eGrips::Staff);
        break;
    case eOffensiveManuevers::Mordhau:
        return (weapon->getType() == eWeaponTypes::Swords
            || weapon->getType() == eWeaponTypes::Longswords);
        break;
    default:
        return true;
    }
}

void Creature::attemptStand()
{
    m_currentPosition.manuever = ePositionManuevers::Stand;
    m_currentPosition.dice = 3;
    reduceCombatPool(3);
    m_hasPosition = true;
}

void Creature::attemptPickup()
{
    m_currentPosition.manuever = ePositionManuevers::Pickup;
    m_currentPosition.dice = 3;
    reduceCombatPool(3);
    m_hasPosition = true;
}

int Creature::getFatigue() const
{
    return m_fatigue.at(eCreatureFatigue::Stamina) / cFatigueDivisor;
}

bool Creature::hasEnoughMetalArmor() const
{
    // if there are more than half hit locations with metal armor on them, return true
    int total = 0;

    for (auto location : m_hitLocations) {
		auto parts = WoundTable::getSingleton()->getAllLocations(location).m_thrust;
        int metalArmorCount = 0;
		int count = 0;
		for (int i = 0; i < WoundTable::cPartsPerLocation; ++i) {
			auto part = parts[i];
            // ignore the secondpart arm/head
            if (part != eBodyParts::SecondLocationArm && part != eBodyParts::SecondLocationHead) {
                ArmorSegment segment = getArmorAtPart(part);
                if (segment.isMetal) {
                    metalArmorCount++;
                }
				count++;
            }
        }
        if (metalArmorCount > count / 2) {
            total++;
        }
    }
    return total == m_hitLocations.size();
}

void Creature::getLowestArmorPart(eBodyParts* pPartOut, eHitLocations* pHitOut) const
{
    int lowestAV = -1;
    for (auto location : getHitLocations()) {
        vector<eBodyParts> parts = WoundTable::getSingleton()->getUniqueParts(location);
        for (auto part : parts) {
            // ignore the secondpart arm/head
            if (part != eBodyParts::SecondLocationArm && part != eBodyParts::SecondLocationHead) {
                ArmorSegment segment = getArmorAtPart(part);
                if (lowestAV == -1 || segment.AV < lowestAV) {
                    *pHitOut = location;
                    *pPartOut = part;
                    lowestAV = segment.AV;
                }
            }
        }
    }
    assert(lowestAV != -1);
}

void Creature::doPositionRoll(const Creature* opponent)
{
    // can be reused for standing up as well
    int dice = 0;
    if (opponent == nullptr) {
        // outnumbered, so we choose some nubmer that feels good
        dice = m_combatPool / 4;
    } else {
        dice = opponent->getQueuedPosition().dice;
    }

    m_currentPosition.dice
        = dice + random_static::get(0, dice / 3) - random_static::get(0, dice / 4);
    m_currentPosition.dice = min(m_combatPool, m_currentPosition.dice);
    m_currentPosition.dice = max(0, m_currentPosition.dice);
    assert(m_currentPosition.dice <= m_combatPool);
    reduceCombatPool(m_currentPosition.dice);
    m_hasPosition = true;
}

void Creature::clearCreatureManuevers(bool skipDisable)
{
    m_currentOffense.reset();
    m_currentDefense.reset();
    m_currentPosition.reset();

    m_hasOffense = false;
    m_hasDefense = false;
    m_hasPosition = false;
    m_hasPrecombat = false;
    m_hasPreResolution = false;
    m_flagInitiative = false;
    m_favoredLocations.clear();

    if (skipDisable == false) {
        if (m_disarm > 0) {
            m_disarm--;
            if (m_disarm == 0) {
                enableWeapon();
            }
        }
    }
}

bool Creature::setCreatureFeint()
{
    int cost = getFeintCost();
    bool canUse = (cost <= getCombatPool());
    if (canUse) {
        m_currentOffense.feint = true;
        reduceCombatPool(cost);
    }
    return canUse;
}

void Creature::setInitiative(eInitiativeRoll initiative)
{
    m_initiative = initiative;
    m_flagInitiative = true;
}

void Creature::clearArmor()
{
    m_AP = 0;
    for (auto it : m_armorValues) {
        it.second.AV = 0;
        it.second.isMetal = false;
        it.second.isRigid = false;
        it.second.type = eArmorTypes::None;
    }
}

void Creature::applyArmor()
{
    clearArmor();
    for (int i : m_armor) {
        const Armor* armor = ArmorTable::getSingleton()->get(i);
        m_AP += armor->getAP();
        for (auto it : armor->getCoverage()) {
            m_armorValues[it].AV = max(m_armorValues[it].AV, armor->getAV());
            m_armorValues[it].isMetal = m_armorValues[it].isMetal || armor->isMetal();
            m_armorValues[it].isRigid = m_armorValues[it].isRigid || armor->isRigid();
            m_armorValues[it].type = max(armor->getType(), m_armorValues[it].type);
        }
    }
}
