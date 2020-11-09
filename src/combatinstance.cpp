
// I have lots of redundant code, refactor me!

#include <assert.h>
#include <iostream>

#include "3rdparty/random.hpp"
#include "combatinstance.h"
#include "creatures/player.h"
#include "creatures/utils.h"
#include "game.h"
#include "items/utils.h"

using namespace std;

constexpr int cMinFatigueTempos = 0;

CombatInstance::CombatInstance()
    : m_initiative(eInitiative::Side1)
    , m_side1(nullptr)
    , m_side2(nullptr)
    , m_currentTempo(eTempo::First)
    , m_currentState(eCombatState::Uninitialized)
    , m_currentReach(eLength::Hand)
    , m_dualWhiteTimes(0)
    , m_dualRedThrow(false)
    , m_numTempos(0)
    , m_inWind(false)
    , m_inGrapple(false)
    , m_sendAllMessages(false)
{
}

void CombatInstance::setSides(Creature*& attacker, Creature*& defender)
{
    if (m_initiative == eInitiative::Side1) {
        attacker = m_side1;
        defender = m_side2;
    } else if (m_initiative == eInitiative::Side2) {
        attacker = m_side2;
        defender = m_side1;
    }
    assert(attacker != nullptr);
    assert(defender != nullptr);
}

void CombatInstance::initCombat(Creature* side1, Creature* side2, bool showAllMessages)
{
    assert(side1 != nullptr);
    assert(side2 != nullptr);
    assert(m_currentState == eCombatState::Uninitialized);

    m_sendAllMessages = showAllMessages;

    m_side1 = side1;
    m_side2 = side2;

    // m_side1->clearCreatureManuevers();
    // m_side2->clearCreatureManuevers();

    m_currentTempo = eTempo::First;
    m_initiative = eInitiative::Side1;
    m_currentState = eCombatState::Initialized;
    m_numTempos = 0;
}

void CombatInstance::forceInitiative(eInitiative initiative)
{
    m_initiative = initiative;
    m_currentState = eCombatState::Offense;
}

void CombatInstance::doInitialization()
{
    assert(m_side1 != nullptr);
    assert(m_side2 != nullptr);
    assert(m_currentState == eCombatState::Initialized);
    m_side1->resetCombatPool();
    m_side2->resetCombatPool();
    writeMessage(m_side1->getName() + " is using " + m_side1->getPrimaryWeapon()->getName()
        + " and " + m_side2->getName() + " is using " + m_side2->getPrimaryWeapon()->getName());
    // ugly implicit casting
    m_currentReach = max(m_side1->getCurrentReach(), m_side2->getCurrentReach());
    m_dualWhiteTimes = 0;
    m_currentState = eCombatState::RollInitiative;
}

void CombatInstance::doRollInitiative()
{
    if (m_side1->isPlayer() == true) {
        Player* player = static_cast<Player*>(m_side1);
        if (player->pollForInitiative() == false) {
            m_currentState = eCombatState::RollInitiative;
            return;
        }
    }

    // get initiative rolls from both sides to determine roles.
    eInitiativeRoll side1 = m_side1->doInitiative(m_side2);
    eInitiativeRoll side2 = m_side2->doInitiative(m_side1);
    if (m_side1->isPlayer() == true) {
        Player* player = static_cast<Player*>(m_side1);
        side1 = player->getInitiative();
    }

    if (side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Defend) {
        // repeat
        writeMessage("Both sides chose to defend, deciding initiative again");
        if (m_dualWhiteTimes > 1) {
            writeMessage("Defense chosen too many times, initiative going to "
                         "willpower contest");
            int side1Successes
                = DiceRoller::rollGetSuccess(m_side1->getBTN(), m_side1->getWillpower());
            int side2Successes
                = DiceRoller::rollGetSuccess(m_side2->getBTN(), m_side2->getWillpower());

            m_initiative
                = side1Successes < side2Successes ? eInitiative::Side1 : eInitiative::Side2;

            if (m_initiative == eInitiative::Side1) {
                writeMessage(m_side1->getName() + " takes initiative");
            } else {
                writeMessage(m_side2->getName() + " takes initiative");
            }
            m_currentState = eCombatState::PreexchangeActions;
            return;
        }
        m_dualWhiteTimes++;
        m_currentState = eCombatState::ResetState;
        return;
    } else if (side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Defend) {
        writeMessage(
            m_side1->getName() + " chose to attack and " + m_side2->getName() + " is defending");
        m_initiative = eInitiative::Side1;
        m_currentState = eCombatState::PreexchangeActions;
        return;
    } else if (side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Attack) {
        writeMessage(
            m_side2->getName() + " chose to attack and " + m_side1->getName() + " is defending");
        m_initiative = eInitiative::Side2;
        m_currentState = eCombatState::PreexchangeActions;
        return;
    } else if (side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Attack) {
        writeMessage("Both sides chose to attack, no defense can be done by "
                     "either side.");
        // no defense here!
        // hopefully you don't die horribly
        // roll speed to determine who goes first
        int side1Successes = DiceRoller::rollGetSuccess(m_side1->getBTN(), m_side1->getMobility());
        int side2Successes = DiceRoller::rollGetSuccess(m_side2->getBTN(), m_side2->getMobility());

        m_initiative = side1Successes < side2Successes ? eInitiative::Side1 : eInitiative::Side2;

        if (m_initiative == eInitiative::Side1) {
            writeMessage(m_side1->getName() + " declares their attack first");
        } else {
            writeMessage(m_side2->getName() + " declares their attack first");
        }
        m_dualRedThrow = true;
        m_currentState = eCombatState::PreexchangeActions;
        return;
    }

    m_currentState = eCombatState::RollInitiative;
}

