#include <iostream>

#include "../3rdparty/random.hpp"
#include "../dice.h"
#include "../items/utils.h"
#include "creature.h"

using namespace std;
using namespace effolkronium;

static Creature::CreatureId ids = static_cast<Creature::CreatureId>(0);

constexpr int cDisableTick = 2;
constexpr int cFatigueDivisor = 10;

Creature::Creature()
    : m_BTN(cBaseBTN)
    , m_strength(1)
    , m_agility(1)
    , m_intuition(1)
    , m_perception(1)
    , m_willpower(1)
    // todo: not fists
    , m_primaryWeaponId(cFistsId)
    , m_secondaryWeaponId(cFistsId)
    , m_disableWeaponId(cFistsId)
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

eLength Creature::getSecondaryWeaponReach() const
{
    // if in staff grip or halfsword, we have alternate secondary
    // so we should not override the reach

    if (getGrip() == eGrips::Staff || getGrip() == eGrips::HalfSword) {
        return getCurrentReach();
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

void Creature::setWeapon(int idx)
{
    const Weapon* weapon = WeaponTable::getSingleton()->get(idx);
    m_primaryWeaponId = idx;
}

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

    m_wounds.push_back(wound);

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
            disableWeapon(true);
        }
    }
    if (drop2 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getReflex()) < 2) {
            disableWeapon(true);
        }
    }
    if (drop3 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getReflex()) < 3) {
            disableWeapon(true);
        }
    }
    if (drop != effects.end()) {
        disableWeapon(true);
    }

    m_BTN = max(m_BTN, wound->getBTN());
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
    const Weapon* weapon = isWeaponDisabled() == false
        ? getPrimaryWeapon()
        : WeaponTable::getSingleton()->get(m_disableWeaponId);
    int carry = m_combatPool;
    carry = min(0, carry);
    m_combatPool = getProficiency(weapon->getType()) + getReflex() + carry;
    m_combatPool -= static_cast<int>(m_AP);

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
    const Weapon* weapon = isWeaponDisabled() == false
        ? getPrimaryWeapon()
        : WeaponTable::getSingleton()->get(m_disableWeaponId);
    int combatPool = getProficiency(weapon->getType()) + getReflex();
    combatPool -= static_cast<int>(m_AP);

    combatPool -= m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor;

    return combatPool;
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

void Creature::resetFatigue()
{
    for (auto it : m_fatigue) {
        it.second = 0;
    }
}

void Creature::disableWeapon(bool drop)
{
    // remove all dice from offense and defense pools when this happens
    // so impact gets transferred directly to remainig CP
    m_currentOffense.dice = 0;
    m_currentDefense.dice = 0;

    // do nothing, since we cannot drop fists
    // or natural weapon
    if (m_primaryWeaponId == cFistsId) {
        return;
    }
    m_primaryWeaponDisabled = true;
    m_disarm = cDisableTick;
}

void Creature::dropWeapon() {}

void Creature::enableWeapon() { m_primaryWeaponDisabled = false; }

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
    int metalArmorCount = 0;
    for (auto location : m_hitLocations) {
        vector<eBodyParts> parts = WoundTable::getSingleton()->getUniqueParts(location);
        for (auto part : parts) {
            // ignore the secondpart arm/head
            if (part != eBodyParts::SecondLocationArm && part != eBodyParts::SecondLocationHead) {
                ArmorSegment segment = getArmorAtPart(part);
                if (segment.isMetal) {
                    metalArmorCount++;
                }
            }
        }
    }
    return metalArmorCount > m_hitLocations.size() / 2;
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

