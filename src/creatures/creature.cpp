#include <iostream>

#include "3rdparty/random.hpp"
#include "creature.h"
#include "dice.h"
#include "items/utils.h"
#include "utils.h"

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
    , m_hasFeint(false)
    , m_hasPreResolution(false)
    , m_flagInitiative(false)
    , m_hasPrecombat(false)
    , m_currentGrip(eGrips::Standard)
    , m_currentStance(eCreatureStance::Standing)
    , m_currentGuard(eCombatGuard::None)
    , m_id(ids++)
{
    m_fatigue[eCreatureFatigue::Stamina] = 0;
    m_fatigue[eCreatureFatigue::Hunger] = 0;
    m_fatigue[eCreatureFatigue::Thirst] = 0;
}

void Creature::createBodyParts()
{
    // should never be generated if we already have body parts
    assert(m_bodyParts.empty());
    assert(m_hitLocations.empty() == false);

    for (eHitLocations location : m_hitLocations) {
        for (eBodyParts part : WoundTable::getSingleton()->getUniqueParts(location)) {
            m_bodyParts.push_back(part);
        }
    }
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

void Creature::bleed()
{
    int increment = 0;
    for (auto it : m_bleedLevel) {
        increment += it.second;
    }
    increment -= getTap(getGrit());
    increment = max(1, increment);
    m_bloodLoss += increment;
    if (m_bloodLoss >= cBaseBloodLoss) {
        m_currentState = eCreatureState::Unconscious;
    }
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

    if (getGrip() == eGrips::Staff) {
        // return static_cast<eLength>(max(getCurrentReach() - static_cast<eLength>(1), 0));
        return getCurrentReach();
    }

    const Weapon* weapon = getSecondaryWeapon();
    // ugly
    int reach = static_cast<int>(weapon->getLength());
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

    /*
    if (getHasPosition() == true) {
        m_currentPosition.dice -= impact;
        if (m_currentPosition.dice < 0) {
            impact = abs(m_currentPosition.dice);
            m_currentPosition.dice = 0;
        } else {
            impact = 0;
        }
    }
    */

    m_combatPool -= impact;

    // m_wounds.push_back(wound);
    eBodyParts location = wound->getLocation();
    m_wounds[location][wound->getLevel()] = wound->getPain();

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
        m_bleedLevel[location] += 1;
    }
    if (BL2 != effects.end()) {
        m_bloodLoss += 2;
        m_bleedLevel[location] += 2;
    }
    if (BL3 != effects.end()) {
        m_bloodLoss += 3;
        m_bleedLevel[location] += 3;
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

    if (effects.find(eEffects::Severed) != effects.end()) {
        // sever the limb
        vector<eBodyParts> partsToSever
            = WoundTable::getSingleton()->limbConnection(location, m_bodyParts);
        for (eBodyParts part : partsToSever) {
            m_severedParts.push_back(part);
            // remove from existing body parts
            for (unsigned i = 0; i < m_bodyParts.size(); ++i) {
                if (m_bodyParts[i] == part) {
                    m_bodyParts.erase(m_bodyParts.begin() + i);
                    break;
                }
            }
        }
    }

    // m_BTN = max(m_BTN, wound->getBTN());
    m_pain += wound->getPain();
}

void Creature::healWound(eBodyParts part, int level)
{
    int pain = m_wounds[part][level];
    m_pain -= pain;
    assert(m_pain >= 0);
    m_wounds[part].erase(level);
}

void Creature::reduceWound(eBodyParts part, int level, int value)
{
    value += getTap(getGrit());
    m_wounds[part][level] -= value;
    m_pain -= value;
    m_pain = max(0, m_pain);
    if (m_wounds[part][level] <= 0) {
        m_wounds[part].erase(level);
    }
}

void Creature::healWounds(int level)
{
    // <part, map>
    for (auto part : m_wounds) {
        // <level, pain>
        for (auto it : part.second) {
            if (it.first <= level) {
                healWound(part.first, it.first);
            }
        }
    }
}

int Creature::getSuccessRate() const
{
    float sides = static_cast<float>(DiceRoller::cDiceSides);
    float btn = static_cast<float>(DiceRoller::cDiceSides - getBTN()) + 1.f;

    float val = btn / sides;
    val *= 100;
    return static_cast<int>(val);
}

bool Creature::canStand() const
{
    for (eBodyParts part : m_severedParts) {
        switch (part) {
        case eBodyParts::Foot:
        case eBodyParts::Shin:
        case eBodyParts::Knee:
            return false;
        }
    }
    return true;
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
    int avg = 0;
    for (auto it : hash) {
        avg += it.first;
        if (it.second > median) {
            median = it.second;
            key = it.first;
        }
    }
    avg = avg / hash.size();
    segment.AV = key;
    // if the median isn't more than half, then its just a mess of armors and we should use the
    // average instead
    if (key < total / 2) {
        segment.AV = avg;
    }
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
    m_combatPool = getMaxCombatPool() + carry;

    // reset favored locations every reset
    m_favoredLocations.clear();
}

int Creature::getMaxCombatPool()
{
    // carryover impact damage across tempos
    const Weapon* weapon = getPrimaryWeapon();
    int combatPool = getProficiency(weapon->getType()) + getReflex();
    combatPool -= static_cast<int>(m_AP);
    combatPool -= getPain();
    combatPool -= m_fatigue[eCreatureFatigue::Hunger];
    combatPool -= m_fatigue[eCreatureFatigue::Thirst];
    combatPool -= m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor;
    if (m_currentStance == eCreatureStance::Prone) {
        combatPool = (combatPool + 1) / 2;
    }
    constexpr int minCombatPool = 4;
    combatPool = max(combatPool, minCombatPool);
    return max(combatPool, 0);
}

void Creature::addAndResetBonusDice()
{
    m_combatPool += m_bonusDice;
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
    requiredSuccesses -= getTap(getGrit());
    requiredSuccesses = max(1, requiredSuccesses);
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

void Creature::modifyFatigue(eCreatureFatigue fatigue, int value)
{
    if (fatigue == eCreatureFatigue::Stamina) {
        value *= cFatigueDivisor;
    }
    m_fatigue[fatigue] += value;
    m_fatigue[fatigue] = max(0, m_fatigue[fatigue]);
}

void Creature::reduceBleed(eBodyParts part, unsigned value)
{
    m_bleedLevel[part] -= value;
    if (m_bleedLevel[part] <= 0) {
        m_bleedLevel.erase(part);
    }
}

void Creature::disableWeapon()
{
    // remove all dice from offense and defense pools when this happens
    // so impact gets transferred directly to remainig CP
    if (m_currentOffense.withPrimaryWeapon == true && getHasOffense()) {
        m_currentOffense.dice = 0;
    }
    m_currentDefense.dice = 0;

    // do nothing, since we cannot drop fists
    // or natural weapon
    if (m_primaryWeaponId == m_naturalWeaponId) {
        return;
    }
    m_primaryWeaponDisabled = true;
    m_disarm = cDisableTick;

    // once a weapon is disabled, it cannot guard
    m_favoredLocations.clear();
}

void Creature::disableSecondaryWeapon()
{
    // remove all dice from offense and defense pools when this happens
    // so impact gets transferred directly to remainig CP
    if (m_currentOffense.withPrimaryWeapon == false && getHasOffense()) {
        m_currentOffense.dice = 0;
    }
    m_currentDefense.dice = 0;

    // do nothing, since we cannot drop fists
    // or natural weapon
    if (m_secondaryWeaponId == m_naturalWeaponId) {
        return;
    }
    m_secondaryWeaponDisabled = true;
    m_disarm = cDisableTick;

    // once a weapon is disabled, it cannot guard
    m_favoredLocations.clear();
}

bool Creature::canFavor() const
{
    return (m_favoredLocations.empty() && primaryWeaponDisabled() == false);
}

bool Creature::canEquip(int id)
{
    int hands = 2;
    for (auto part : m_severedParts) {
        if (part == eBodyParts::Hand) {
            hands--;
        }
    }
    const Weapon* primaryWeapon = getPrimaryWeapon();
    switch (primaryWeapon->getType()) {
    case eWeaponTypes::Polearms:
    case eWeaponTypes::Longswords:
        hands -= 2;
        break;
    }

    assert(hands >= 0);
    const Weapon* toEquip = WeaponTable::getSingleton()->get(id);
    if (hands == 0) {
        return false;
    }
    switch (toEquip->getType()) {
    case eWeaponTypes::Polearms:
    case eWeaponTypes::Longswords:
        if (hands < 2) {
            return false;
        }
        break;
    }
    return true;
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

void Creature::addFavored(eHitLocations location)
{
    m_favoredLocations.insert(location);

    // shield gets a bunch of free surrounding locations
    if (getSecondaryWeapon()->isShield()) {
        switch (location) {
        case eHitLocations::Arm:
            m_favoredLocations.insert(eHitLocations::Belly);
            m_favoredLocations.insert(eHitLocations::Chest);
            break;
        case eHitLocations::Head:
            m_favoredLocations.insert(eHitLocations::Arm);
            m_favoredLocations.insert(eHitLocations::Chest);
            break;
        case eHitLocations::Chest:
            m_favoredLocations.insert(eHitLocations::Head);
            m_favoredLocations.insert(eHitLocations::Arm);
            break;
        case eHitLocations::Belly:
            m_favoredLocations.insert(eHitLocations::Chest);
            m_favoredLocations.insert(eHitLocations::Arm);
            break;
        case eHitLocations::Thigh:
            m_favoredLocations.insert(eHitLocations::Belly);
            m_favoredLocations.insert(eHitLocations::Arm);
            break;
        case eHitLocations::Shin:
            m_favoredLocations.insert(eHitLocations::Belly);
            m_favoredLocations.insert(eHitLocations::Thigh);
            break;
        }
    }

    m_currentGuard = getLocationGuard(location);
}

void Creature::enableWeapon() { m_primaryWeaponDisabled = false; }

int Creature::getAvailableHands() const { return 2; }

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

int Creature::getFatigue(eCreatureFatigue fatigue) const
{
    switch (fatigue) {
    case eCreatureFatigue::Stamina:
        return m_fatigue.at(eCreatureFatigue::Stamina) / cFatigueDivisor;
        break;
    default:
        return m_fatigue.at(fatigue);
    }
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

void Creature::getLowestArmorPart(
    bool inAltGrip, eBodyParts* pPartOut, eHitLocations* pHitOut) const
{
    int lowestAV = -1;
    for (auto location : getHitLocations()) {
        vector<eBodyParts> parts = WoundTable::getSingleton()->getUniqueParts(location);
        for (auto part : parts) {
            // ignore the secondpart arm/head
            if (part == eBodyParts::SecondLocationArm || part == eBodyParts::SecondLocationHead) {
                continue;
            }
            if (inAltGrip && (part == eBodyParts::Armpit || part == eBodyParts::Elbow)) {
                continue;
            }
            ArmorSegment segment = getArmorAtPart(part);

            if (lowestAV == -1 || segment.AV < lowestAV) {
                *pHitOut = location;
                *pPartOut = part;
                lowestAV = segment.AV;
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
    reduceCombatPool(m_currentPosition.dice);
    m_hasPosition = true;
}

void Creature::clearCreatureManuevers(bool skipDisable)
{
    m_currentOffense.reset();
    m_currentDefense.reset();
    m_currentPosition.reset();
    resetGuard();
    m_hasOffense = false;
    m_hasDefense = false;
    m_hasPosition = false;
    m_hasFeint = false;
    m_hasPrecombat = false;
    m_hasPreResolution = false;
    m_flagInitiative = false;

    if (skipDisable == false) {
        if (m_disarm > 0) {
            m_disarm--;
            if (m_disarm == 0) {
                enableWeapon();
                enableSecondaryWeapon();
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
    for (auto& it : m_armorValues) {
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
    if (getSecondaryWeapon()->isShield()) {
        m_AP += 1;
    }
}