void CombatInstance::doPreexchangeActions()
{
    if (m_side1->isPlayer() == true) {
        if (m_side1->getHasPrecombat() == false) {
            m_currentState = eCombatState::PreexchangeActions;
            return;
        }
    } else {
        m_side1->doPrecombat(m_side2);
    }

    m_side2->doPrecombat(m_side1);

    // check if grip causes combatants to move closer
    eLength length = max(m_side1->getCurrentReach(), m_side2->getCurrentReach());
    if (length < m_currentReach) {
        m_currentReach = length;
    }
    m_currentState = eCombatState::PositionActions;
}

void CombatInstance::doPosition()
{
    if (m_side1->isPlayer() == true) {

    } else {
    }
    if (m_dualRedThrow == true) {
        m_currentState = eCombatState::DualOffense1;
    } else {
        m_currentState = eCombatState::Offense;
    }
}

void CombatInstance::doResetState() { m_currentState = eCombatState::RollInitiative; }

bool CombatInstance::doOffense()
{
    // get offensive manuever and dice from side 1
    // then get defensive manuever and dice from side 2
    // then resolve
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    if (attacker->getHasOffense() == false) {
        if (attacker->getCombatPool() <= 0 && defender->getCombatPool() > 0) {
            writeMessage(
                attacker->getName() + " has no more action points! Initiative swaps to defender");
            switchInitiative();
            setSides(attacker, defender);
        }
        if (attacker->getCombatPool() <= 0 && defender->getCombatPool() <= 0) {
            writeMessage("Neither side has any action points left, starting "
                         "new exchange and resetting combat pools. It is now first tempo");
            m_currentTempo = eTempo::First;
            attacker->resetCombatPool();
            defender->resetCombatPool();
            attacker->clearCreatureManuevers(true);
            defender->clearCreatureManuevers(true);
        }
    }

    int reachCost = calculateReachCost(m_currentReach, attacker->getCurrentReach());

    if (attacker->isPlayer() == true) {
        // wait until we get input from player
        Player* player = static_cast<Player*>(attacker);
        if (player->getHasOffense() == false) {
            m_currentState = eCombatState::Offense;
            return false;
        }
    } else {
        bool allin = m_currentTempo == eTempo::Second;
        attacker->doOffense(defender, reachCost, m_currentReach, allin, m_dualRedThrow);
    }
    if (attacker->getHasOffense() == false) {
        m_currentState = eCombatState::Offense;
        return false;
    }
    Offense attack = attacker->getQueuedOffense();
    const Weapon* offenseWeapon = getAttackingWeapon(attacker);
    outputReachCost(reachCost, attacker);
    assert(attack.component != nullptr);

    attacker->addAndResetBonusDice();

    writeMessage(attacker->getName() + " " + offensiveManueverToString(attack.manuever) + "s with "
        + offenseWeapon->getName() + " at " + hitLocationToString(attack.target) + " using "
        + attack.component->getName() + " with " + to_string(attacker->getQueuedOffense().dice)
        + " action points");

    m_currentState = eCombatState::Defense;
    return true;
}

void CombatInstance::doDualOffense1()
{
    // both sides rolled red
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    // person who rolled better on speed goes second
    // allow stealing of initiative here
    if (doOffense() == false) {
        m_currentState = eCombatState::DualOffense1;
        return;
    }

    Defense defense = attacker->getQueuedDefense();
    if (defense.manuever == eDefensiveManuevers::StealInitiative
        && attacker->getHasDefense() == true) {
        writeMessage(
            attacker->getName() + " allocates " + to_string(defense.dice) + " to steal initiative");
        m_currentState = eCombatState::DualOffenseStealInitiative;
        return;
    }
    switchInitiative();

    m_currentState = eCombatState::DualOffense2;
}

void CombatInstance::doDualOffenseStealInitiative()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    int reachCost = calculateReachCost(m_currentReach, defender->getCurrentReach());

    if (defender->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(defender);

        // causes issues with new implementation
        if (player->getHasOffense() == false) {
            m_currentState = eCombatState::DualOffenseStealInitiative;
            return;
        }

    } else {
        // confusing nomenclature
        defender->doOffense(attacker, reachCost, m_currentReach, false, true);
    }

    Offense offense = defender->getQueuedOffense();

    outputReachCost(reachCost, defender);

    Defense defense = defender->getQueuedDefense();

    const Weapon* offenseWeapon = getAttackingWeapon(defender);

    writeMessage(defender->getName() + " allocates " + to_string(defense.dice) + " for initiative");
    writeMessage(defender->getName() + " " + offensiveManueverToString(offense.manuever) + "s with "
        + offenseWeapon->getName() + " at " + hitLocationToString(offense.target) + " using "
        + offense.component->getName() + " with " + to_string(offense.dice) + " action points");
    switchInitiative();
    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doDualOffense2()
{
    // both sides rolled red
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    if (doOffense() == false) {
        m_currentState = eCombatState::DualOffense2;
        return;
    }
    Defense defense = attacker->getQueuedDefense();
    if (defense.manuever == eDefensiveManuevers::StealInitiative
        && attacker->getHasDefense() == true) {
        writeMessage(
            attacker->getName() + " allocates " + to_string(defense.dice) + " for initiative");
        // defender already declared initiative roll, just go to resolution
        if (defender->getQueuedDefense().manuever == eDefensiveManuevers::StealInitiative
            && defender->getHasDefense() == true) {
            m_currentState = eCombatState::Resolution;
            return;
        }
        m_currentState = eCombatState::DualOffenseSecondInitiative;
        return;
    }

    m_currentState = eCombatState::DualOffenseResolve;
}

