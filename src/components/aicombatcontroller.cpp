#include <iostream>
#include <queue>

#include "3rdparty/random.hpp"
#include "aicombatcontroller.h"
#include "combatinstance.h"
#include "combatmanager.h"
#include "items/utils.h"

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
    int dice; // only for defense for now
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
    m_hitPriorities[eHitLocations::Belly] = 4;
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
        doPrecombat(controlledCreature, enemy, instance);
    }
    if (instance->getState() == eCombatState::BetweenExchangeActions) {
        doPrecombat(controlledCreature, enemy, instance);
    }
    if (instance->getState() == eCombatState::PreResolution
        && instance->getAttacker()->getId() == creatureId) {
        doPreresolution(controlledCreature, enemy, instance);
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
            controlledCreature, instance->getAttacker(), reachCost, instance, true, false, true);
        return;
    }

    if (instance->getState() == eCombatState::DualOffenseStealInitiative
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost, instance, false, true);
    }
    if (instance->getState() == eCombatState::DualOffense1
        && instance->getAttacker()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getDefender(), reachCost, instance, true, true);
    }
    if (instance->getState() == eCombatState::DualOffense2
        && instance->getDefender()->getId() == creatureId) {
        doStolenInitiative(controlledCreature, instance->getAttacker(), true);
    }
    if (instance->getState() == eCombatState::DualOffenseSecondInitiative
        && instance->getDefender()->getId() == creatureId) {
        doStolenInitiative(controlledCreature, instance->getAttacker(), true);
    }
}

bool AICombatController::setCreatureOffenseManuever(
    Creature* controlledCreature, eOffensiveManuevers manuever, eLength currentReach, int cost)
{
    bool usePrimary = controlledCreature->getQueuedOffense().withPrimaryWeapon;
    eLength effectiveReach = usePrimary ? controlledCreature->getCurrentReach()
                                        : controlledCreature->getSecondaryWeaponReach();
    bool canUse = (cost <= controlledCreature->getCombatPool());
    if (canUse) {
        controlledCreature->setOffenseManuever(manuever);
        controlledCreature->reduceCombatPool(cost);
    }
    return canUse;
}

bool AICombatController::setCreatureDefenseManuever(
    Creature* controlledCreature, eDefensiveManuevers manuever, eLength currentReach, int cost)
{
    bool usePrimary = controlledCreature->getQueuedOffense().withPrimaryWeapon;
    eLength effectiveReach = usePrimary ? controlledCreature->getCurrentReach()
                                        : controlledCreature->getSecondaryWeaponReach();

    bool canUse = (cost <= controlledCreature->getCombatPool() || cost == 0);
    if (canUse) {
        controlledCreature->setDefenseManuever(manuever);
        controlledCreature->reduceCombatPool(cost);
    }
    return canUse;
}

