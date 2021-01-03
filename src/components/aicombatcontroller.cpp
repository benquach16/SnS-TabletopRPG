#include <iostream>
#include <queue>

#include "../3rdparty/random.hpp"
#include "../items/utils.h"
#include "aicombatcontroller.h"
#include "combatinstance.h"
#include "combatmanager.h"

using namespace std;
using namespace effolkronium;

struct ManueverContainer {
    int priority;
    int cost;
    eOffensiveManuevers offManuever;
    eDefensiveManuevers defManuever;
    Component* component;
    eHitLocations hitLocation;
    eBodyParts pinpointLocation;
};

bool operator<(const ManueverContainer& lhs, const ManueverContainer& rhs)
{
    return lhs.priority < rhs.priority;
}
constexpr int cFuzz = 2;
constexpr int cLowestPriority = -10;

AICombatController::AICombatController()
{
    m_hitPriorities[eHitLocations::Head] = 5;
    m_hitPriorities[eHitLocations::Chest] = 4;
    m_hitPriorities[eHitLocations::Belly] = 3;
    m_hitPriorities[eHitLocations::Arm] = 3;
    m_hitPriorities[eHitLocations::Thigh] = 2;
    m_hitPriorities[eHitLocations::Shin] = 1;
}

// this function should be very similar to player combat ui code
// in terms of control flow. if they deviate then logic is really flawed, as
// it should be easy to plug in and out inputs
void AICombatController::run(const CombatManager* manager, Creature* controlledCreature)
{
    assert(manager != nullptr);

    CombatEdge* edge = manager->getCurrentEdge();
    if (edge == nullptr) {
        return;
    }
    CombatInstance* instance = edge->getInstance();
    int reachCost
        = calculateReachCost(instance->getCurrentReach(), controlledCreature->getCurrentReach());
    if (instance->getState() == eCombatState::Uninitialized) {
        return;
    }

    auto creatureId = controlledCreature->getId();
    Creature* enemy = nullptr;
    if (instance->getAttacker()->getId() == creatureId) {
        enemy = instance->getDefender();
    } else {
        enemy = instance->getAttacker();
    }
    if (instance->getState() == eCombatState::RollInitiative) {
        doInitiative(controlledCreature, enemy);
    }

    if (instance->getState() == eCombatState::PreexchangeActions) {
        doPrecombat(controlledCreature, enemy);
    }
    if (instance->getState() == eCombatState::PreResolution
        && instance->getAttacker()->getId() == creatureId) {
        doPreresolution(controlledCreature, enemy);
    }
    if (instance->getState() == eCombatState::Offense
        && instance->getAttacker()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getDefender(), reachCost, instance,
            instance->getLastTempo(), instance->getDualRedThrow());
        return;
    }
    if (instance->getState() == eCombatState::StolenOffense
        && instance->getAttacker()->getId() == creatureId) {
        doStolenInitiative(controlledCreature, instance->getDefender(), false);
    }
    if (instance->getState() == eCombatState::Defense
        && instance->getDefender()->getId() == creatureId) {
        doDefense(controlledCreature, instance->getAttacker(), instance, instance->getLastTempo());
        return;
    }
    if (instance->getState() == eCombatState::AttackFromDefense
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getDefender(), reachCost, instance, true, false);
        return;
    }
    if (instance->getState() == eCombatState::StealInitiative
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost, instance, true, false);
        return;
    }
    if (instance->getState() == eCombatState::ParryLinked
        && instance->getDefender()->getId() == creatureId) {
        // do not pay costs
        doOffense(
            controlledCreature, instance->getAttacker(), reachCost, instance, false, false, false);
        return;
    }

    if (instance->getState() == eCombatState::DualOffenseStealInitiative
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost, instance, false, true);
    }
    if (instance->getState() == eCombatState::DualOffense1
        && instance->getAttacker()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getDefender(), reachCost, instance, false, true);
    }
    if (instance->getState() == eCombatState::DualOffense2
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost, instance, false, true);
    }
    if (instance->getState() == eCombatState::DualOffenseSecondInitiative
        && instance->getDefender()->getId() == creatureId) {
        doStolenInitiative(controlledCreature, instance->getAttacker(), true);
    }
}

