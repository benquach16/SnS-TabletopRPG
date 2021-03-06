
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

void CombatInstance::setSides(Creature*& attacker, Creature*& defender) const
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
    m_currentReach = max(side1->getCurrentReach(), side2->getCurrentReach());
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
    if (m_side1->pollForInitiative() == false) {
        m_currentState = eCombatState::RollInitiative;
        return;
    }
    if (m_side2->pollForInitiative() == false) {
        m_currentState = eCombatState::RollInitiative;
        return;
    }

    // get initiative rolls from both sides to determine roles.
    eInitiativeRoll side1 = m_side1->getInitiative();
    eInitiativeRoll side2 = m_side2->getInitiative();

    if (side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Defend) {
        // repeat
        writeMessage("Both sides chose to defend, deciding initiative again");
        m_side1->resetInitiative();
        m_side2->resetInitiative();
        if (m_dualWhiteTimes > 1) {
            writeMessage("Defense chosen too many times, initiative going to "
                         "willpower contest");
            int side1Successes
                = DiceRoller::rollGetSuccess(m_side1->getBTN(), m_side1->getWillpower());
            int side2Successes
                = DiceRoller::rollGetSuccess(m_side2->getBTN(), m_side2->getWillpower());

            m_initiative
                = side1Successes > side2Successes ? eInitiative::Side1 : eInitiative::Side2;

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
        // m_initiative = eInitiative::Side2;
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

    if (m_side1->getHasPrecombat() == false || m_side2->getHasPrecombat() == false) {
        m_currentState = eCombatState::PreexchangeActions;
        return;
    }
    auto side1Favoring = m_side1->getFavoredLocations();
    if (side1Favoring.size() > 0) {
        string str = m_side1->getName() + " is guarding ";
        for (auto location : side1Favoring) {
            str += hitLocationToString(location) + " ";
        }
        writeMessage(str);
    }
    auto side2Favoring = m_side2->getFavoredLocations();
    if (side2Favoring.size() > 0) {
        string str = m_side2->getName() + " is guarding ";
        for (auto location : side2Favoring) {
            str += hitLocationToString(location) + " ";
        }
        writeMessage(str);
    }
    // check if grip causes combatants to move closer
    eLength length = max(m_side1->getCurrentReach(), m_side2->getCurrentReach());
    if (length < m_currentReach) {
        m_currentReach = length;
    }
    m_currentState = eCombatState::PositionActions;
}

void CombatInstance::alignReach()
{
    // check if grip causes combatants to move closer
    eLength length = max(m_side1->getCurrentReach(), m_side2->getCurrentReach());
    m_side1->getSecondaryWeaponReach();
    m_side2->getSecondaryWeaponReach();
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

    attacker->addAndResetBonusDice();
    if (attacker->getHasOffense() == false) {
        m_currentState = eCombatState::Offense;
        return false;
    }
    applyAttackGuardBonuses(attacker);
    int reachCost = calcReachCost(attacker, true);
    outputReachCost(reachCost, attacker, true);
    outputOffense(attacker);
    m_originalLocation = attacker->getQueuedOffense().target;
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

    m_currentState = eCombatState::DualOffense1;
}

void CombatInstance::doDualOffenseStealInitiative()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    // causes issues with new implementation
    if (defender->getHasOffense() == false) {
        m_currentState = eCombatState::DualOffenseStealInitiative;
        return;
    }
    int reachCost = calcReachCost(defender, true);
    outputReachCost(reachCost, defender, true);

    Defense defense = defender->getQueuedDefense();

    writeMessage(defender->getName() + " allocates " + to_string(defense.dice) + " for initiative");
    outputOffense(defender);
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

    if (defender->getHasDefense() == false) {
        m_currentState = eCombatState::DualOffenseSecondInitiative;
        return;
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
    defender->addAndResetBonusDice();
    if (defender->getHasOffense() == false) {
        m_currentState = eCombatState::AttackFromDefense;
        return;
    }
    applyAttackGuardBonuses(defender);
    int reachCost = calcReachCost(defender, true);
    outputReachCost(reachCost, defender, true);
    outputOffense(defender);

    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doDefense()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    if (defender->getHasDefense() == false) {
        m_currentState = eCombatState::Defense;
        return;
    }
    int reachCost = calcReachCost(defender, false);
    outputReachCost(reachCost, defender, false);

    applyDefendGuardBonuses(defender);
    Defense defend = defender->getQueuedDefense();

    const Weapon* defendingWeapon = getDefendingWeapon(defender);
    if (defend.manuever == eDefensiveManuevers::StealInitiative) {
        // need both sides to attempt to allocate dice
        m_currentState = eCombatState::StealInitiative;
        return;
    }
    if (defend.manuever == eDefensiveManuevers::AttackFromDef) {
        m_currentState = eCombatState::AttackFromDefense;
        return;
    }

    if (defend.manuever == eDefensiveManuevers::ParryLinked) {
        writeMessage(defender->getName() + " performs a Masterstrike with "
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

    if (defender->getHasOffense() == false) {
        m_currentState = eCombatState::ParryLinked;
        return;
    }
    Offense offense = defender->getQueuedOffense();

    outputOffense(defender);
    m_currentState = eCombatState::PreResolution;
}

void CombatInstance::doStealInitiative()
{
    // defender inputs offense and dice to steal initiative
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);

    // then input manuever
    Defense defend = defender->getQueuedDefense();
    // do dice to steal initiative first
    // this polls for offense since the initiative steal is stored inside the
    // defense struct
    defender->addAndResetBonusDice();
    if (defender->getHasOffense() == false) {
        m_currentState = eCombatState::StealInitiative;
        return;
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

    if (attacker->getHasDefense() == false) {
        m_currentState = eCombatState::StolenOffense;
        return;
    }

    writeMessage(attacker->getName() + " allocates " + to_string(attacker->getQueuedDefense().dice)
        + " action points to contest initiative steal");
    int reachCost = calcReachCost(defender, true);
    reachCost = abs(reachCost);
    outputReachCost(reachCost, defender, true);
    outputOffense(defender);
    m_currentState = eCombatState::PreResolution;
}

void CombatInstance::doPreResolution()
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    if (attacker->getHasPreResolution() == false) {
        m_currentState = eCombatState::PreResolution;
        return;
    }
    Offense attack = attacker->getQueuedOffense();
    if (attack.feint) {
        eHitLocations newLocation = attack.target;
        if (m_originalLocation == eHitLocations::Head && newLocation == eHitLocations::Shin) {
            attacker->setOffenseFeintDice(attack.feintdie + 1);
        }
        if (m_originalLocation == eHitLocations::Shin && newLocation == eHitLocations::Head) {
            attacker->setOffenseFeintDice(attack.feintdie + 1);
        }
        if (m_originalLocation == eHitLocations::Chest && newLocation == eHitLocations::Shin) {
            attacker->setOffenseFeintDice(attack.feintdie + 1);
        }
        if (m_originalLocation == eHitLocations::Shin && newLocation == eHitLocations::Chest) {
            attacker->setOffenseFeintDice(attack.feintdie + 1);
        }
        attack = attacker->getQueuedOffense();
        writeMessage(attacker->getName() + " attempts to feint to a "
            + offensiveManueverToString(attack.manuever) + " at "
            + hitLocationToString(attack.target) + " with " + to_string(attack.feintdie)
            + " action points!");
    }
    m_currentState = eCombatState::Resolution;
}

void CombatInstance::doFeintAttack() { m_currentState = eCombatState::Resolution; }

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
        int side1Dice = (m_side1->getQueuedDefense().dice + 1) / 2;
        if (m_side1->getQueuedOffense().manuever == eOffensiveManuevers::Thrust
            || m_side1->getQueuedOffense().manuever == eOffensiveManuevers::PinpointThrust) {
            side1Dice += cThrustDie;
        }
        int side2Dice = (m_side2->getQueuedDefense().dice + 1) / 2;
        if (m_side2->getQueuedOffense().manuever == eOffensiveManuevers::Thrust
            || m_side2->getQueuedOffense().manuever == eOffensiveManuevers::PinpointThrust) {
            side2Dice += cThrustDie;
        }
        int side1reach = 0;
        int side2reach = 0;
        if (m_side1->getCurrentReach() == m_currentReach
            && m_side2->getCurrentReach() != m_currentReach) {
            side1reach++;
        } else if (m_side2->getCurrentReach() == m_currentReach
            && m_side1->getCurrentReach() != m_currentReach) {
            side2reach++;
        }

        int side1InitiativeSuccesses = DiceRoller::rollGetSuccess(
            side1BTN, side1Dice + getTap(m_side1->getMobility()) + side1reach);
        int side2InitiativeSuccesses = DiceRoller::rollGetSuccess(
            side2BTN, side2Dice + getTap(m_side2->getMobility()) + side2reach);

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
        int tn = getAttackTN(attacker);
        int attackerSuccesses = DiceRoller::rollGetSuccess(tn, attacker->getQueuedOffense().dice);

        bool wasStanding = defender->getStance() == eCreatureStance::Standing;
        bool wasGrappled = false;
        if (attackerSuccesses > 0) {
            changeReachTo(attacker);
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
                attacker->setBonusDice(attackerSuccesses + 2);
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
        } else if (wasGrappled == true) {
            writeMessage(defender->getName() + " was grappled, their attack is interrupted!");
        } else if (defender->getQueuedOffense().dice <= 0) {
            // if the attack wiped out their combat pool, do nothing
            writeMessage(defender->getName()
                + " had their action points eliminated by impact, their attack "
                  "can no longer resolve.");
            m_currentState = eCombatState::Offense;
        } else if ((defender->primaryWeaponDisabled()
                       && defender->getQueuedOffense().withPrimaryWeapon)
            || defender->droppedWeapon()) {
            // need to check if secondary weapon part vs secondary weapon for dropped

            // if the attack disabled or caused their weapon to drop
            // sans secondary attacks
            writeMessage(defender->getName()
                + " had their weapon disabled! Their attack can no longer resolve.");
            m_currentState = eCombatState::Offense;

        } else {
            int tn = getAttackTN(defender);
            int defendSuccesses = DiceRoller::rollGetSuccess(tn, defender->getQueuedOffense().dice);
            if (defendSuccesses > 0) {
                if (defender->getQueuedOffense().manuever != eOffensiveManuevers::Grab) {
                    if (inflictWound(
                            defender, defendSuccesses, defender->getQueuedOffense(), attacker)) {
                        m_currentState = eCombatState::FinishedCombat;
                        return;
                    }
                } else {
                    startGrapple(attacker, defender);
                    defender->setBonusDice(defendSuccesses + 2);
                }

            } else {
                writeMessage(defender->getName() + " had no successes");
            }

            if (defend.manuever == eDefensiveManuevers::StealInitiative) {
                if (defendSuccesses > attackerSuccesses) {
                    changeReachTo(defender);
                    writeMessage(defender->getName() + " had more successes, taking initiative");
                    switchInitiative();
                } else {
                    changeReachTo(attacker);
                }
            }
            if (defend.manuever == eDefensiveManuevers::AttackFromDef && defendSuccesses > 0) {
                changeReachTo(defender);
                writeMessage(
                    defender->getName() + "'s attack landed after, but does not take initiative");
            }
        }
    } else {
        // standard attacker-defender resolution
        // if stomp discard everything
        // roll dice
        if (attack.feint == true) {
            // resolve feint to change successes
            int bonusDie = attack.feintdie / 2;
            cout << "feint bonus die: " << bonusDie << endl;
            int attackerKeen = DiceRoller::rollGetSuccess(
                attacker->getBTN(), attacker->getShrewdness() + bonusDie);
            int defenderKeen
                = DiceRoller::rollGetSuccess(defender->getBTN(), defender->getShrewdness());

            int keenDifference = attackerKeen - defenderKeen;
            if (keenDifference > 0) {
                keenDifference++;
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
        int offenseSuccesses = DiceRoller::rollGetSuccess(getAttackTN(attacker), attack.dice);
        int defenseSuccesses = DiceRoller::rollGetSuccess(getDefendTN(defender), defend.dice);

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
                changeReachTo(attacker);

            } else {
                startGrapple(attacker, defender);
                attacker->setBonusDice(MoS + 2);
            }
        } else if (MoS == 0) {
            writeMessage("no net successes, " + attacker->getName() + " retains initiative");
        } else {
            if (defend.manuever == eDefensiveManuevers::Dodge) {
                writeMessage("attack dodged with " + to_string(-MoS) + " successes");
            } else {
                writeMessage("attack parried with " + to_string(-MoS) + " successes");

                // natural weapon parries still take damage thru the parry
                if (getDefendingWeapon(defender)->getNaturalWeapon()) {
                    attack.target = eHitLocations::Arm;
                    inflictWound(attacker, 0, attack, defender);
                }
            }
            if (defend.manuever == eDefensiveManuevers::Expulsion) {
                if (attack.withPrimaryWeapon) {
                    attacker->disableWeapon();
                } else {
                    attacker->disableSecondaryWeapon();
                }

                // workaround here, impact deals straight to attack die first but attack has
                // resolved already, while clearCreatureManuevers has not been called yet
                attacker->inflictImpact(-MoS + 1 + attack.dice);
                Log::push(
                    "Attacker's weapon disabled and " + to_string(-MoS + 1) + " impact inflicted!");
            }
            if (defend.manuever == eDefensiveManuevers::ParryLinked) {
                // resolve offense
                Offense offense = defender->getQueuedOffense();

                // disadvantage on the attack unless done with secondary weapon

                bool linked = false;
                if (offense.withPrimaryWeapon != defend.withPrimaryWeapon) {
                    if (defend.withPrimaryWeapon == true) {
                        linked = true;
                    } else if (defender->getSecondaryWeapon()->isShield()) {
                        linked = true;
                    }
                }
                const Component* component = offense.component;
                int tn = getAttackTN(defender);
                int BTN = linked == true ? tn : getDisadvantagedTN(tn);

                int linkedOffenseMoS = DiceRoller::rollGetSuccess(BTN, offense.dice);
                cout << "Linked hits: " << linkedOffenseMoS << endl;
                // reset attacker offense to properly deal impact
                attacker->clearCreatureManuevers();
                if (linkedOffenseMoS > 0) {
                    changeReachTo(defender);
                    if (inflictWound(defender, linkedOffenseMoS, offense, attacker) == true) {
                        m_currentState = eCombatState::FinishedCombat;
                        return;
                    }
                }
            }
            if (defend.manuever == eDefensiveManuevers::Counter
                || defend.manuever == eDefensiveManuevers::Reverse) {
                offenseSuccesses += 2;
                cout << "bonus: " << offenseSuccesses << endl;
                defender->setBonusDice(offenseSuccesses);
                writeMessage(defender->getName() + " receives " + to_string(offenseSuccesses)
                    + " action points in their next attack");
            }
            // dodge needs 2 MoS to take init
            if (defend.manuever != eDefensiveManuevers::Dodge || -MoS > 1) {
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

    int tn1 = getAttackTN(m_side1);
    int tn2 = getAttackTN(m_side2);
    int MoS = DiceRoller::rollGetSuccess(tn1, attack.dice);
    int MoS2 = DiceRoller::rollGetSuccess(tn2, attack2.dice);

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
        changeReachTo(m_side1);
        m_initiative = eInitiative::Side1;
    } else if (MoS < MoS2) {
        changeReachTo(m_side2);
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
        m_currentState = eCombatState::BetweenExchangeActions;
    }
    // m_currentState = eCombatState::PreexchangeActions;
    switchTempo();
}

void CombatInstance::doBetweenExchange()
{
    // bug - if both sides have combat pools totally wiped then this will go on infinitely
    if (m_side1->getCombatPool() <= 0 && m_side2->getCombatPool() <= 0) {
        writeMessage("Neither side has any action points left, starting "
                     "new exchange and resetting combat pools. It is now first tempo");
        m_currentTempo = eTempo::First;
        m_side1->resetCombatPool();
        m_side2->resetCombatPool();
        m_side1->clearCreatureManuevers(true);
        m_side2->clearCreatureManuevers(true);
        m_currentState = eCombatState::PreexchangeActions;
        return;
    }
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    // bug - this condition will not get hit since the ai might update faster than this function
    // gets called
    if (attacker->getCombatPool() <= 0 && defender->getCombatPool() > 0) {
        writeMessage(
            attacker->getName() + " has no more action points! Initiative swaps to defender");
        switchInitiative();
        setSides(attacker, defender);
    }
    if (m_side1->getHasPrecombat() == false || m_side2->getHasPrecombat() == false) {
        m_currentState = eCombatState::BetweenExchangeActions;
        return;
    }
    // check if grip causes combatants to move closer
    eLength length = max(m_side1->getCurrentReach(), m_side2->getCurrentReach());
    if (length < m_currentReach) {
        m_currentReach = length;
    }
    m_currentState = eCombatState::PositionActions;
}

void CombatInstance::resolvePosition(Creature* creature)
{
    Position position = creature->getQueuedPosition();
    if (creature->canStand() == false) {
        writeMessage(creature->getName()
                + " tried to stand but they are missing a leg and fall over instead!",
            Log::Damage);
        return;
    }
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
    m_side1->enableWeapon();
    m_side2->enableWeapon();
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
    if (attacker->getPrimaryWeapon()->getLength() > eLength::Hand) {
        attacker->dropWeapon();
    }
    if (attacker->getSecondaryWeapon()->getLength() > eLength::Hand) {
        attacker->dropSecondaryWeapon();
    }
    if (defender->getPrimaryWeapon()->getLength() > eLength::Hand) {
        defender->dropWeapon();
    }
    if (defender->getSecondaryWeapon()->getLength() > eLength::Hand) {
        defender->dropSecondaryWeapon();
    }
}

bool CombatInstance::inflictWound(Creature* attacker, int MoS, Offense attack, Creature* target)
{
    if (attack.manuever == eOffensiveManuevers::NoOffense) {
        return false;
    }
    writeMessage(
        attacker->getName() + "'s attack landed with " + to_string(MoS) + " net successes!",
        Log::eMessageTypes::Announcement);
    if (attack.manuever == eOffensiveManuevers::Hook) {
        writeMessage(target->getName() + " loses " + to_string(MoS) + " action points from impact",
            Log::eMessageTypes::Alert);
        target->inflictImpact(MoS);
        switch (attack.hookTarget) {
        case eHookTargets::Primary:
            writeMessage(target->getPrimaryWeapon()->getName() + " has been hooked and disabled!",
                Log::eMessageTypes::Alert, true);
            target->disableWeapon();
            break;
        case eHookTargets::Secondary:
            writeMessage(target->getSecondaryWeapon()->getName() + " has been hooked and disabled!",
                Log::eMessageTypes::Alert, true);
            target->disableSecondaryWeapon();
            break;
        default:
            if (MoS >= 3) {
                writeMessage(target->getName() + " has been hooked and thrown prone!",
                    Log::eMessageTypes::Alert, true);
                target->setProne();
            }
            break;
        }

        return false;
    }
    if (attack.manuever == eOffensiveManuevers::Throw) {
        writeMessage(
            target->getName() + " loses " + to_string(MoS * 2) + " action points from impact",
            Log::eMessageTypes::Alert);
        target->inflictImpact(MoS * 2);
        writeMessage(
            target->getName() + " has been thrown to the ground!", Log::eMessageTypes::Alert, true);
        target->setProne();

        return false;
    }
    if (attack.manuever == eOffensiveManuevers::Disarm) {
        if (MoS >= 3) {
            writeMessage(target->getName() + " has been disarmed!", Log::eMessageTypes::Alert);
            target->dropWeapon();
        } else {
            writeMessage("Disarm was not effective", Log::eMessageTypes::Alert);
        }

        return false;
    }

    if (attack.manuever == eOffensiveManuevers::Beat) {
        const int impact = MoS + 1;
        switch (attack.hookTarget) {
        case eHookTargets::Secondary:
            writeMessage(target->getSecondaryWeapon()->getName()
                    + " has been beaten aside and inflicts " + to_string(impact) + " impact!",
                Log::eMessageTypes::Alert, true);
            target->disableSecondaryWeapon();
            break;
        default:
            writeMessage(target->getPrimaryWeapon()->getName()
                    + " has been beaten aside and inflicts " + to_string(impact) + " impact!",
                Log::eMessageTypes::Alert, true);
            target->disableWeapon();
            break;
        }

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
    } else if (attack.manuever == eOffensiveManuevers::VisorThrust) {
        bodyPart = eBodyParts::Face;
    } else if (attack.manuever == eOffensiveManuevers::Snap) {
        damageType = eDamageTypes::Blunt;
        bodyPart = eBodyParts::Elbow;
    } else if (attack.manuever == eOffensiveManuevers::Swing
        || attack.manuever == eOffensiveManuevers::HeavyBlow) {
        // swings in these grips do less damage, unless its a linked component
        eGrips grip = attacker->getGrip();
        if ((grip == eGrips::HalfSword || grip == eGrips::Staff || grip == eGrips::Overhand)) {
            // overhand swings should be VERY bad
            if (grip == eGrips::Overhand) {
                MoS -= 1;
            }
            MoS -= 1;
        }
        if (attack.manuever == eOffensiveManuevers::HeavyBlow) {
            MoS += 2;
        }
        MoS += attack.heavyblow;
    } else if (attack.manuever == eOffensiveManuevers::Mordhau) {
        damageType = eDamageTypes::Blunt;
    }

    int finalDamage = MoS + attack.component->getDamage();
    // add strength bonus minus constitution
    finalDamage += getTap(attacker->getStrength());

    finalDamage -= target->getConstitution();

    const ArmorSegment armorAtLocation = target->getArmorAtPart(bodyPart);
    // if visor thrust calc here, before armor reduction but after damage
    bool ignoreArmor = false;
    if (attack.manuever == eOffensiveManuevers::VisorThrust
        || attack.manuever == eOffensiveManuevers::Snap) {
        ignoreArmor = true;
    }
    writeMessage(
        "Attack strikes the " + bodyPartToString(bodyPart) + "!", Log::eMessageTypes::Announcement);
    int AV = armorAtLocation.AV;
    if (armorAtLocation.isMetal == true && damageType != eDamageTypes::Blunt && finalDamage > 0) {
        if ((attack.component->hasProperty(eWeaponProperties::MaillePiercing) == false
                && attack.component->hasProperty(eWeaponProperties::PlatePiercing) == false)
            && armorAtLocation.type == eArmorTypes::Maille) {
            writeMessage("Maille armor heavily increases armor rating against this damage type.");
            // piercing attacks round up, otherwise round down
            if (attack.component->getType() == eDamageTypes::Piercing) {
                AV = AV * 2 - 1;
            } else {
                AV = AV * 2;
                // only slashing attacks get converted to blunt against maille
                doBlunt = true;
            }
            finalDamage = max(finalDamage, 1);

        } else if (attack.component->hasProperty(eWeaponProperties::PlatePiercing) == false
            && armorAtLocation.type == eArmorTypes::Plate) {
            writeMessage("Plate armor heavily increases armor rating against this damage type.");
            AV = AV * 2;
            doBlunt = true;
            finalDamage = max(finalDamage, 1);
            doBlunt = true;
        } else if (armorAtLocation.type != eArmorTypes::Plate
            && armorAtLocation.type != eArmorTypes::Maille) {
            // weird case where we have metal armor but not maille or plate (?)
            writeMessage("Metal armor heavily increases armor rating against this damage type.");
            // piercing attacks round up, otherwise round down
            if (attack.component->getType() == eDamageTypes::Piercing) {
                AV = AV * 2 - 1;
            } else {
                AV = AV * 2;
                // only slashing attacks get converted to blunt against maille
                doBlunt = true;
            }
            finalDamage = max(finalDamage, 1);
            doBlunt = true;
        }
    }

    if (ignoreArmor == false) {
        finalDamage -= AV;

        if (AV > 0) {
            writeMessage(target->getName() + "'s armor reduced wound level by " + to_string(AV));
        }
    }
    constexpr int cCrushingImpact = 2;
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
    bool hasWeapon = target->getPrimaryWeaponId() != target->getNaturalWeaponId();
    target->inflictWound(wound);
    if (target->getPrimaryWeaponId() == target->getNaturalWeaponId() && hasWeapon == true) {
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

Creature* CombatInstance::getAttacker() const
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    return attacker;
}

Creature* CombatInstance::getDefender() const
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    return defender;
}

bool CombatInstance::isAttackerPlayer() const
{
    Creature* attacker = nullptr;
    Creature* defender = nullptr;
    setSides(attacker, defender);
    return attacker->isPlayer();
}

bool CombatInstance::isDefenderPlayer() const
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
        doBetweenExchange();
        break;
    case eCombatState::FeintAttack:
        break;
    }
}