void CombatInstance::doDualOffenseSecondInitiative()
{
    // both sides rolled red
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    // defender is person who went first, they need to respond with dice
    // allocation
    if (defender->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(defender);
        if (player->getHasOffense() == false) {
            m_currentState = eCombatState::DualOffenseSecondInitiative;
            return;
        }
    } else {
        // confusing nomenclature
        defender->doStolenInitiative(attacker, true);
    }

    Defense defense = defender->getQueuedDefense();
    writeMessage(defender->getName() + " allocates " + to_string(defense.dice)
        + " action points for initiative");
    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doAttackFromDefense()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    int reachCost = calculateReachCost(m_currentReach, defender->getCurrentReach());

    if (defender->isPlayer() == true) {
        // wait until we get input from player
        Player* player = static_cast<Player*>(defender);
        if (player->getHasOffense() == false) {
            m_currentState = eCombatState::AttackFromDefense;
            return;
        }
    } else {
        attacker->doOffense(defender, reachCost, m_currentReach, true, m_dualRedThrow);
    }
    if (attacker->getHasOffense() == false) {
        m_currentState = eCombatState::AttackFromDefense;
        return;
    }

    outputReachCost(reachCost, defender);
    Offense attack = defender->getQueuedOffense();
    const Weapon* offenseWeapon = getAttackingWeapon(defender);
    assert(attack.component != nullptr);

    defender->addAndResetBonusDice();

    writeMessage(defender->getName() + " " + offensiveManueverToString(attack.manuever)
        + "s from defense with " + offenseWeapon->getName() + " at "
        + hitLocationToString(attack.target) + " using " + attack.component->getName() + " with "
        + to_string(defender->getQueuedOffense().dice) + " action points");

    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doDefense()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    if (defender->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(defender);
        if (player->getHasDefense() == false) {
            m_currentState = eCombatState::Defense;
            return;
        }

    } else {
        defender->doDefense(attacker, m_currentTempo == eTempo::Second);
    }

    Defense defend = defender->getQueuedDefense();
    if (defend.manuever == eDefensiveManuevers::StealInitiative) {
        // need both sides to attempt to allocate dice
        m_currentState = eCombatState::StealInitiative;
        return;
    }
    if (defend.manuever == eDefensiveManuevers::AttackFromDef) {
        m_currentState = eCombatState::AttackFromDefense;
        return;
    }
    const Weapon* defendingWeapon = getDefendingWeapon(defender);
    if (defend.manuever == eDefensiveManuevers::ParryLinked) {
        writeMessage(defender->getName() + " performs a block and strike with "
            + defendingWeapon->getName() + " using " + to_string(defend.dice) + " action points");
        m_currentState = eCombatState::ParryLinked;
        return;
    }

    writeMessage(defender->getName() + " attempts " + defensiveManueverToString(defend.manuever)
        + " with " + defendingWeapon->getName() + " using " + to_string(defend.dice)
        + " action points");

    m_currentState = eCombatState::PreResolution;
}

void CombatInstance::doParryLinked()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    int reachCost = defender->getCurrentReach() - m_currentReach;
    reachCost = abs(reachCost);
    if (defender->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(defender);
        if (player->getHasOffense() == false) {
            m_currentState = eCombatState::ParryLinked;
            return;
        }

    } else {

        defender->doOffense(
            attacker, reachCost, m_currentReach, m_currentTempo == eTempo::Second, false, false);
    }
    Offense offense = defender->getQueuedOffense();

    writeMessage(defender->getName() + " prepares to attack with " + offense.component->getName()
        + " at " + hitLocationToString(offense.target));
    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doStealInitiative()
{
    // defender inputs offense and dice to steal initiative
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    // then input manuever
    Defense defend = defender->getQueuedDefense();
    int reachCost = defender->getCurrentReach() - m_currentReach;
    reachCost = abs(reachCost);
    // do dice to steal initiative first
    // this polls for offense since the initiative steal is stored inside the
    // defense struct
    if (defender->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(defender);
        if (player->getHasOffense() == false) {
            m_currentState = eCombatState::StealInitiative;
            return;
        }

    } else {
        defender->doOffense(attacker, reachCost, m_currentReach, true);
    }

    writeMessage(defender->getName() + " attempts to steal intiative using "
        + to_string(defend.dice) + " action points!");

    m_currentState = eCombatState::StolenOffense;
}

