#include <iostream>

#include "../3rdparty/random.hpp"
#include "../items/utils.h"
#include "aicombatcontroller.h"
#include "combatinstance.h"
#include "combatmanager.h"

using namespace std;
using namespace effolkronium;

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
        doOffense(controlledCreature, instance->getDefender(), reachCost,
            instance->getCurrentReach(), instance->getLastTempo(), instance->getDualRedThrow());
        return;
    }
    if (instance->getState() == eCombatState::StolenOffense
        && instance->getAttacker()->getId() == creatureId) {
        doStolenInitiative(controlledCreature, instance->getDefender(), false);
    }
    if (instance->getState() == eCombatState::Defense
        && instance->getDefender()->getId() == creatureId) {
        doDefense(controlledCreature, instance->getAttacker(), instance->getLastTempo());
        return;
    }
    if (instance->getState() == eCombatState::AttackFromDefense
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getDefender(), reachCost,
            instance->getCurrentReach(), true, false);
        return;
    }
    if (instance->getState() == eCombatState::StealInitiative
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost,
            instance->getCurrentReach(), true, false);
        return;
    }
    if (instance->getState() == eCombatState::ParryLinked
        && instance->getDefender()->getId() == creatureId) {
        // do not pay costs
        doOffense(controlledCreature, instance->getAttacker(), reachCost,
            instance->getCurrentReach(), false, false, false);
        return;
    }

    if (instance->getState() == eCombatState::DualOffenseStealInitiative
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost,
            instance->getCurrentReach(), false, true);
    }
    if (instance->getState() == eCombatState::DualOffense1
        && instance->getAttacker()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getDefender(), reachCost,
            instance->getCurrentReach(), false, true);
    }
    if (instance->getState() == eCombatState::DualOffense2
        && instance->getDefender()->getId() == creatureId) {
        doOffense(controlledCreature, instance->getAttacker(), reachCost,
            instance->getCurrentReach(), false, true);
    }
    if (instance->getState() == eCombatState::DualOffenseSecondInitiative
        && instance->getDefender()->getId() == creatureId) {
        doStolenInitiative(controlledCreature, instance->getAttacker(), true);
    }
}

bool AICombatController::setCreatureOffenseManuever(
    Creature* controlledCreature, eOffensiveManuevers manuever, eLength currentReach)
{
    int cost = getOffensiveManueverCost(manuever, controlledCreature->getGrip(),
        controlledCreature->getPrimaryWeapon(), currentReach);
    bool canUse = (cost <= controlledCreature->getCombatPool());
    if (canUse) {
        controlledCreature->setOffenseManuever(manuever);
        controlledCreature->reduceCombatPool(cost);
    }
    return canUse;
}

bool AICombatController::setCreatureDefenseManuever(
    Creature* controlledCreature, eDefensiveManuevers manuever)
{
    int cost = getDefensiveManueverCost(manuever, controlledCreature->getGrip());
    bool canUse = (cost <= controlledCreature->getCombatPool());
    if (canUse) {
        controlledCreature->setDefenseManuever(manuever);
        controlledCreature->reduceCombatPool(cost);
    }
    return canUse;
}