int CombatInstance::calcReachCost(Creature* creature, bool attacker)
{
    if (attacker) {
        assert(creature->getHasOffense());
        Offense offense = creature->getQueuedOffense();
        eLength length = offense.withPrimaryWeapon ? creature->getCurrentReach()
                                                   : creature->getSecondaryWeaponReach();

        int cost = calculateReachCost(getCurrentReach(), length);
        if (offense.manuever == eOffensiveManuevers::Beat) {
            return max(cost, 1);
        }
        return cost;
    } else {
        assert(creature->getHasDefense());
        Defense defense = creature->getQueuedDefense();
        if (defense.manuever == eDefensiveManuevers::Dodge) {
            return 0;
        }
        eLength length = defense.withPrimaryWeapon ? creature->getCurrentReach()
                                                   : creature->getSecondaryWeaponReach();
        // pay reach if weapon is too long
        if (getCurrentReach() < length) {
            return calculateReachCost(getCurrentReach(), length);
        } else {
            return 0;
        }
    }
}

void CombatInstance::outputOffense(const Creature* creature)
{
    Offense attack = creature->getQueuedOffense();
    const Weapon* offenseWeapon = getAttackingWeapon(creature);
    switch (attack.manuever) {
    case eOffensiveManuevers::NoOffense:
        writeMessage(creature->getName() + " does nothing!");
        break;
    case eOffensiveManuevers::Beat:
        writeMessage(creature->getName() + " attempts to Beat at "
            + hookTargetToString(attack.hookTarget) + " with " + offenseWeapon->getName()
            + " using " + to_string(creature->getQueuedOffense().dice) + " action points");
        break;
    case eOffensiveManuevers::Hook:
        writeMessage(creature->getName() + " attempts to Hook at "
            + hookTargetToString(attack.hookTarget) + " with " + offenseWeapon->getName()
            + " using " + to_string(creature->getQueuedOffense().dice) + " action points");
        break;
    case eOffensiveManuevers::Grab:
        writeMessage(creature->getName() + " attempts to start a grapple using "
            + to_string(creature->getQueuedOffense().dice) + " action points");
        break;
    case eOffensiveManuevers::Disarm:
        writeMessage(creature->getName() + " attempts to disarm using "
            + to_string(creature->getQueuedOffense().dice) + " action points");
        break;
    case eOffensiveManuevers::Throw:
        writeMessage(creature->getName() + " attempts a throw using "
            + to_string(creature->getQueuedOffense().dice) + " action points");
        break;
    default:
        assert(attack.component != nullptr);
        assert(offenseWeapon);
        writeMessage(creature->getName() + " " + offensiveManueverToString(attack.manuever)
            + "s with " + offenseWeapon->getName() + " at " + hitLocationToString(attack.target)
            + " using " + attack.component->getName() + " with "
            + to_string(creature->getQueuedOffense().dice) + " action points");
        break;
    }
}