void CombatInstance::doStolenOffense()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    if (attacker->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(attacker);
        if (player->getHasDefense() == false) {
            m_currentState = eCombatState::StolenOffense;
            return;
        }
    } else {
        attacker->doStolenInitiative(defender);
    }

    writeMessage(attacker->getName() + " allocates " + to_string(attacker->getQueuedDefense().dice)
        + " action points to contest initiative steal");
    int reachCost = calculateReachCost(defender->getCurrentReach(), m_currentReach);
    reachCost = abs(reachCost);
    outputReachCost(reachCost, defender);

    writeMessage(defender->getName() + " "
        + offensiveManueverToString(defender->getQueuedOffense().manuever) + "s with "
        + getAttackingWeapon(defender)->getName() + " at "
        + hitLocationToString(defender->getQueuedOffense().target) + " using "
        + defender->getQueuedOffense().component->getName() + " with "
        + to_string(defender->getQueuedOffense().dice) + " action points");
    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doPreResolution()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    if (attacker->isPlayer() == true) {
        // wait until player inputs
        Player* player = static_cast<Player*>(attacker);
        if (player->getHasPreResolution() == false) {
            m_currentState = eCombatState::PreResolution;
            return;
        }
    } else {
        attacker->doPreresolution(defender);
    }

    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doResolution()
{
    cout << "Resolving combat" << endl;
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    Offense attack = attacker->getQueuedOffense();
    Defense defend = defender->getQueuedDefense();

    // determine who was originally attacking
    if (defend.manuever == eDefensiveManuevers::StealInitiative
        || defend.manuever == eDefensiveManuevers::AttackFromDef) {
        // original attacker gets advantage
        // ptr compares bad
        int side1BTN = (m_side1->getId() == attacker->getId() && m_dualRedThrow == false)
            ? m_side1->getAdvantagedBTN()
            : m_side1->getBTN();
        int side2BTN = (m_side2->getId() == attacker->getId() && m_dualRedThrow == false)
            ? m_side2->getAdvantagedBTN()
            : m_side2->getBTN();

        // special case for thrust manuever, get an extra die
        constexpr unsigned cThrustDie = 1;
        // divide dice allocation by 2
        int side1Dice = (m_side1->getQueuedOffense().manuever == eOffensiveManuevers::Thrust)
            ? (m_side1->getQueuedDefense().dice + 1) / 2 + cThrustDie
            : (m_side1->getQueuedDefense().dice + 1) / 2;
        int side2Dice = (m_side2->getQueuedOffense().manuever == eOffensiveManuevers::Thrust)
            ? (m_side2->getQueuedDefense().dice + 1) / 2 + cThrustDie
            : (m_side2->getQueuedDefense().dice + 1) / 2;

        int side1InitiativeSuccesses
            = DiceRoller::rollGetSuccess(side1BTN, side1Dice + getTap(m_side1->getMobility()));
        int side2InitiativeSuccesses
            = DiceRoller::rollGetSuccess(side2BTN, side2Dice + getTap(m_side2->getMobility()));

        if (defend.manuever == eDefensiveManuevers::StealInitiative) {
            // only steal initiative can change initiative
            if (side1InitiativeSuccesses > side2InitiativeSuccesses) {
                m_initiative = eInitiative::Side1;
            } else if (side1InitiativeSuccesses < side2InitiativeSuccesses) {
                m_initiative = eInitiative::Side2;
            } else {
                // tie
                m_currentState = eCombatState::DualOffenseResolve;
                return;
            }
            setSides(attacker, defender);
            writeMessage(attacker->getName() + " has taken the initiative!");
        }

        cout << "attack roll" << endl;
        int attackerSuccesses
            = DiceRoller::rollGetSuccess(attacker->getBTN(), attacker->getQueuedOffense().dice);

        bool wasStanding = defender->getStance() == eCreatureStance::Standing;
        bool wasGrappled = false;
        if (attackerSuccesses > 0) {
            if (attacker->getQueuedOffense().manuever != eOffensiveManuevers::Grab) {
                if (inflictWound(
                        attacker, attackerSuccesses, attacker->getQueuedOffense(), defender)
                    == true) {
                    m_currentState = eCombatState::FinishedCombat;
                    return;
                }
            } else {
                // special grab
                startGrapple(attacker, defender);
                wasGrappled = true;
            }
        } else {
            writeMessage(attacker->getName() + " had no successes");
        }

        bool becameProne = wasStanding == true && defender->getStance() == eCreatureStance::Prone;

        if (becameProne == true) {
            // if the attack knocked them prone
            writeMessage(defender->getName()
                + " was knocked down by the attack, their attack cannot resolve.");
            m_currentState = eCombatState::Offense;
            m_currentReach = attacker->getCurrentReach();
        } else if (wasGrappled == true) {
            writeMessage(defender->getName() + " was grappled, their attack is interrupted!");
            m_currentReach = eLength::Hand;
        } else if (defender->getQueuedOffense().dice <= 0) {
            // if the attack wiped out their combat pool, do nothing
            writeMessage(defender->getName()
                + " had their action points eliminated by impact, their attack "
                  "can no longer resolve.");
            m_currentState = eCombatState::Offense;
            m_currentReach = attacker->getCurrentReach();
        } else if (defender->isWeaponDisabled() || defender->droppedWeapon()) {
            // if the attack disabled or caused their weapon to drop
            // if the attack wiped out their combat pool, do nothing
            writeMessage(defender->getName()
                + " had their weapon disabled! Their attack can no longer resolve.");
            m_currentState = eCombatState::Offense;
            m_currentReach = attacker->getCurrentReach();
        } else {
            int defendSuccesses
                = DiceRoller::rollGetSuccess(defender->getBTN(), defender->getQueuedOffense().dice);
            if (defendSuccesses > 0) {
                if (defender->getQueuedOffense().manuever != eOffensiveManuevers::Grab) {
                    if (inflictWound(
                            defender, defendSuccesses, defender->getQueuedOffense(), attacker)) {
                        m_currentState = eCombatState::FinishedCombat;
                        return;
                    }
                } else {
                    startGrapple(attacker, defender);
                }
            } else {
                writeMessage(defender->getName() + " had no successes");
            }
            m_currentReach = attacker->getCurrentReach();
            if (defendSuccesses > attackerSuccesses) {
                m_currentReach = defender->getCurrentReach();
                writeMessage(defender->getName() + " had more successes, taking initiative");
                switchInitiative();
            }
        }
    } else {
        // standard attacker-defender resolution
        // if stomp discard everything
        // roll dice
        if (attack.feint == true) {
            // resolve feint to change successes
            int attackerKeen
                = DiceRoller::rollGetSuccess(attacker->getBTN(), attacker->getShrewdness());
            int defenderKeen
                = DiceRoller::rollGetSuccess(defender->getBTN(), defender->getShrewdness());

            int keenDifference = attackerKeen - defenderKeen;
            if (keenDifference > 0) {
                writeMessage(attacker->getName() + " successfully feints and weakens "
                    + defender->getName() + "'s defense by " + to_string(keenDifference));
            } else {
                writeMessage(attacker->getName() + " attempted to feint but " + defender->getName()
                    + " was able to catch the feint in time!");
            }
            cout << "keen difference " << keenDifference << endl;
            keenDifference = max(0, keenDifference);
            defend.dice -= keenDifference;
        }
        int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), attack.dice);
        int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defend.dice);

        int MoS = offenseSuccesses - defenseSuccesses;
        cout << MoS << endl;
        if (MoS > 0) {
            // grab is special because it completely changes the state
            if (attack.manuever != eOffensiveManuevers::Grab) {
                // writeMessage("attack landed with " + to_string(MoS) + " successes");
                if (inflictWound(attacker, MoS, attack, defender) == true) {
                    m_currentState = eCombatState::FinishedCombat;
                    return;
                }
                m_currentReach = attacker->getCurrentReach();
            } else {
                startGrapple(attacker, defender);
            }
        } else if (MoS == 0) {
            writeMessage("no net successes, " + attacker->getName() + " retains initiative");
        } else {
            if (defend.manuever == eDefensiveManuevers::Dodge) {
                writeMessage("attack dodged with " + to_string(-MoS) + " successes");
            } else {
                writeMessage("attack parried with " + to_string(-MoS) + " successes");

                // natural weapon parries still take damage thru the parry
                if (defender->getPrimaryWeapon()->getNaturalWeapon() == true) {
                    attack.target = eHitLocations::Arm;
                    inflictWound(attacker, 0, attack, defender);
                }
            }
            if (defend.manuever == eDefensiveManuevers::Expulsion) {
                attacker->disableWeapon();
                attacker->inflictImpact(-MoS + 2);
                Log::push(
                    "Attacker's weapon disabled and " + to_string(-MoS + 2) + " impact inflicted!");
            }
            if (defend.manuever == eDefensiveManuevers::ParryLinked) {
                // resolve offense
                Offense offense = defender->getQueuedOffense();

                // disadvantage on the attack unless done with secondary weapon

                bool linked = false;
                if (offense.withPrimaryWeapon != defend.withPrimaryWeapon) {
                    linked = true;
                }
                int BTN = linked == true ? defender->getBTN() : defender->getDisadvantagedBTN();

                int reachCost = abs(defender->getCurrentReach() - m_currentReach);
                MoS = (-MoS) - reachCost;
                int linkedOffenseMoS = DiceRoller::rollGetSuccess(BTN, MoS);
                cout << "Linked hits: " << linkedOffenseMoS << endl;

                if (linkedOffenseMoS > 0
                    && inflictWound(defender, linkedOffenseMoS, offense, attacker) == true) {
                    m_currentState = eCombatState::FinishedCombat;
                    return;
                }
            }
            if (defend.manuever == eDefensiveManuevers::Counter) {
                cout << "bonus: " << offenseSuccesses << endl;
                defender->setBonusDice(offenseSuccesses);
                writeMessage(defender->getName() + " receives " + to_string(offenseSuccesses)
                    + " action points in their next attack");
            }
            if (defend.manuever != eDefensiveManuevers::Dodge) {
                writeMessage(defender->getName() + " now has initative, becoming attacker");
                switchInitiative();
            }
        }
    }

    if (m_side1->getHasPosition() == true) {
        resolvePosition(m_side1);
    }
    if (m_side2->getHasPosition() == true) {
        resolvePosition(m_side2);
    }
    m_currentState = eCombatState::PostResolution;
}