bool AICombatController::setCreatureOffenseManuever(
    Creature* controlledCreature, eOffensiveManuevers manuever, eLength currentReach, bool payReach)
{
    bool usePrimary = controlledCreature->getQueuedOffense().withPrimaryWeapon;
    eLength effectiveReach = usePrimary ? controlledCreature->getCurrentReach()
                                        : controlledCreature->getSecondaryWeaponReach();
    int cost = getOffensiveManueverCost(
        manuever, controlledCreature->getGrip(), effectiveReach, currentReach, payReach);
    bool canUse = (cost <= controlledCreature->getCombatPool());
    if (canUse) {
        controlledCreature->setOffenseManuever(manuever);
        controlledCreature->reduceCombatPool(cost);
    }
    return canUse;
}

bool AICombatController::setCreatureDefenseManuever(
    Creature* controlledCreature, eDefensiveManuevers manuever, eLength currentReach)
{
    bool usePrimary = controlledCreature->getQueuedOffense().withPrimaryWeapon;
    eLength effectiveReach = usePrimary ? controlledCreature->getCurrentReach()
                                        : controlledCreature->getSecondaryWeaponReach();
    int cost = getDefensiveManueverCost(
        manuever, controlledCreature->getGrip(), effectiveReach, currentReach);
    bool canUse = (cost <= controlledCreature->getCombatPool() || cost == 0);
    if (canUse) {
        controlledCreature->setDefenseManuever(manuever);
        controlledCreature->reduceCombatPool(cost);
    }
    return canUse;
}

void AICombatController::doOffense(Creature* controlledCreature, const Creature* target,
    int reachCost, const CombatInstance* instance, bool allin, bool dualRedThrow, bool payCosts)
{
    if (controlledCreature->getHasOffense()) {
        cout << "already has offense";
        return;
    }
    cout << "allin : " << allin << endl;
    const Weapon* weapon = nullptr;

    if (target->getCombatPool() <= 0) {
        allin = true;
    }
    if (controlledCreature->primaryWeaponDisabled() == false) {
        controlledCreature->setOffenseWeapon(true);
        weapon = controlledCreature->getPrimaryWeapon();
    } else {
        controlledCreature->setOffenseWeapon(false);
        weapon = controlledCreature->getSecondaryWeapon();
    }
    map<eOffensiveManuevers, int> manuevers = getAvailableOffManuevers(controlledCreature,
        controlledCreature->getQueuedOffense().withPrimaryWeapon, instance->getCurrentReach(),
        instance->getInGrapple(), payCosts);

    bool targetHasEnoughArmor = target->hasEnoughMetalArmor();
    // algorithm - for each manuever, assign some priority and add to priority queue
    // then iterate through queue and select the best item if we can afford the cost
    // do nothing is at the bottom and always free
    // priority should be determined by weapon damage and enemy armor
    priority_queue<ManueverContainer, vector<ManueverContainer>> manueverPriorities;
    for (auto it : manuevers) {
        // default high enough to avoid weapons wiht negative damage values,
        // as priority is modified by the damage of the weapon
        int priority = controlledCreature->getCombatPool() - it.second;
        ManueverContainer toPush;
        toPush.offManuever = it.first;
        toPush.cost = it.second;
        Component* component = nullptr;
        switch (it.first) {
        case eOffensiveManuevers::NoOffense:
            priority = 0;
            break;
        case eOffensiveManuevers::Swing: {
            auto swings = weapon->getSwingComponents();
            // what kind of component we use depends on the armor of the opponent
            // base this on the median armor value and properties
            // if more than half the target location is metal armor, then favor blunt
            int bestDamage = -10;
            for (auto swing : swings) {
                int damage = 0;
                eHitLocations bestTarget = getBestHitLocation(target, swing, damage);
                if (damage > bestDamage) {
                    bestDamage = damage;
                    component = swing;
                    toPush.hitLocation = bestTarget;
                }
            }
            priority += random_static::get(bestDamage, bestDamage + cFuzz);
            assert(component != nullptr);
        } break;
        case eOffensiveManuevers::Thrust: {
            component = weapon->getBestThrust();
            // heavily prioritize the thrust as we probably will never want to swing if our best
            // attack is the thrust
            int damage = 0;
            toPush.hitLocation = getBestHitLocation(target, component, damage);
            if (weapon->getBestAttack()->getAttack() == eAttacks::Thrust) {
                priority += random_static::get(damage, damage + cFuzz);
            }
        } break;
        case eOffensiveManuevers::PinpointThrust: {
            // only used for bypassing armor
            eHitLocations location;
            eBodyParts part;
            target->getLowestArmorPart(&part, &location);
            component = weapon->getBestThrust();
            priority += component->getDamage() - target->getArmorAtPart(part).AV;
            toPush.hitLocation = location;
            toPush.pinpointLocation = part;
        } break;
        case eOffensiveManuevers::Beat: {
            // big random
            if (target->primaryWeaponDisabled() == false) {
                priority += random_static::get(0, cFuzz + reachCost);
            } else {
                priority = cLowestPriority;
            }

        } break;
        case eOffensiveManuevers::Hook: {
            // really should not do this if they are already prone
            if (target->getStance() == eCreatureStance::Prone) {
                priority = cLowestPriority;
            } else {
                priority += random_static::get(0, cFuzz);
            }

        } break;
        case eOffensiveManuevers::Mordhau:
            if (targetHasEnoughArmor == false) {
                priority = cLowestPriority;
            } else {
            }
            break;
        case eOffensiveManuevers::Throw: {
            if (target->getStance() == eCreatureStance::Prone) {
                priority = cLowestPriority;
            } else {
                // really important to do this in a grapple if possible
                priority += 20;
            }
        } break;
        }

        toPush.priority = priority;
        toPush.component = component;
        manueverPriorities.push(toPush);
    }
    assert(manueverPriorities.empty() == false);
    while (manueverPriorities.empty() == false) {
        auto current = manueverPriorities.top();
        if (current.cost < controlledCreature->getCombatPool() || current.cost == 0) {
            // choose
            setCreatureOffenseManuever(
                controlledCreature, current.offManuever, instance->getCurrentReach(), payCosts);
            controlledCreature->setOffenseTarget(current.hitLocation);
            controlledCreature->setOffenseComponent(current.component);
            controlledCreature->setOffensePinpointTarget(current.pinpointLocation);
            break;
        }
        manueverPriorities.pop();
    }

    int dice = controlledCreature->getCombatPool() / 2
        + random_static::get(0, controlledCreature->getCombatPool() / 3)
        - random_static::get(0, controlledCreature->getCombatPool() / 4);

    if (target->getBTN() < controlledCreature->getBTN()) {
        dice += controlledCreature->getBTN() - target->getBTN();
    }

    if (controlledCreature->getCombatPool() > target->getCombatPool()) {
        dice
            += random_static::get(0, controlledCreature->getCombatPool() - target->getCombatPool());
    }
    // bound
    dice += reachCost;
    dice = min(dice, controlledCreature->getCombatPool());
    dice = max(0, dice);

    // never issue 0 dice for attack
    if (controlledCreature->getCombatPool() > 0 && dice == 0) {
        dice = 1;
    }
    if (allin == true) {
        dice = max(0, controlledCreature->getCombatPool());
    }

    controlledCreature->setOffenseDice(dice);

    if (dualRedThrow == true && controlledCreature->getCombatPool() > 0) {
        controlledCreature->setDefenseWeapon(true);
        controlledCreature->setDefenseManuever(eDefensiveManuevers::StealInitiative);
        int defDie
            = controlledCreature->getCombatPool() - controlledCreature->getQueuedOffense().dice;
        controlledCreature->setDefenseDice(defDie);
        controlledCreature->setDefenseReady();
        assert(defDie <= controlledCreature->getCombatPool() || defDie == 0);
        if (payCosts) {
            controlledCreature->reduceCombatPool(defDie);
        }
    }
    assert(controlledCreature->getQueuedOffense().dice <= controlledCreature->getCombatPool()
        || controlledCreature->getQueuedOffense().dice == 0);
    if (payCosts) {
        controlledCreature->reduceCombatPool(controlledCreature->getQueuedOffense().dice);
    }
    controlledCreature->setOffenseReady();
}