void CombatInstance::outputDefense(const Creature* creature) {}

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

void CombatInstance::outputReachCost(int cost, Creature* creature, bool attacker)
{
    // reach costs shouldn't be so extreme
    int reachCost = abs(cost);

    if (reachCost != 0) {
        if (attacker) {
            if (creature->getQueuedOffense().manuever != eOffensiveManuevers::Beat) {
                writeMessage("Weapon length difference causes reach cost of " + to_string(reachCost)
                        + " action points",
                    Log::eMessageTypes::Announcement);
                // creature->reduceOffenseDie(reachCost);
                // attacker->reduceCombatPool(min(reachCost, attacker->getCombatPool()));
            } else {
                writeMessage("Beat does not change reach", Log::eMessageTypes::Announcement);
            }

        } else {
            writeMessage("Weapon is too long for defense, causes reach cost of "
                    + to_string(reachCost) + " action points",
                Log::eMessageTypes::Announcement);
            // creature->reduceDefenseDie(reachCost);
        }
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

int CombatInstance::getAttackTN(const Creature* creature)
{
    assert(creature->getHasOffense());
    int tn = getAttackingWeapon(creature)->getBaseTN();
    if (creature->getQueuedOffense().component != nullptr) {
        tn = creature->getQueuedOffense().component->getTN();
    }
    return tn;
}

int CombatInstance::getDefendTN(const Creature* creature)
{
    assert(creature->getHasDefense());
    const Weapon* defendWeapon = getDefendingWeapon(creature);
    if (creature->getQueuedDefense().manuever == eDefensiveManuevers::Dodge) {
        constexpr int cDodgeTn = 7;
        return cDodgeTn;
    }
    return defendWeapon->getGuardTN();
}

eLength CombatInstance::getEffectiveReach(const Creature* creature)
{
    return creature->getQueuedOffense().withPrimaryWeapon == true
        ? creature->getCurrentReach()
        : creature->getSecondaryWeaponReach();
}

void CombatInstance::changeReachTo(const Creature* creature)
{
    Offense attack = creature->getQueuedOffense();
    eLength effectiveReach = getEffectiveReach(creature);
    if (m_inGrapple == false) {
        switch (attack.manuever) {
        case eOffensiveManuevers::NoOffense:
        case eOffensiveManuevers::Beat:
            return;
        case eOffensiveManuevers::Grab:
            m_currentReach = eLength::Hand;
        default:
            m_currentReach = effectiveReach;
        }
    } else {
        m_currentReach = eLength::Hand;
    }
}

void CombatInstance::applyAttackGuardBonuses(Creature* attacker)
{
    assert(attacker->getHasOffense());

    Offense attack = attacker->getQueuedOffense();
    switch (attacker->getCurrentGuard()) {
    case eCombatGuard::HighGuard: {
        switch (attack.manuever) {
        case eOffensiveManuevers::Beat: {
            writeMessage(
                "Beat gets an attack bonus from being in a high guard.", Log::eMessageTypes::Alert);
            attacker->setOffenseDice(attack.dice + 2);
        } break;
        case eOffensiveManuevers::Swing: {
            if (attack.target == eHitLocations::Head) {
                writeMessage("Swing gets an attack bonus to head targets when in a high guard.",
                    Log::eMessageTypes::Alert);
                attacker->setOffenseDice(attack.dice + 2);
            }
        } break;
        }
    } break;
    case eCombatGuard::MiddleGuard: {
        switch (attack.manuever) {
        case eOffensiveManuevers::Thrust:
        case eOffensiveManuevers::PinpointThrust:
            if (attack.target == eHitLocations::Chest) {
                writeMessage("Thrust gets an attack bonus to chest targets when in a middle guard.",
                    Log::eMessageTypes::Alert);
                attacker->setOffenseDice(attack.dice + 2);
            }
        }
        break;
    } break;
    case eCombatGuard::LowGuard: {
        switch (attack.manuever) {
        case eOffensiveManuevers::Thrust:
        case eOffensiveManuevers::Swing:
            if (attack.target == eHitLocations::Arm) {
                writeMessage(offensiveManueverToString(attack.manuever)
                        + " gets an attack bonus to the arm and belly targets in a low guard.",
                    Log::eMessageTypes::Alert);
                attacker->setOffenseDice(attack.dice + 2);
            }
        }
        break;
    } break;
    }
}

void CombatInstance::applyDefendGuardBonuses(Creature* defender)
{
    assert(defender->getHasDefense());

    Defense defend = defender->getQueuedDefense();
    switch (defender->getCurrentGuard()) {
    case eCombatGuard::LowGuard: {
        switch (defend.manuever) {
        case eDefensiveManuevers::Parry:
        case eDefensiveManuevers::Expulsion:
            writeMessage(defensiveManueverToString(defend.manuever)
                    + " gets a defensive bonus from being in a low guard",
                Log::eMessageTypes::Alert);
            defender->setDefenseDice(defend.dice + 2);
            break;
        case eDefensiveManuevers::StealInitiative:
            writeMessage(defensiveManueverToString(defend.manuever)
                    + " gets a penalty for being in a low guard",
                Log::eMessageTypes::Alert);
            defender->setDefenseDice(defend.dice - 2);
            break;
        }

    } break;
    }
}