void CombatInstance::doDualOffenseResolve()
{
    writeMessage("Both attacks land at the same time!", Log::eMessageTypes::Announcement);
    // dual aggression
    Offense attack = m_side1->getQueuedOffense();
    Offense attack2 = m_side2->getQueuedOffense();

    int MoS = DiceRoller::rollGetSuccess(m_side1->getBTN(), attack.dice);
    int MoS2 = DiceRoller::rollGetSuccess(m_side2->getBTN(), attack2.dice);

    // resolve both
    bool death = false;
    if (MoS > 0) {
        if (inflictWound(m_side1, MoS, attack, m_side2) == true) {
            death = true;
        }
    } else {
        writeMessage(m_side1->getName() + " got no successes");
    }
    if (MoS2 > 0) {
        if (inflictWound(m_side2, MoS2, attack2, m_side1) == true) {
            death = true;
        }
    } else {
        writeMessage(m_side2->getName() + " got no successes");
    }

    // intiative goes to whoever got more hits
    m_currentState = eCombatState::PostResolution;
    if (MoS > MoS2) {
        m_currentReach = m_side1->getCurrentReach();
        m_initiative = eInitiative::Side1;
    } else if (MoS < MoS2) {
        m_currentReach = m_side2->getCurrentReach();
        m_initiative = eInitiative::Side2;
    } else {
        // random
        m_initiative
            = effolkronium::random_static::get(1, 2) == 2 ? eInitiative::Side2 : eInitiative::Side1;
    }

    m_currentState = death == true ? eCombatState::FinishedCombat : m_currentState;
}