// TODO: MIGRATE THIS TO SOMETHING ELSE
// not good to have creature as an AI god class
void Creature::doOffense(const Creature* target, int reachCost, eLength currentReach, bool allin,
    bool dualRedThrow, bool payCosts)
{
    cout << "allin : " << allin << endl;
    const Weapon* weapon = nullptr;

    if (target->getCombatPool() <= 0) {
        allin = true;
    }

    setCreatureOffenseManuever(eOffensiveManuevers::Thrust, currentReach);
    if (primaryWeaponDisabled() == false) {
        m_currentOffense.withPrimaryWeapon = true;
        weapon = getPrimaryWeapon();
    } else {
        m_currentOffense.withPrimaryWeapon = false;
        weapon = getSecondaryWeapon();
    }

    m_currentOffense.component = weapon->getBestAttack();
    if (m_currentOffense.component->getAttack() == eAttacks::Swing) {
        setCreatureOffenseManuever(eOffensiveManuevers::Swing, currentReach);
    }
    // randomly beat
    if (target->primaryWeaponDisabled() == false && random_static::get(0, 3) < 1) {
        setCreatureOffenseManuever(eOffensiveManuevers::Beat, currentReach);
    }
    // replace me
    m_currentOffense.target = target->getHitLocations()[random_static::get(
        0, static_cast<int>(target->getHitLocations().size()) - 1)];

    // get least armored location
    if (target->hasEnoughMetalArmor()) {
        // full of metal armor, so lets do some fancy shit

        if (hasEnoughMetalArmor() == false && weapon->canHook() == true
            && target->getStance() != eCreatureStance::Prone) {
            // try to trip
            setCreatureOffenseManuever(eOffensiveManuevers::Hook, currentReach);
        } else if (weapon->getType() == eWeaponTypes::Polearms) {
            // temporary
            if (getOffensiveManueverCost(eOffensiveManuevers::PinpointThrust, getGrip(),
                    getPrimaryWeapon(), currentReach)
                <= m_combatPool) {
                target->getLowestArmorPart(
                    &m_currentOffense.pinpointTarget, &m_currentOffense.target);
                m_currentOffense.component = weapon->getBestThrust();
                // change
                setCreatureOffenseManuever(eOffensiveManuevers::PinpointThrust, currentReach);
            } else {
                m_currentOffense.manuever = eOffensiveManuevers::Hook;
            }
        } else if (weapon->getType() == eWeaponTypes::Longswords) {
            // temporary
            if (getOffensiveManueverCost(eOffensiveManuevers::PinpointThrust, getGrip(),
                    getPrimaryWeapon(), currentReach)
                <= m_combatPool) {
                target->getLowestArmorPart(
                    &m_currentOffense.pinpointTarget, &m_currentOffense.target);
                m_currentOffense.component = weapon->getBestThrust();
                // change
                setCreatureOffenseManuever(eOffensiveManuevers::PinpointThrust, currentReach);
            } else {
                // todo : mordhau
                m_currentOffense.component = weapon->getPommelStrike();
                m_currentOffense.manuever = eOffensiveManuevers::Mordhau;
            }
        }
    } else {
        int highestUnarmoredLocations = 0;
        for (auto location : target->getHitLocations()) {
            vector<eBodyParts> parts = WoundTable::getSingleton()->getUniqueParts(location);

            int unarmoredLocations = 0;
            for (auto part : parts) {
                // ignore the secondpart arm/head
                if (part != eBodyParts::SecondLocationArm
                    && part != eBodyParts::SecondLocationHead) {
                    ArmorSegment segment = target->getArmorAtPart(part);
                    if (segment.isMetal == false && segment.isRigid == false) {
                        unarmoredLocations++;
                    }
                }
            }
            if (unarmoredLocations > highestUnarmoredLocations) {
                m_currentOffense.target = location;
                highestUnarmoredLocations = unarmoredLocations;
            }
            constexpr int bufferDice = 5;
            if ((target->getCombatPool() + bufferDice < getCombatPool())
                || (target->getBTN() > getBTN())) {
                // we have enough of an advantage to do anything we want
                // but if head and chest are fully armored and we don't have a blunt damage attack
                // then thats bad
            }
        }
    }

    int dice = m_combatPool / 2 + random_static::get(0, m_combatPool / 3)
        - random_static::get(0, m_combatPool / 4);

    if (target->getBTN() < getBTN()) {
        dice += getBTN() - target->getBTN();
    }

    if (m_combatPool > target->getCombatPool()) {
        dice += random_static::get(0, m_combatPool - target->getCombatPool());
    }
    if (weapon->getLength() > target->getPrimaryWeapon()->getLength()) {
        dice += weapon->getLength() - target->getPrimaryWeapon()->getLength();
    }

    // bound
    dice += reachCost;
    dice = max(0, dice);
    dice = min(dice, m_combatPool);
    // never issue 0 dice for attack
    if (m_combatPool > 0 && dice == 0) {
        dice = 1;
    }
    if (allin == true) {
        m_currentOffense.dice = m_combatPool;
    } else {
        m_currentOffense.dice = dice;
    }

    if (dualRedThrow == true && m_combatPool > 0) {
        m_currentDefense.withPrimaryWeapon = true;
        m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
        m_currentDefense.dice = m_combatPool - m_currentOffense.dice;
        m_hasDefense = true;
        assert(m_currentDefense.dice <= m_combatPool || m_currentDefense.dice == 0);
        if (payCosts) {
            reduceCombatPool(m_currentDefense.dice);
        }
    }
    assert(m_currentOffense.dice <= m_combatPool || m_currentOffense.dice == 0);
    if (payCosts) {
        reduceCombatPool(m_currentOffense.dice);
    }
    m_hasOffense = true;
}