void AICombatController::chooseOffenseManuever(Creature* controlledCreature, const Creature* target,
    const CombatInstance* instance, bool allin, bool payReach, bool feint)
{
    const Weapon* weapon = nullptr;
    eLength creatureReach = controlledCreature->getCurrentReach();
    if (controlledCreature->primaryWeaponDisabled() == false) {
        controlledCreature->setOffenseWeapon(true);
        weapon = controlledCreature->getPrimaryWeapon();
    } else {
        controlledCreature->setOffenseWeapon(false);
        weapon = controlledCreature->getSecondaryWeapon();
        creatureReach = controlledCreature->getSecondaryWeaponReach();
    }

    int reachCost = calculateReachCost(creatureReach, instance->getCurrentReach());

    map<eOffensiveManuevers, int> manuevers = getAvailableOffManuevers(controlledCreature,
        controlledCreature->getQueuedOffense().withPrimaryWeapon, instance->getCurrentReach(),
        instance->getInGrapple(), payReach, feint, instance->getLastTempo());

    map<eHitLocations, int> locationCosts
        = getHitLocationCost(target, feint, controlledCreature->getQueuedOffense().target);
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
        case eOffensiveManuevers::Draw:
        case eOffensiveManuevers::HeavyBlow:
        case eOffensiveManuevers::Swing: {
            auto swings = weapon->getSwingComponents();
            // what kind of component we use depends on the armor of the opponent
            // base this on the median armor value and properties
            // if more than half the target location is metal armor, then favor blunt
            int bestDamage = -10;
            component = swings[0];
            for (auto swing : swings) {
                int damage = 0;
                eHitLocations bestTarget = getBestHitLocation(
                    target, swing, feint, controlledCreature->getQueuedOffense().target, damage);
                if (damage > bestDamage) {
                    bestDamage = damage;
                    component = swing;
                    toPush.hitLocation = bestTarget;
                }
            }
            toPush.cost += locationCosts[toPush.hitLocation];
            if (controlledCreature->getGrip() == eGrips::Staff
                || controlledCreature->getGrip() == eGrips::HalfSword) {
                bestDamage--;
            }
            // damage may be negative
            // cutting does more pain + impact so favor it unless thrust is way better
            bestDamage = bestDamage + cFuzz + 1;
            if (it.first == eOffensiveManuevers::HeavyBlow) {
                // do this if we have a dice advantage
                priority += (controlledCreature->getCombatPool() - target->getCombatPool()) / 2;
                priority += target->primaryWeaponDisabled() ? 2 : 0;
                bestDamage += 2;
            }
            if (it.first == eOffensiveManuevers::Draw) {
                bestDamage -= 1;
            }
            priority += random_static::get(bestDamage, bestDamage + cFuzz);
            assert(component != nullptr);
        } break;
        case eOffensiveManuevers::Thrust: {
            component = weapon->getBestThrust();
            // heavily prioritize the thrust as we probably will never want to swing if our best
            // attack is the thrust
            int damage = 0;
            toPush.hitLocation = getBestHitLocation(
                target, component, feint, controlledCreature->getQueuedOffense().target, damage);
            if (weapon->getBestAttack()->getAttack() == eAttacks::Thrust) {
                damage += 1;
            }
            // thrust is better on steal init
            if (controlledCreature->getHasDefense()
                && controlledCreature->getQueuedDefense().manuever
                    == eDefensiveManuevers::StealInitiative) {
                damage += 1;
            }
            damage = damage + cFuzz;
            toPush.cost += locationCosts[toPush.hitLocation];
            priority += random_static::get(damage, damage + cFuzz);
        } break;
        case eOffensiveManuevers::PinpointThrust: {
            // only used for bypassing armor
            eHitLocations location;
            eBodyParts part;
            bool inAltGrip = controlledCreature->getGrip() == eGrips::Staff
                || controlledCreature->getGrip() == eGrips::HalfSword
                || weapon->getLength() == eLength::Hand;
            target->getLowestArmorPart(inAltGrip, &part, &location);
            component = weapon->getBestThrust();
            int AV = target->getArmorAtPart(part).AV;
            if (target->getArmorAtPart(part).isMetal
                && component->getProperties().find(eWeaponProperties::MaillePiercing)
                    == component->getProperties().end()) {
                AV *= 2;
            }
            // the more dice advantage we have, the more we want to do this as it guarantees a
            // killshot
            int damage = component->getDamage() - AV;
            damage = damage + cFuzz;
            // thrust is better on steal init
            if (controlledCreature->getHasDefense()
                && controlledCreature->getQueuedDefense().manuever
                    == eDefensiveManuevers::StealInitiative) {
                damage += 1;
            }
            priority += random_static::get(damage, damage + cFuzz);
            priority += target->primaryWeaponDisabled() ? 2 : 0;
            priority += (controlledCreature->getCombatPool() - target->getCombatPool()) / 2;
            toPush.hitLocation = location;
            toPush.cost += locationCosts[toPush.hitLocation];
            toPush.pinpointLocation = part;
        } break;
        case eOffensiveManuevers::Beat: {
            // big random
            if (target->primaryWeaponDisabled() == false) {
                // if we are stealing initiative beat is a really good move
                if (controlledCreature->getHasDefense()
                    && controlledCreature->getQueuedDefense().manuever
                        == eDefensiveManuevers::StealInitiative) {
                    priority += random_static::get(0, controlledCreature->getCombatPool());
                }
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
                // if we are stealing initiative hook is a really good move
                if (controlledCreature->getHasDefense()
                    && controlledCreature->getQueuedDefense().manuever
                        == eDefensiveManuevers::StealInitiative) {
                    priority += random_static::get(0, controlledCreature->getCombatPool());
                    if (controlledCreature->getCombatPool() > 7 && allin) {
                        priority += 10;
                    }
                }
                priority += random_static::get(0, cFuzz);
                // do this if we have enough die for a knockdown
                // never do a hook if we can't afford to get enough MoS for knockdown
                if (controlledCreature->getCombatPool() > target->getCombatPool() + 5
                    && controlledCreature->getCombatPool() > 8) {
                    priority *= 2;
                }
            }

        } break;
        case eOffensiveManuevers::Mordhau:
            if (targetHasEnoughArmor == false) {
                priority = cLowestPriority;
            } else {
            }
            component = weapon->getPommelStrike();
            break;
        case eOffensiveManuevers::Throw: {
            if (target->getStance() == eCreatureStance::Prone) {
                priority = cLowestPriority;
            } else {
                // really important to do this in a grapple if possible
                priority += 20;
            }
        } break;
        case eOffensiveManuevers::Snap: {
            // temporary
            priority += 10;
        }
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
                controlledCreature, current.offManuever, instance->getCurrentReach(), current.cost);
            int damage;
            if (m_doFeint && current.component != nullptr) {
                eHitLocations feintTarget = getBestHitLocation(
                    target, current.component, true, current.hitLocation, damage);
                controlledCreature->setOffenseTarget(feintTarget);
            } else {
                controlledCreature->setOffenseTarget(current.hitLocation);
            }
            controlledCreature->setOffenseComponent(current.component);
            controlledCreature->setOffensePinpointTarget(current.pinpointLocation);

            break;
        }
        manueverPriorities.pop();
    }
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

    int shrewdDiff = controlledCreature->getShrewdness() - target->getShrewdness();
    if (allin == false) {
        if (shrewdDiff > 3 && random_static::get(0, 3) > 2) {
            m_doFeint = true;
        }
    } else {
        m_doFeint = false;
    }

    chooseOffenseManuever(controlledCreature, target, instance, allin, true, false);
    // handles dual red initiative steal
    // should be moved out as it causes confusion
    if (dualRedThrow == true && controlledCreature->getCombatPool() > 0) {
        controlledCreature->setDefenseWeapon(true);
        controlledCreature->setDefenseManuever(eDefensiveManuevers::StealInitiative);
        int defDie = controlledCreature->getCombatPool() / 2 + 2;
        defDie = min(defDie, controlledCreature->getCombatPool());
        defDie = max(0, defDie);
        controlledCreature->setDefenseDice(defDie);
        controlledCreature->setDefenseReady();
        assert(defDie <= controlledCreature->getCombatPool() || defDie == 0);
        if (payCosts) {
            controlledCreature->reduceCombatPool(defDie);
        }
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
    if (controlledCreature->getQueuedOffense().manuever == eOffensiveManuevers::NoOffense) {
        dice = 0;
    }
    controlledCreature->setOffenseDice(dice);

    assert(controlledCreature->getQueuedOffense().dice <= controlledCreature->getCombatPool()
        || controlledCreature->getQueuedOffense().dice == 0);
    if (payCosts) {
        controlledCreature->reduceCombatPool(controlledCreature->getQueuedOffense().dice);
    }
    controlledCreature->setOffenseReady();
}