void CombatInstance::doPostResolution()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    Defense defend = defender->getQueuedDefense();
    if (defend.manuever == eDefensiveManuevers::Dodge) {
        // allow using 2 dice to take initiative
    }

    // simulating some kind of pollaxe play thing here
    if (switchToStaffGrip(defender)) {
        defender->setGrip(eGrips::Staff);
    }
    // simulating some kind of pollaxe play thing here
    if (switchToStaffGrip(attacker)) {
        defender->setGrip(eGrips::Staff);
    }

    m_side1->clearCreatureManuevers();
    m_side2->clearCreatureManuevers();
    m_dualRedThrow = false;

    if (m_numTempos > cMinFatigueTempos) {
        // roll for fatigue
        cout << "fatigue roll" << endl;
        if (m_side1->rollFatigue() == true) {
            writeMessage(m_side1->getName() + " has gotten more tired", Log::eMessageTypes::Alert);
        }
        if (m_side2->rollFatigue() == true) {
            writeMessage(m_side2->getName() + " has gotten more tired", Log::eMessageTypes::Alert);
        }
    }
    m_numTempos++;

    // only preexchangeactions after a refresh
    if (m_currentTempo == eTempo::Second) {
        m_currentState = eCombatState::PreexchangeActions;
    } else {
        m_currentState = eCombatState::Offense;
    }
    // m_currentState = eCombatState::PreexchangeActions;
    switchTempo();
}

void CombatInstance::resolvePosition(Creature* creature)
{
    Position position = creature->getQueuedPosition();
    // don't roll, because not being able to stand or pick up weapon is frustrating
    int successes = position.dice;
    if (successes < 1) {
        writeMessage("Impact prevented " + creature->getName() + " from repositioning",
            Log::eMessageTypes::Alert);
        return;
    }
    if (position.manuever == ePositionManuevers::Pickup) {
        writeMessage(creature->getName() + " picked up dropped weapon");
        creature->enableWeapon();
    }
    if (position.manuever == ePositionManuevers::Stand) {
        writeMessage(creature->getName() + " stood up");
        creature->setStand();
    }
}

void CombatInstance::doEndCombat()
{
    writeMessage("Combat has ended", Log::eMessageTypes::Announcement);
    m_side1->clearCreatureManuevers();
    m_side2->clearCreatureManuevers();

    m_side1 = nullptr;
    m_side2 = nullptr;
    m_currentState = eCombatState::Uninitialized;
}

void CombatInstance::startGrapple(Creature* attacker, Creature* defender)
{
    writeMessage(attacker->getName() + " has started a grapple with " + defender->getName(),
        Log::eMessageTypes::Announcement);
    m_inGrapple = true;
    m_currentReach = eLength::Hand;
}