void Creature::doDefense(const Creature* attacker, bool isLastTempo)
{
    int diceAllocated = attacker->getQueuedOffense().dice;
    const Weapon* weapon = getPrimaryWeapon();
    if (primaryWeaponDisabled()) {
        m_currentDefense.withPrimaryWeapon = false;
    } else {
        m_currentDefense.withPrimaryWeapon = true;
    }
    constexpr int buffer = 3;
    if ((diceAllocated + buffer < m_combatPool && random_static::get(0, 2) == 0)
        || (isLastTempo && diceAllocated + buffer < getCombatPool())) {
        if (random_static::get(0, 3) == 0) {
            m_currentDefense.manuever = eDefensiveManuevers::ParryLinked;
            reduceCombatPool(getDefensiveManueverCost(eDefensiveManuevers::ParryLinked, getGrip()));
        } else {
            m_currentDefense.manuever = eDefensiveManuevers::Expulsion;
            reduceCombatPool(getDefensiveManueverCost(eDefensiveManuevers::Expulsion, getGrip()));
        }

    } else {
        m_currentDefense.manuever = eDefensiveManuevers::Parry;
    }
    if (isLastTempo == true) {
        // use all dice because we're going to refresh anyway
        m_currentDefense.dice = m_combatPool;
        m_currentDefense.dice = max(m_currentDefense.dice, 0);
        reduceCombatPool(m_currentDefense.dice);
        return;
    }
    int stealDie = 0;
    if (stealInitiative(attacker, stealDie) == true) {
        m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
        m_currentDefense.dice = stealDie;
        assert(m_currentDefense.dice <= m_combatPool || m_currentDefense.dice == 0);
        reduceCombatPool(m_currentDefense.dice);
        return;
    }

    int dice = std::min(diceAllocated + random_static::get(0, diceAllocated / 3)
            - random_static::get(0, diceAllocated / 4),
        m_combatPool);
    dice = min(m_combatPool, dice);
    dice = max(dice, 0);
    m_currentDefense.dice = dice;
    assert(m_currentDefense.dice <= m_combatPool || m_currentDefense.dice == 0);
    reduceCombatPool(m_currentDefense.dice);
    m_hasDefense = true;
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

bool Creature::stealInitiative(const Creature* attacker, int& outDie)
{
    int diceAllocated = attacker->getQueuedOffense().dice;

    int combatPool = attacker->getCombatPool() / 2 + getTap(attacker->getMobility());
    combatPool += attacker->getQueuedOffense().manuever == eOffensiveManuevers::Thrust ? 1 : 0;
    float maxDiff = cMaxBTN - cBaseBTN;
    float attackerDisadvantage = (maxDiff - (attacker->getBTN() - cBaseBTN)) / maxDiff;
    float myDisadvantage = (maxDiff - (getBTN() - cBaseBTN)) / maxDiff;

    // make sure this is enough for an attack + overcoming advantage
    int bufferDie = random_static::get(4, 8);
    int reachCost = max(attacker->getCurrentReach() - getCurrentReach(), 0);
    bufferDie += reachCost;
    if ((combatPool * attackerDisadvantage) + bufferDie
        < (m_combatPool + getTap(getMobility())) * myDisadvantage) {
        float mult = 1.0;
        // favor in my tn difference
        mult += (getBTN() - cBaseBTN) / 10.f;
        cout << mult << endl;
        int diff = attacker->getCombatPool() * mult;
        int dice = diff + random_static::get(4, 8);
        if (m_combatPool - bufferDie >= dice) {
            outDie = dice;
            m_hasDefense = true;
            return true;
        }
    }
    return false;
}

void Creature::doPrecombat(const Creature* opponent)
{
    /*
        if(m_combatPool > 1) {
        m_favoredLocations.insert(eHitLocations::Head);
        reduceCombatPool(1);
    }
     */
    const Weapon* weapon = getPrimaryWeapon();
    if (opponent->hasEnoughMetalArmor()) {
        if (hasEnoughMetalArmor() == false && weapon->canHook()) {
            setGrip(eGrips::Standard);
        } else if (weapon->getType() == eWeaponTypes::Polearms) {
            setGrip(eGrips::Staff);
        } else if (weapon->getType() == eWeaponTypes::Longswords) {
            setGrip(eGrips::HalfSword);
        }
    }
    if (getCombatPool() > 3 && droppedWeapon()) {
        attemptPickup();
    }

    if (getCombatPool() > 3 && getStance() == eCreatureStance::Prone) {
        attemptStand();
    }
    m_hasPrecombat = true;
}

void Creature::doPreresolution(const Creature* opponent)
{
    if (getFeintCost() < getCombatPool()) {
        setCreatureFeint();
    }
}

void Creature::doStolenInitiative(const Creature* defender, bool allin)
{
    m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
    Defense defend = defender->getQueuedDefense();
    m_currentDefense.dice = min(m_combatPool, defend.dice);
    if (allin == true) {
        m_currentDefense.dice = m_combatPool;
    }
    assert(m_currentDefense.dice <= m_combatPool || m_currentDefense.dice == 0);
    reduceCombatPool(m_currentDefense.dice);
    m_hasDefense = true;
}

eInitiativeRoll Creature::doInitiative(const Creature* opponent)
{
    // do random for now
    // this should be based on other creatures weapon length and armor and stuff

    int modifiers = 0;
    int reachDiff
        = static_cast<int>(opponent->getCurrentReach()) - static_cast<int>(getCurrentReach());

    constexpr int cBase = 8;
    int base = cBase;
    base += reachDiff;
    base += opponent->getMobility() - getMobility();
    base += opponent->getCombatPool() - getCombatPool();

    int passiveness = random_static::get(2, 4);
    if (random_static::get(0, base) < (cBase / passiveness)) {
        return eInitiativeRoll::Attack;
    }
    return eInitiativeRoll::Defend;
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

bool Creature::setCreatureOffenseManuever(eOffensiveManuevers manuever, eLength currentReach)
{
    int cost = getOffensiveManueverCost(manuever, getGrip(), getPrimaryWeapon(), currentReach);
    bool canUse = (cost <= getCombatPool());
    if (canUse) {
        m_currentOffense.manuever = manuever;
        reduceCombatPool(cost);
    }
    return canUse;
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