void AICombatController::normalizeGrip(Creature* controlledCreature, bool isLastTempo)
{
    if (controlledCreature->getGrip() == eGrips::Standard) {
        return;
    }
    eWeaponTypes type = controlledCreature->getPrimaryWeapon()->getType();
    if (type != eWeaponTypes::Polearms && type != eWeaponTypes::Longswords) {
        return;
    }
    int cost = getGripChangeCost(isLastTempo);
    if (controlledCreature->getCombatPool() > cost) {
        controlledCreature->setGrip(eGrips::Standard);
        controlledCreature->reduceCombatPool(cost);
    }
}

void AICombatController::shortenGrip(Creature* controlledCreature, bool isLastTempo)
{
    assert(controlledCreature->getSecondaryWeaponId() == controlledCreature->getNaturalWeaponId());
    if (controlledCreature->getGrip() != eGrips::Standard) {
        return;
    }
    eWeaponTypes type = controlledCreature->getPrimaryWeapon()->getType();
    if (type != eWeaponTypes::Polearms && type != eWeaponTypes::Longswords) {
        return;
    }
    int cost = getGripChangeCost(isLastTempo);
    if (controlledCreature->getCombatPool() > cost) {
        if (type == eWeaponTypes::Polearms) {
            controlledCreature->setGrip(eGrips::Staff);
        } else if (type == eWeaponTypes::Longswords) {
            controlledCreature->setGrip(eGrips::HalfSword);
        }
        controlledCreature->reduceCombatPool(cost);
    }
}