bool CombatInstance::inflictWound(Creature* attacker, int MoS, Offense attack, Creature* target)
{
    writeMessage(
        attacker->getName() + "'s attack landed with " + to_string(MoS) + " net successes!",
        Log::eMessageTypes::Announcement);
    if (attack.manuever == eOffensiveManuevers::Hook) {
        writeMessage(target->getName() + " loses " + to_string(MoS) + " action points from impact",
            Log::eMessageTypes::Alert);
        target->inflictImpact(MoS);
        if (MoS >= 3) {
            writeMessage(target->getName() + " has been hooked and thrown prone!",
                Log::eMessageTypes::Alert, true);
            target->setProne();
        }
        return false;
    }

    if (attack.manuever == eOffensiveManuevers::Disarm) {
        if (MoS >= 2) {
            writeMessage(target->getName() + " has been disarmed!", Log::eMessageTypes::Alert);
        } else {
            writeMessage(target->getName() + "'s weapon has been disabled for 1 tempo.",
                Log::eMessageTypes::Alert);
        }

        target->disableWeapon(MoS >= 2);
        return false;
    }

    if (attack.manuever == eOffensiveManuevers::Beat) {
        const int impact = MoS + 2;
        writeMessage(target->getName() + "'s weapon has been disabled for 1 tempo and inflicts "
                + to_string(impact) + " impact!",
            Log::eMessageTypes::Alert);
        target->disableWeapon();

        target->inflictImpact(impact);
        return false;
    }

    bool doBlunt = false;
    eDamageTypes damageType = attack.component->getType();

    eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(attack.target);
    // any thrust manevuer should trigger this
    if (attack.manuever == eOffensiveManuevers::Thrust) {
        bodyPart = WoundTable::getSingleton()->getThrust(attack.target);
    } else if (attack.manuever == eOffensiveManuevers::PinpointThrust) {
        bodyPart = attack.pinpointTarget;
    } else if (attack.manuever == eOffensiveManuevers::Swing) {
        // swings in these grips do less damage, unless its a linked component
        eGrips grip = attacker->getGrip();
        if ((grip == eGrips::HalfSword || grip == eGrips::Staff || grip == eGrips::Overhand)) {
            // overhand swings should be VERY bad
            if (grip == eGrips::Overhand) {
                MoS -= 1;
            }
            MoS -= 1;
        }
        MoS += attack.heavyblow;
    } else if (attack.manuever == eOffensiveManuevers::Mordhau) {
        damageType = eDamageTypes::Blunt;
        MoS++;
    }

    int finalDamage = MoS + attack.component->getDamage();

    const ArmorSegment armorAtLocation = target->getArmorAtPart(bodyPart);

    if (armorAtLocation.AV > 0) {
        writeMessage(
            target->getName() + "'s armor reduced wound level by " + to_string(armorAtLocation.AV));
    }

    // complicated armor calcs go here
    finalDamage -= armorAtLocation.AV;
    // add strength bonus minus constitution
    finalDamage += getTap(attacker->getStrength());

    finalDamage -= target->getConstitution();

    if (armorAtLocation.isMetal == true && damageType != eDamageTypes::Blunt && finalDamage > 0) {
        if (attack.component->hasProperty(eWeaponProperties::MaillePiercing) == false
            && armorAtLocation.type == eArmorTypes::Maille) {
            writeMessage("Maille armor reduces wound level by half");
            // piercing attacks round up, otherwise round down
            if (attack.component->getType() == eDamageTypes::Piercing) {
                finalDamage = (finalDamage + 1) / 2;
            } else {
                finalDamage = finalDamage / 2;
                // only slashing attacks get converted to blunt against maille
                doBlunt = true;
            }
            finalDamage = max(finalDamage, 1);

        } else if (attack.component->hasProperty(eWeaponProperties::PlatePiercing) == false
            && armorAtLocation.type == eArmorTypes::Plate) {
            writeMessage("Plate armor reduces wound level by half");
            // piercing attacks round up, otherwise round down
            if (attack.component->getType() == eDamageTypes::Piercing) {
                finalDamage = (finalDamage + 1) / 2;
            } else {
                finalDamage = finalDamage / 2;
            }
            finalDamage = max(finalDamage, 1);
            doBlunt = true;
        } else if (armorAtLocation.type != eArmorTypes::Plate
            && armorAtLocation.type != eArmorTypes::Maille) {
            // weird case where we have metal armor but not maille or plate (?)
            writeMessage("Metal armor reduces wound level by half");
            // piercing attacks round up, otherwise round down
            if (attack.component->getType() == eDamageTypes::Piercing) {
                finalDamage = (finalDamage + 1) / 2;
            } else {
                finalDamage = finalDamage / 2;
            }
            finalDamage = max(finalDamage, 1);
            doBlunt = true;
        }
    }
    constexpr int cCrushingImpact = 3;
    if (attack.component->hasProperty(eWeaponProperties::Crushing)
        && getHitLocation(bodyPart) == eHitLocations::Head) {
        writeMessage("Crushing weapon inflicts extra impact to head", Log::eMessageTypes::Alert);
        target->inflictImpact(cCrushingImpact);
    }

    if (finalDamage <= 0) {
        writeMessage(target->getName() + "'s armor prevented any damage!",
            Log::eMessageTypes::Announcement, true);
        return false;
    }

    eDamageTypes finalType = doBlunt == true ? eDamageTypes::Blunt : attack.component->getType();
    if (finalType == eDamageTypes::Blunt && armorAtLocation.isRigid == true
        && attack.component->hasProperty(eWeaponProperties::Crushing) == false) {
        constexpr int cMaxRigid = 3;
        finalDamage = min(cMaxRigid, finalDamage);
    }
    writeMessage(target->getName() + " received a level " + to_string(finalDamage) + " "
            + damageTypeToString(finalType) + " wound to " + bodyPartToString(bodyPart),
        Log::eMessageTypes::Standard, true);
    Wound* wound = WoundTable::getSingleton()->getWound(finalType, bodyPart, finalDamage);
    writeMessage(wound->getText(), Log::eMessageTypes::Damage);
    if (wound->getBTN() > target->getBTN()) {
        writeMessage(target->getName() + " begins to struggle from the pain",
            Log::eMessageTypes::Alert, true);
    }
    bool hasWeapon = target->getPrimaryWeaponId() != cFistsId;
    target->inflictWound(wound);
    if (target->getPrimaryWeaponId() == cFistsId && hasWeapon == true) {
        writeMessage(target->getName() + " has been disarmed!", Log::eMessageTypes::Alert);
    }

    if (target->getCreatureState() == eCreatureState::Dead) {
        // end combat
        writeMessage(
            target->getName() + " has been killed", Log::eMessageTypes::Announcement, true);
        m_currentState = eCombatState::FinishedCombat;
        return true;
    }
    if (target->getCreatureState() == eCreatureState::Unconscious) {
        // end combat
        writeMessage(target->getName() + " has been knocked unconcious",
            Log::eMessageTypes::Announcement, true);
        m_currentState = eCombatState::FinishedCombat;
        return true;
    }
    writeMessage("Wound impact causes " + target->getName() + " to lose "
            + to_string(wound->getImpact()) + " action points!",
        Log::eMessageTypes::Alert);
    return false;
}

