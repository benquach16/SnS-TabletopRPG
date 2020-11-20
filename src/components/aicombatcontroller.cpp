#include <iostream>

#include "../3rdparty/random.hpp"
#include "../items/utils.h"
#include "aicombatcontroller.h"
#include "combatinstance.h"
#include "combatmanager.h"

using namespace std;
using namespace effolkronium;

void AICombatController::run(const CombatManager* manager, Creature* controlledCreature)
{
    assert(manager != nullptr);

    CombatEdge* edge = manager->getCurrentEdge();
    if (edge == nullptr) {
        return;
    }
    CombatInstance* instance = edge->getInstance();

    if (instance->getState() == eCombatState::Uninitialized) {
        return;
    }

    if (instance->getState() == eCombatState::Offense
        && instance->getAttacker()->getId() == controlledCreature->getId()) {

        return;
    }
    if (instance->getState() == eCombatState::Defense
        && instance->getDefender()->getId() == controlledCreature->getId()) {

        return;
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
        controlledCreature->setOffenseWeapon(true);
        weapon = controlledCreature->getPrimaryWeapon();
    } else {
        controlledCreature->setOffenseWeapon(false);
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
        return;
    }
    int stealDie = 0;
    if (stealInitiative(attacker, stealDie) == true) {
        controlledCreature->setDefenseManuever(eDefensiveManuevers::StealInitiative);
        controlledCreature->setDefenseDice(stealDie);
        assert(stealDie <= controlledCreature->getCombatPool() || stealDie == 0);
        controlledCreature->reduceCombatPool(stealDie);
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