void AICombatController::doDefense(Creature* controlledCreature, const Creature* attacker,
    const CombatInstance* instance, bool isLastTempo)
{
    int diceAllocated = attacker->getQueuedOffense().dice;
    eOffensiveManuevers attack = attacker->getQueuedOffense().manuever;
    const Weapon* weapon = controlledCreature->getPrimaryWeapon();
    const Weapon* secondary = controlledCreature->getSecondaryWeapon();
    if (controlledCreature->primaryWeaponDisabled() == false
        && secondary->getGuardTN() >= weapon->getGuardTN()) {
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
        toPush.dice = 0;
        switch (it.first) {
        case eDefensiveManuevers::Resist:
            // resist and parry are basically the same
        case eDefensiveManuevers::Parry: {
            if (weapon->getNaturalWeapon()) {
                priority -= random_static::get(0, 3);
            }
            if (isLastTempo) {
                toPush.dice = controlledCreature->getCombatPool();
            } else {
                int dice = std::min(diceAllocated + random_static::get(0, diceAllocated / 3)
                        - random_static::get(0, diceAllocated / 4),
                    controlledCreature->getCombatPool());
                if (attack == eOffensiveManuevers::Hook) {
                    dice = dice + 1 / 2;
                }
                dice = max(dice, 3);
                toPush.dice = dice;
            }
        } break;
        case eDefensiveManuevers::ParryLinked: {
            constexpr int buffer = 3;
            // for now don't do this until we figure out reach costs for compound defenses
            if (diceAllocated < controlledCreature->getCombatPool() / 2 && isLastTempo == false) {
                priority += 10;
                priority -= reachCost;
                toPush.dice = diceAllocated + random_static::get(2, 4);
            } else {
                priority = cLowestPriority;
            }
        }

        break;
        case eDefensiveManuevers::Dodge: {
            if (controlledCreature->primaryWeaponDisabled() && weapon->getNaturalWeapon()) {
                priority++;
            } else {
                priority--;
            }
            int dice = std::min(diceAllocated + random_static::get(0, diceAllocated / 3)
                    - random_static::get(0, diceAllocated / 4),
                controlledCreature->getCombatPool());
            toPush.dice = dice;
        } break;
        case eDefensiveManuevers::Expulsion: {
            constexpr int buffer = 3;
            if (diceAllocated + buffer < controlledCreature->getCombatPool()) {
                if (isLastTempo) {
                    priority += 5;
                    toPush.dice = controlledCreature->getCombatPool();
                } else {
                    int dice = std::min(diceAllocated + random_static::get(0, diceAllocated / 3)
                            - random_static::get(0, diceAllocated / 4),
                        controlledCreature->getCombatPool());
                    // if its not too much dice, favor doing this
                    if (dice + random_static::get(cFuzz, cFuzz * 2)
                        < controlledCreature->getCombatPool()) {
                        priority += 10;
                    }
                    dice = max(dice, 3);
                    toPush.dice = dice;
                }
            }
        } break;
        case eDefensiveManuevers::Counter:
            break;
        case eDefensiveManuevers::AttackFromDef: {

            if (isLastTempo == false) {
                priority = cLowestPriority;
            } else if (controlledCreature->getCombatPool() > reachCost + 3) {
                // if incoming attack we can take, then just attack
                if (attack == eOffensiveManuevers::Hook && diceAllocated < 9) {
                    priority = 25;
                }
            }
            if (attack == eOffensiveManuevers::NoOffense
                && controlledCreature->getCombatPool() > 1) {
                priority = 25;
            } else {
                priority = cLowestPriority;
            }
        }

        break;
        case eDefensiveManuevers::StealInitiative: {
            int stealDie = 0;
            if (stealInitiative(controlledCreature, attacker, toPush.cost, stealDie)) {
                if (stealDie + reachCost < controlledCreature->getCombatPool()) {
                    toPush.dice = stealDie;
                    priority += 20;
                    if (weapon->getNaturalWeapon()) {
                        priority -= 10;
                    }
                } else {
                    priority = cLowestPriority;
                }

            } else {
                priority = cLowestPriority;
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
                controlledCreature, current.defManuever, instance->getCurrentReach(), current.cost);
            current.dice = min(current.dice, controlledCreature->getCombatPool());
            current.dice = max(0, current.dice);
            controlledCreature->setDefenseDice(current.dice);
            controlledCreature->reduceCombatPool(current.dice);
            break;
        }
        manueverPriorities.pop();
    }

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

void AICombatController::doPrecombat(
    Creature* controlledCreature, const Creature* opponent, const CombatInstance* instance)
{
    // do favoring
    constexpr int cMinFavorDie = 5;
    if (controlledCreature->canFavor() && instance->getLastTempo() == false
        && controlledCreature->getCombatPool() > cMinFavorDie) {
        // map is guaranteed to be ordered (smallest first)
        map<int, eHitLocations> possibleFavorLocations;

        int possibleDamage = opponent->getPrimaryWeapon()->getBestAttack()->getDamage();
        for (auto location : controlledCreature->getHitLocations()) {
            ArmorSegment segment = controlledCreature->getMedianArmor(location, false);
            if (segment.isMetal == false) {
                int av = segment.AV - possibleDamage;

                // only guard if its an attack we think we can't take
                if (av < 0) {
                    auto it = possibleFavorLocations.find(av);
                    if (it == possibleFavorLocations.end()) {
                        possibleFavorLocations[av] = location;
                    } else {
                        if (m_hitPriorities[location] > m_hitPriorities[it->second]) {
                            possibleFavorLocations[av] = location;
                        }
                    }
                }
            }
        }

        if (possibleFavorLocations.size() > 0) {
            // todo: replace with const
            controlledCreature->reduceCombatPool(1);
            controlledCreature->addFavored(possibleFavorLocations.begin()->second);
        }
    }

    const Weapon* weapon = controlledCreature->getPrimaryWeapon();
    if (opponent->hasEnoughMetalArmor()) {
        bool hasCrushing = false;
        if (weapon->getType() == eWeaponTypes::Polearms) {
            // with swingy blunt polearms like the pollaxe, we don't need to always switch to staff
            // grip cause the hammer hurts
            auto component = weapon->getBestBlunt();
            if (component != nullptr) {
                if (component->getProperties().find(eWeaponProperties::Crushing)
                    != component->getProperties().end()) {
                    hasCrushing = true;
                }
            }
        }
        if (hasCrushing == false) {
            // shorter grip == better against armor usually
            shortenGrip(controlledCreature, instance->getLastTempo());
        }
    } else if (instance->getCurrentReach() > controlledCreature->getCurrentReach()) {
        // switch back to normal grip if its not a problem
        normalizeGrip(controlledCreature, instance->getLastTempo());
    }
    if (controlledCreature->primaryWeaponDisabled()
        && controlledCreature->getGrip() == eGrips::Standard
        && controlledCreature->getSecondaryWeaponId() == controlledCreature->getNaturalWeaponId()) {
        // only do this if we didn't have an overwhelming reach advantage
        int opponentReachCost
            = calculateReachCost(opponent->getCurrentReach(), instance->getCurrentReach());
        if (opponentReachCost < 3) {
            // unlocks a secondary weapon if we didn't already have one
            shortenGrip(controlledCreature, instance->getLastTempo());
        }
    }
    if (instance->getCurrentReach() < controlledCreature->getCurrentReach()
        && controlledCreature->getGrip() == eGrips::Standard) {
        shortenGrip(controlledCreature, instance->getLastTempo());
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

void AICombatController::doPreresolution(
    Creature* controlledCreature, const Creature* opponent, const CombatInstance* instance)
{
    int shrewdDiff = controlledCreature->getShrewdness() - opponent->getShrewdness();
    int opponentDefenseDie = opponent->getQueuedDefense().dice;
    int die = controlledCreature->getQueuedOffense().dice;
    if (m_doFeint || opponentDefenseDie < die) {
        m_doFeint = false;
        if (getFeintCost() + 1 < controlledCreature->getCombatPool()) {
            controlledCreature->setCreatureFeint();
            controlledCreature->setOffenseFeintDice(controlledCreature->getCombatPool());
            chooseOffenseManuever(controlledCreature, opponent, instance, true, false, true);

            if (opponent->getQueuedDefense().manuever != eDefensiveManuevers::StealInitiative
                && opponent->getQueuedDefense().manuever != eDefensiveManuevers::AttackFromDef) {
                controlledCreature->reduceCombatPool(controlledCreature->getCombatPool());
            }
        }
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
    Creature* controlledCreature, const Creature* attacker, int cost, int& outDie)
{
    int diceAllocated = attacker->getQueuedOffense().dice;

    int combatPool = attacker->getCombatPool() / 2 + getTap(attacker->getMobility());
    combatPool += attacker->getQueuedOffense().manuever == eOffensiveManuevers::Thrust ? 1 : 0;
    float maxDiff = cMaxBTN - cBaseBTN;
    float attackerDisadvantage = (maxDiff - (attacker->getBTN() - cBaseBTN)) / maxDiff;
    float myDisadvantage = (maxDiff - (controlledCreature->getBTN() - cBaseBTN)) / maxDiff;

    // make sure this is enough for an attack + overcoming advantage
    int bufferDie = random_static::get(3, 7);
    int reachCost = max(attacker->getCurrentReach() - controlledCreature->getCurrentReach(), 0);
    bufferDie += reachCost + cost;
    if ((combatPool) + bufferDie
        < (controlledCreature->getCombatPool() + getTap(controlledCreature->getMobility()))) {
        float mult = 1.0;
        // favor in my tn difference
        mult += (controlledCreature->getBTN() - cBaseBTN) / 10.f;
        cout << mult << endl;
        int diff = attacker->getCombatPool() * mult;
        int dice = diff + random_static::get(3, 8) + getTap(attacker->getMobility())
            - getTap(controlledCreature->getMobility());
        if (controlledCreature->getCombatPool() - bufferDie >= dice) {
            outDie = dice;
            return true;
        }
    }
    return false;
}

eHitLocations AICombatController::getBestHitLocation(const Creature* target,
    const Component* component, bool feint, eHitLocations originalLocation, int& outDamage)
{
    map<eHitLocations, int> locations = getHitLocationCost(target, feint, originalLocation);
    assert(locations.size() > 1);
    eHitLocations ret = locations.begin()->first;
    // lower than anything possible
    int highestDamage = -50;
    for (auto location : locations) {
        ArmorSegment segment
            = target->getMedianArmor(location.first, component->getAttack() == eAttacks::Swing);
        if (component->getType() != eDamageTypes::Blunt && segment.isMetal) {
            if ((component->hasPlatePiercing() == false)
                || (segment.type == eArmorTypes::Maille
                    && component->hasMaillePiercing() == false)) {
                segment.AV *= 2;
                if (component->getType() == eDamageTypes::Piercing) {
                    segment.AV -= 2;
                }
            }
        }

        int damage = component->getDamage() - segment.AV - (location.second / 2);
        if (component->hasProperty(eWeaponProperties::Crushing)
            && location.first == eHitLocations::Head) {
            damage++;
        }
        if (damage > highestDamage) {
            highestDamage = damage;
            ret = location.first;
        }
    }
    outDamage = highestDamage;
    return ret;
}