void CombatInstance::forceRefresh()
{
    // reset combat pools
    m_currentTempo = eTempo::First;
    m_side1->resetCombatPool();
    m_side2->resetCombatPool();
}

void CombatInstance::switchTempo()
{
    if (m_currentTempo == eTempo::First) {
        writeMessage("Second tempo of exchange.", Log::eMessageTypes::Announcement);
        m_currentTempo = eTempo::Second;
    } else {
        writeMessage("Exchange has ended, combat pools have reset. First tempo of exchange.",
            Log::eMessageTypes::Announcement);
        forceRefresh();
    }
}

bool CombatInstance::isAttackerPlayer()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    return attacker->isPlayer();
}

bool CombatInstance::isDefenderPlayer()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    return defender->isPlayer();
}

void CombatInstance::run()
{
    // cout << "CombatState:" << (int)(m_currentState) << endl;
    switch (m_currentState) {
    case eCombatState::Uninitialized:
        return;
        break;
    case eCombatState::Initialized:
        doInitialization();
        break;
    case eCombatState::RollInitiative:
        doRollInitiative();
        break;
    case eCombatState::PreexchangeActions:
        doPreexchangeActions();
        break;
    case eCombatState::PositionActions:
        doPosition();
        break;
    case eCombatState::ResetState:
        doResetState();
        break;
    case eCombatState::Offense:
        doOffense();
        break;
    case eCombatState::StealInitiative:
        doStealInitiative();
        break;
    case eCombatState::StolenOffense:
        doStolenOffense();
        break;
    case eCombatState::AttackFromDefense:
        doAttackFromDefense();
        break;
    case eCombatState::DualOffense1:
        doDualOffense1();
        break;
    case eCombatState::DualOffenseStealInitiative:
        doDualOffenseStealInitiative();
        break;
    case eCombatState::DualOffense2:
        doDualOffense2();
        break;
    case eCombatState::DualOffenseSecondInitiative:
        doDualOffenseSecondInitiative();
        break;
    case eCombatState::Defense:
        doDefense();
        break;
    case eCombatState::PostDefense:
        break;
    case eCombatState::ParryLinked:
        doParryLinked();
        break;
    case eCombatState::PreResolution:
        doPreResolution();
        break;
    case eCombatState::Resolution:
        doResolution();
        break;
    case eCombatState::DualOffenseResolve:
        doDualOffenseResolve();
        break;
    case eCombatState::PostResolution:
        doPostResolution();
        break;
    case eCombatState::FinishedCombat:
        doEndCombat();
        break;
    case eCombatState::BetweenExchangeActions:
        break;
    }
}

void CombatInstance::writeMessage(const std::string& str, Log::eMessageTypes type, bool important)
{
    // combat manager is not a singleton, so we can have multiple.
    // we can choose not to display combatmanager messages if we want to.
    if (m_sendAllMessages == true) {
        Log::push(str, type);
    } else if (important == true) {
        Log::push(str, Log::eMessageTypes::Background);
    }
}

void CombatInstance::outputReachCost(int cost, Creature* attacker)
{
    // reach costs shouldn't be so extreme
    int reachCost = abs(cost);
    if (reachCost != 0) {
        writeMessage("Weapon length difference causes reach cost of " + to_string(reachCost)
                + " action points",
            Log::eMessageTypes::Announcement);
        attacker->reduceOffenseDie(reachCost);
        // attacker->reduceCombatPool(min(reachCost, attacker->getCombatPool()));
    }
}

bool CombatInstance::switchToStaffGrip(Creature* creature)
{
    Offense offense = creature->getQueuedOffense();
    return (
        creature->getGrip() == eGrips::Overhand && offense.manuever == eOffensiveManuevers::Swing);
}

const Weapon* CombatInstance::getAttackingWeapon(const Creature* creature)
{
    return creature->getQueuedOffense().withPrimaryWeapon == true ? creature->getPrimaryWeapon()
                                                                  : creature->getSecondaryWeapon();
}

const Weapon* CombatInstance::getDefendingWeapon(const Creature* creature)
{
    return creature->getQueuedDefense().withPrimaryWeapon == true ? creature->getPrimaryWeapon()
                                                                  : creature->getSecondaryWeapon();
}