void AICombatController::doOffense(Creature* controlledCreature, const Creature* target,
    int reachCost, eLength currentReach, bool allin, bool dualRedThrow, bool payCosts)
{
    cout << "allin : " << allin << endl;
    const Weapon* weapon = nullptr;

    if (target->getCombatPool() <= 0) {
        allin = true;
    }

    controlledCreature->setCreatureOffenseManuever(eOffensiveManuevers::Thrust, currentReach);
    if (controlledCreature->primaryWeaponDisabled() == false) {
        controlledCreature->setOffenseWeapon(true);
        weapon = controlledCreature->getPrimaryWeapon();
    } else {
        controlledCreature->setOffenseWeapon(false);
        weapon = controlledCreature->getSecondaryWeapon();
    }
    controlledCreature->setOffenseComponent(weapon->getBestAttack());
    if (controlledCreature->getQueuedOffense().component->getAttack() == eAttacks::Swing) {
        controlledCreature->setCreatureOffenseManuever(eOffensiveManuevers::Swing, currentReach);
    }
    // randomly beat
    if (target->primaryWeaponDisabled() == false && random_static::get(0, 3) < 1) {
        controlledCreature->setCreatureOffenseManuever(eOffensiveManuevers::Beat, currentReach);
    }
    // replace me
    controlledCreature->setOffenseTarget(target->getHitLocations()[random_static::get(
        0, static_cast<int>(target->getHitLocations().size()) - 1)]);

    // get least armored location
    if (target->hasEnoughMetalArmor()) {
        // full of metal armor, so lets do some fancy shit
        if (weapon->canHook() == true && target->getStance() != eCreatureStance::Prone) {
            // try to trip
            setCreatureOffenseManuever(controlledCreature, eOffensiveManuevers::Hook, currentReach);
        } else if (weapon->getType() == eWeaponTypes::Polearms) {
            // temporary
            if (setCreatureOffenseManuever(
                    controlledCreature, eOffensiveManuevers::PinpointThrust, currentReach)) {
                eHitLocations location;
                eBodyParts part;
                target->getLowestArmorPart(&part, &location);
                controlledCreature->setOffenseComponent(weapon->getBestThrust());
                controlledCreature->setOffensePinpointTarget(part);
                controlledCreature->setOffenseTarget(location);
            } else {
                setCreatureOffenseManuever(
                    controlledCreature, eOffensiveManuevers::Hook, currentReach);
            }
        } else if (weapon->getType() == eWeaponTypes::Longswords) {
            // temporary
            if (setCreatureOffenseManuever(
                    controlledCreature, eOffensiveManuevers::PinpointThrust, currentReach)) {
                eHitLocations location;
                eBodyParts part;
                target->getLowestArmorPart(&part, &location);
                controlledCreature->setOffenseComponent(weapon->getBestThrust());
                controlledCreature->setOffensePinpointTarget(part);
                controlledCreature->setOffenseTarget(location);
            } else {
                // todo : mordhau
                if (setCreatureOffenseManuever(
                        controlledCreature, eOffensiveManuevers::Mordhau, currentReach)) {
                    controlledCreature->setOffenseComponent(weapon->getPommelStrike());
                }
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
                controlledCreature->setOffenseTarget(location);
                highestUnarmoredLocations = unarmoredLocations;
            }
            constexpr int bufferDice = 5;
            if ((target->getCombatPool() + bufferDice < controlledCreature->getCombatPool())
                || (target->getBTN() > controlledCreature->getBTN())) {
                // we have enough of an advantage to do anything we want
                // but if head and chest are fully armored and we don't have a blunt damage attack
                // then thats bad
            }
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
    if (weapon->getLength() > target->getPrimaryWeapon()->getLength()) {
        dice += weapon->getLength() - target->getPrimaryWeapon()->getLength();
    }

    // bound
    dice += reachCost;
    dice = max(0, dice);
    dice = min(dice, controlledCreature->getCombatPool());
    // never issue 0 dice for attack
    if (controlledCreature->getCombatPool() > 0 && dice == 0) {
        dice = 1;
    }
    if (allin == true) {
        controlledCreature->setOffenseDice(controlledCreature->getCombatPool());
    } else {
        controlledCreature->setOffenseDice(dice);
    }

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

void AICombatController::doDefense(
    Creature* controlledCreature, const Creature* attacker, bool isLastTempo)
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
    constexpr int buffer = 3;
    if ((diceAllocated + buffer < controlledCreature->getCombatPool()
            && random_static::get(0, 2) == 0)
        || (isLastTempo && diceAllocated + buffer < controlledCreature->getCombatPool())) {
        if (random_static::get(0, 3) == 0) {
            setCreatureDefenseManuever(controlledCreature, eDefensiveManuevers::ParryLinked);
        } else {
            setCreatureDefenseManuever(controlledCreature, eDefensiveManuevers::Expulsion);
        }

    } else {
        // parry or dodge
        setCreatureDefenseManuever(controlledCreature, eDefensiveManuevers::Parry);
    }
    if (isLastTempo == true) {
        // use all dice because we're going to refresh anyway
        int defDie = max(controlledCreature->getCombatPool(), 0);
        controlledCreature->setDefenseDice(defDie);
        controlledCreature->reduceCombatPool(defDie);
        controlledCreature->setDefenseReady();
        return;
    }
    int stealDie = 0;
    if (stealInitiative(controlledCreature, attacker, stealDie) == true) {
        controlledCreature->setDefenseManuever(eDefensiveManuevers::StealInitiative);
        controlledCreature->setDefenseDice(stealDie);
        assert(stealDie <= controlledCreature->getCombatPool() || stealDie == 0);
        controlledCreature->reduceCombatPool(stealDie);
        controlledCreature->setDefenseReady();
        return;
    }

    int dice = std::min(diceAllocated + random_static::get(0, diceAllocated / 3)
            - random_static::get(0, diceAllocated / 4),
        controlledCreature->getCombatPool());
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