void AICombatController::doDefense(Creature* controlledCreature, const Creature* attacker,
    const CombatInstance* instance, bool isLastTempo)
{
    int diceAllocated = attacker->getQueuedOffense().dice;
    const Weapon* weapon = controlledCreature->getPrimaryWeapon();
    if (controlledCreature->primaryWeaponDisabled() == false) {
        controlledCreature->setDefenseWeapon(true);
        weapon = controlledCreature->getPrimaryWeapon();
    } else {
        controlledCreature->setDefenseWeapon(false);
        weapon = controlledCreature->getSecondaryWeapon();
    }
    eLength effectiveReach = controlledCreature->getQueuedDefense().withPrimaryWeapon
        ? controlledCreature->getCurrentReach()
        : controlledCreature->getSecondaryWeaponReach();
    int reachCost = calculateReachCost(effectiveReach, instance->getCurrentReach());
    map<eDefensiveManuevers, int> manuevers = getAvailableDefManuevers(controlledCreature,
        controlledCreature->getQueuedOffense().withPrimaryWeapon, isLastTempo,
        instance->getCurrentReach(), instance->getInGrapple());

    priority_queue<ManueverContainer, vector<ManueverContainer>> manueverPriorities;

    for (auto it : manuevers) {
        int priority = controlledCreature->getCombatPool() - it.second;
        ManueverContainer toPush;
        toPush.defManuever = it.first;
        toPush.cost = it.second;
        switch (it.first) {
        case eDefensiveManuevers::Parry:
            if (weapon->getNaturalWeapon()) {
                priority -= random_static::get(0, 3);
            }
            break;
        case eDefensiveManuevers::ParryLinked: {
            constexpr int buffer = 3;
            // for now don't do this until we figure out reach costs for compound defenses
            if (diceAllocated + buffer < controlledCreature->getCombatPool()) {
                if (isLastTempo) {
                    // priority += 10;
                }
            }
        }

        break;
        case eDefensiveManuevers::Dodge:
            if (controlledCreature->primaryWeaponDisabled()) {
                priority++;
            } else {
                priority--;
            }

            break;
        case eDefensiveManuevers::Expulsion: {
            constexpr int buffer = 3;
            if (diceAllocated + buffer < controlledCreature->getCombatPool()) {
                if (isLastTempo) {
                    priority += 10;
                }
            }
        } break;
        case eDefensiveManuevers::Counter:

            break;
        case eDefensiveManuevers::AttackFromDef: {
            priority -= 20;
        }

        break;
        case eDefensiveManuevers::StealInitiative: {
            int stealDie = 0;
            if (stealInitiative(controlledCreature, attacker, stealDie)) {
                priority += 20;
            } else {
                priority -= 20;
            }
        } break;
        }

        toPush.priority = priority;
        manueverPriorities.push(toPush);
    }
    assert(manueverPriorities.empty() == false);
    while (manueverPriorities.empty() == false) {
        auto current = manueverPriorities.top();
        if (current.cost < controlledCreature->getCombatPool() || current.cost == 0) {
            // choose
            setCreatureDefenseManuever(
                controlledCreature, current.defManuever, instance->getCurrentReach());
            break;
        }
        manueverPriorities.pop();
    }

    int stealDie = 0;

    if (isLastTempo == false && stealInitiative(controlledCreature, attacker, stealDie) == true) {
        controlledCreature->setDefenseManuever(eDefensiveManuevers::StealInitiative);
        controlledCreature->setDefenseDice(stealDie);
        assert(stealDie <= controlledCreature->getCombatPool() || stealDie == 0);
        controlledCreature->reduceCombatPool(stealDie);
        controlledCreature->setDefenseReady();
        return;
    }
    if (isLastTempo == true) {
        // use all dice because we're going to refresh anyway
        int defDie = max(controlledCreature->getCombatPool(), 0);
        controlledCreature->setDefenseDice(defDie);
        controlledCreature->reduceCombatPool(defDie);
        controlledCreature->setDefenseReady();
        return;
    }
    int dice = std::min(diceAllocated + random_static::get(0, diceAllocated / 3)
            - random_static::get(0, diceAllocated / 4),
        controlledCreature->getCombatPool());
    dice = max(dice, 3);
    dice = min(controlledCreature->getCombatPool(), dice);
    dice = max(dice, 0);
    controlledCreature->setDefenseDice(dice);
    assert(dice <= controlledCreature->getCombatPool() || dice == 0);
    controlledCreature->reduceCombatPool(dice);
    controlledCreature->setDefenseReady();
}

