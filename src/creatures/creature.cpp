#include <iostream>

#include "../3rdparty/random.hpp"
#include "../dice.h"
#include "creature.h"

using namespace std;
using namespace effolkronium;

static Creature::CreatureId ids = static_cast<Creature::CreatureId>(0);

constexpr int cDisableTick = 2;
constexpr int cFatigueDivisor = 10;

Creature::Creature()
    : m_BTN(cBaseBTN)
    , m_brawn(1)
    , m_agility(1)
    , m_cunning(1)
    , m_perception(1)
    , m_will(1)
    , m_primaryWeaponId(cFistsId)
    , m_disableWeaponId(cFistsId)
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
        if (DiceRoller::rollGetSuccess(getBTN(), getBrawn()) < 1) {
            disableWeapon();
            m_disarm = -1;
        }
    }
    if (drop2 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getBrawn()) < 2) {
            disableWeapon();
            m_disarm = -1;
        }
    }
    if (drop3 != effects.end()) {
        if (DiceRoller::rollGetSuccess(getBTN(), getBrawn()) < 3) {
            disableWeapon();
            m_disarm = -1;
        }
    }
    if (drop != effects.end()) {
        disableWeapon();
        m_disarm = -1;
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

ArmorSegment Creature::getArmorAtPart(eBodyParts part) { return m_armorValues[part]; }

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

    // apply fatigue
    /*
    for (auto it : m_fatigue) {
        m_combatPool -= it.second;
        }*/
    m_combatPool -= m_fatigue[eCreatureFatigue::Stamina] / cFatigueDivisor;

    // prone gives us less CP
    if (m_currentStance == eCreatureStance::Prone) {
        m_combatPool -= 2;
    }

    // cant go below 0 for CP even if impact took out a lot of dice
    m_combatPool = max(0, m_combatPool);
    // cout << getName() << m_combatPool << endl;
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

void Creature::disableWeapon()
{
    m_disarm = cDisableTick;
    m_disableWeaponId = m_primaryWeaponId;
    m_primaryWeaponId = cFistsId;
}

void Creature::enableWeapon()
{
    // don't do anything if we have an actual weapon
    if (m_primaryWeaponId != cFistsId) {
        return;
    }

    swap(m_primaryWeaponId, m_disableWeaponId);
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

void Creature::doOffense(const Creature* target, int reachCost, bool allin, bool dualRedThrow)
{
    cout << "allin : " << allin << endl;
    const Weapon* weapon = getPrimaryWeapon();

    if (target->getCombatPool() <= 0) {
        allin = true;
    }

    m_currentOffense.manuever = eOffensiveManuevers::Thrust;
    m_currentOffense.component = weapon->getBestAttack();
    if (m_currentOffense.component->getAttack() == eAttacks::Swing) {
        m_currentOffense.manuever = eOffensiveManuevers::Swing;
    }

    // replace me
    m_currentOffense.target = target->getHitLocations()[random_static::get(
        0, static_cast<int>(target->getHitLocations().size()) - 1)];
    int dice = m_combatPool / 2 + random_static::get(0, m_combatPool / 3)
        - random_static::get(0, m_combatPool / 3);

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

    if (dualRedThrow == true && m_combatPool > 0 && random_static::get(0, 1) == 0) {
        m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
        m_currentDefense.dice = m_combatPool - m_currentOffense.dice;
        m_hasDefense = true;
        assert(m_currentDefense.dice <= m_combatPool || m_currentDefense.dice == 0);
        reduceCombatPool(m_currentDefense.dice);
    }
    assert(m_currentOffense.dice <= m_combatPool || m_currentOffense.dice == 0);
    reduceCombatPool(m_currentOffense.dice);
    m_hasOffense = true;
}

void Creature::doDefense(const Creature* attacker, bool isLastTempo)
{
    int diceAllocated = attacker->getQueuedOffense().dice;

    constexpr int buffer = 3;
    if (diceAllocated + buffer < m_combatPool && random_static::get(0, 2) == 0) {
        m_currentDefense.manuever = eDefensiveManuevers::ParryLinked;
        reduceCombatPool(1);
    } else {
        m_currentDefense.manuever = eDefensiveManuevers::Parry;
    }

    int stealDie = 0;
    if (stealInitiative(attacker, stealDie) == true) {
        m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
        m_currentDefense.dice = stealDie;
        assert(m_currentDefense.dice <= m_combatPool || m_currentDefense.dice == 0);
        reduceCombatPool(m_currentDefense.dice);
        return;
    }
    if (isLastTempo == true) {
        // use all dice because we're going to refresh anyway
        m_currentDefense.dice = m_combatPool;
        m_currentDefense.dice = max(m_currentDefense.dice, 0);
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

    int combatPool = attacker->getCombatPool() + attacker->getSpeed() / 2;

    int bufferDie = random_static::get(3, 5);

    float disadvantageMult = 1.0;
    float mult = static_cast<float>(random_static::get(3, 5));
    mult += m_BTN - cBaseBTN;
    // mult += attacker->getQueuedOffense().manuever == eOffensiveManuevers::Thrust ? 2 : 0;
    disadvantageMult += (mult / 10);
    if ((combatPool * disadvantageMult) + bufferDie < m_combatPool + getSpeed() / 2) {
        int diff = static_cast<int>(abs((getSpeed() - attacker->getSpeed()) * disadvantageMult));
        int dice = diff + bufferDie;
        if (m_combatPool > dice) {
            outDie = dice;
            m_hasDefense = true;
            return true;
        }
    }
    return false;
}

void Creature::doPrecombat()
{
    /*
        if(m_combatPool > 1) {
        m_favoredLocations.insert(eHitLocations::Head);
        reduceCombatPool(1);
    }
     */
    if (m_combatPool > 3 && droppedWeapon()) {
        attemptPickup();
    }

    if (m_combatPool > 3 && getStance() == eCreatureStance::Prone) {
        attemptStand();
    }
    m_hasPrecombat = true;
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
    base += opponent->getSpeed() - getSpeed();
    base += opponent->getCombatPool() - getCombatPool();

    int passiveness = random_static::get(2, 4);
    if (random_static::get(0, base) < (cBase / passiveness)) {
        return eInitiativeRoll::Attack;
    }
    return eInitiativeRoll::Defend;
}

void Creature::clearCreatureManuevers()
{
    m_currentOffense.dice = 0;
    m_currentOffense.heavyblow = 0;
    m_currentOffense.linked = false;
    m_currentOffense.feint = false;
    m_currentOffense.component = nullptr;
    m_currentDefense.manuever = eDefensiveManuevers::Parry;
    m_currentDefense.dice = 0;
    m_currentPosition.dice = 0;

    m_hasOffense = false;
    m_hasDefense = false;
    m_hasPosition = false;
    m_hasPrecombat = false;

    m_favoredLocations.clear();

    if (m_disarm > 0) {
        m_disarm--;
        if (m_disarm == 0) {
            enableWeapon();
        }
    }
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