void AICombatController::doStolenInitiative(
    Creature* controlledCreature, const Creature* defender, bool allin)
{
    controlledCreature->setDefenseManuever(eDefensiveManuevers::StealInitiative);
    Defense defend = defender->getQueuedDefense();
    controlledCreature->setDefenseDice(min(controlledCreature->getCombatPool(), defend.dice));
    assert(defend.dice >= 0);
    if (allin == true) {
        controlledCreature->setDefenseDice(controlledCreature->getCombatPool());
    }
    controlledCreature->reduceCombatPool(controlledCreature->getQueuedDefense().dice);
    controlledCreature->setDefenseReady();
}

void AICombatController::doPositionRoll(Creature* controlledCreature, const Creature* opponent)
{
    // can be reused for standing up as well
    int dice = 0;
    if (opponent == nullptr) {
        // outnumbered, so we choose some nubmer that feels good
        dice = controlledCreature->getCombatPool() / 4;
    } else {
        dice = opponent->getQueuedPosition().dice;
    }

    dice += random_static::get(0, dice / 3) - random_static::get(0, dice / 4);
    dice = min(controlledCreature->getCombatPool(), dice);
    dice = max(0, dice);

    assert(dice <= controlledCreature->getCombatPool());
    controlledCreature->setPositionDice(dice);
    controlledCreature->reduceCombatPool(dice);
    controlledCreature->setPositionReady();
}

void AICombatController::doPrecombat(Creature* controlledCreature, const Creature* opponent)
{
    const Weapon* weapon = controlledCreature->getPrimaryWeapon();
    if (opponent->hasEnoughMetalArmor()) {
        if (controlledCreature->hasEnoughMetalArmor() == false && weapon->canHook()) {
            controlledCreature->setGrip(eGrips::Standard);
        } else if (weapon->getType() == eWeaponTypes::Polearms) {
            controlledCreature->setGrip(eGrips::Staff);
        } else if (weapon->getType() == eWeaponTypes::Longswords) {
            controlledCreature->setGrip(eGrips::HalfSword);
        }
    }
    if (controlledCreature->getCombatPool() > 3 && controlledCreature->droppedWeapon()) {
        controlledCreature->attemptPickup();
    }

    if (controlledCreature->getCombatPool() > 3
        && controlledCreature->getStance() == eCreatureStance::Prone) {
        controlledCreature->attemptStand();
    }
    controlledCreature->setPrecombatReady();
}

void AICombatController::doPreresolution(Creature* controlledCreature, const Creature* opponent)
{
    if (getFeintCost() < controlledCreature->getCombatPool()) {
        controlledCreature->setCreatureFeint();
    }
    controlledCreature->setPreResolutionReady();
}

void AICombatController::doInitiative(Creature* controlledCreature, const Creature* opponent)
{
    // do random for now
    // this should be based on other creatures weapon length and armor and stuff

    int modifiers = 0;
    int reachDiff = static_cast<int>(opponent->getCurrentReach())
        - static_cast<int>(controlledCreature->getCurrentReach());

    constexpr int cBase = 8;
    int base = cBase;
    base += reachDiff;
    base += opponent->getMobility() - controlledCreature->getMobility();
    base += opponent->getCombatPool() - controlledCreature->getCombatPool();

    int passiveness = random_static::get(2, 4);
    if (random_static::get(0, base) < (cBase / passiveness)) {
        controlledCreature->setInitiative(eInitiativeRoll::Attack);
    } else {
        controlledCreature->setInitiative(eInitiativeRoll::Defend);
    }
}

bool AICombatController::stealInitiative(
    Creature* controlledCreature, const Creature* attacker, int& outDie)
{
    int diceAllocated = attacker->getQueuedOffense().dice;

    int combatPool = attacker->getCombatPool() / 2 + getTap(attacker->getMobility());
    combatPool += attacker->getQueuedOffense().manuever == eOffensiveManuevers::Thrust ? 1 : 0;
    float maxDiff = cMaxBTN - cBaseBTN;
    float attackerDisadvantage = (maxDiff - (attacker->getBTN() - cBaseBTN)) / maxDiff;
    float myDisadvantage = (maxDiff - (controlledCreature->getBTN() - cBaseBTN)) / maxDiff;

    // make sure this is enough for an attack + overcoming advantage
    int bufferDie = random_static::get(4, 8);
    int reachCost = max(attacker->getCurrentReach() - controlledCreature->getCurrentReach(), 0);
    bufferDie += reachCost;
    if ((combatPool * attackerDisadvantage) + bufferDie
        < (controlledCreature->getCombatPool() + getTap(controlledCreature->getMobility()))
            * myDisadvantage) {
        float mult = 1.0;
        // favor in my tn difference
        mult += (controlledCreature->getBTN() - cBaseBTN) / 10.f;
        cout << mult << endl;
        int diff = attacker->getCombatPool() * mult;
        int dice = diff + random_static::get(4, 8);
        if (controlledCreature->getCombatPool() - bufferDie >= dice) {
            outDie = dice;
            return true;
        }
    }
    return false;
}

eHitLocations AICombatController::getBestHitLocation(
    const Creature* target, const Component* component, int& outDamage)
{

    eHitLocations ret = target->getHitLocations()[0];
    // lower than anything possible
    int highestDamage = -50;
    for (auto location : target->getHitLocations()) {
        ArmorSegment segment
            = target->getMedianArmor(location, component->getAttack() == eAttacks::Swing);
        if (component->getType() != eDamageTypes::Blunt && segment.isMetal) {
            segment.AV *= 2;
        }
        int damage = component->getDamage() - segment.AV;
        if (damage > highestDamage) {
            highestDamage = damage;
            ret = location;
        }
    }
    outDamage = highestDamage;
    return ret;
}
