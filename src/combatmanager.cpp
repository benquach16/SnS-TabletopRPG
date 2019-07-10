#include <iostream>
#include "creatures/utils.h"
#include "items/utils.h"
#include "combatmanager.h"
#include "game.h"
#include "creatures/player.h"

#include <assert.h>

using namespace std;

CombatManager::CombatManager(): m_initiative(eInitiative::Side1), m_side1(nullptr), m_side2(nullptr),
								m_currentTempo(eTempo::First), m_currentState(eCombatState::Uninitialized)
{
}

void CombatManager::setSides(Creature*& attacker, Creature*& defender)
{
	if(m_initiative == eInitiative::Side1) {
		attacker = m_side1;
		defender = m_side2;
	}
	else if(m_initiative == eInitiative::Side2) {
		attacker = m_side2;
		defender = m_side1;
	}
	assert(attacker != nullptr);
	assert(defender != nullptr);
}

void CombatManager::initCombat(Creature* side1, Creature* side2)
{
	assert(side1 != nullptr);
	assert(side2 != nullptr);
	assert(m_currentState == eCombatState::Uninitialized);
	
	m_side1 = side1;
	m_side1->resetCombatPool();
	m_side2 = side2;
	m_side2->resetCombatPool();

	m_currentTempo = eTempo::First;
	m_initiative = eInitiative::Side1;
	m_currentState = eCombatState::Initialized;
}

void CombatManager::doInitialization()
{
	assert(m_side1 != nullptr);
	assert(m_side2 != nullptr);
	assert(m_currentState == eCombatState::Initialized);
	
	writeMessage("Combat between " + m_side1->getName() + " and " + m_side2->getName(), Log::eMessageTypes::Announcement);
	writeMessage(m_side1->getName() + " is using " + m_side1->getPrimaryWeapon()->getName() + " and " +
			  m_side2->getName() + " is using " + m_side2->getPrimaryWeapon()->getName());

	m_currentState = eCombatState::RollInitiative;
}

void CombatManager::doRollInitiative()
{
	if(m_side1->isPlayer() == true) {
		Player* player = static_cast<Player*>(m_side1);
		if(player->pollForInitiative() == false) {
			m_currentState = eCombatState::RollInitiative;
			return;			
		}
		
	}
	
	//get initiative rolls from both sides to determine roles.
	eInitiativeRoll side1 = m_side1->doInitiative();
	eInitiativeRoll side2 = m_side2->doInitiative();
	if(m_side1->isPlayer() == true) {
		Player* player = static_cast<Player*>(m_side1);
		side1 = player->getInitiative();
	}
	
	if(side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Defend) {
		//repeat
		writeMessage("Both sides chose to defend, deciding initiative again");
		m_currentState = eCombatState::ResetState;
		return;
	} else if(side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Defend) {
		writeMessage(m_side1->getName() + " chose to attack and " + m_side2->getName() + " is defending");
		m_initiative = eInitiative::Side1;
		m_currentState = eCombatState::Offense;
		return;
	} else if(side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Attack) {
		writeMessage(m_side2->getName() + " chose to attack and " + m_side1->getName() + " is defending");
		m_initiative = eInitiative::Side2;
		m_currentState = eCombatState::Offense;		
		return;
	} else if(side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Attack) {
		writeMessage("Both sides chose to attack, no defense can be done by either side.");
		// no defense here!
		// hopefully you don't die horribly
		// roll speed to determine who goes first
		int side1Successes = DiceRoller::rollGetSuccess(m_side1->getBTN(), m_side1->getSpeed());
		int side2Successes = DiceRoller::rollGetSuccess(m_side2->getBTN(), m_side2->getSpeed());

		m_initiative = side1Successes < side2Successes ? eInitiative::Side1 : eInitiative::Side2;
		if(m_initiative == eInitiative::Side1) {
			writeMessage(m_side1->getName() + " declares their attack first");
		} else {
			writeMessage(m_side2->getName() + " declares their attack first");
		}
		m_currentState = eCombatState::DualOffense1;
		return;
	}

	m_currentState = eCombatState::RollInitiative;
}

void CombatManager::doResetState()
{
	m_currentState = eCombatState::RollInitiative;
}

bool CombatManager::doOffense()
{
	//get offensive manuever and dice from side 1
	//then get defensive manuever and dice from side 2
	//then resolve		
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	
	if(attacker->getCombatPool() <= 0 && defender->getCombatPool() > 0) {
		writeMessage(attacker->getName() + " has no more action points! Initiative swaps to defender");
		switchInitiative();
		setSides(attacker, defender);
	}
	
	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	
	int offenseCombatPool = attacker->getCombatPool();

	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(offenseWeapon->getLength());
	reachCost = std::max(0, reachCost);
	if(attacker->isPlayer() == true)
	{
		//wait until we get input from player
		Player* player = static_cast<Player*>(attacker);
		if(player->pollForOffense() == false) {
			m_currentState = eCombatState::Offense;
			return false;
		} 
	}
	else {
		attacker->doOffense(defender, reachCost);		
	}

	Creature::Offense attack = attacker->getQueuedOffense();
	
	assert(attack.component != nullptr);
	assert(attack.dice <= offenseCombatPool);
	cout << attack.dice << endl;
	assert(attack.dice >= 0);
	attacker->reduceCombatPool(attack.dice);
	
	writeMessage(attacker->getName() + " " + offensiveManueverToString(attack.manuever) + "s with " +
				 offenseWeapon->getName() + " using " +
				 attack.component->getName() + " with " +
				 to_string(attack.dice) + " action points");
	
	m_currentState = eCombatState::Defense;
	return true;
}

void CombatManager::doDualOffense1()
{
	//both sides rolled red
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	//person who rolled better on speed goes second

	if(doOffense() == false) {
		m_currentState = eCombatState::DualOffense1;
		return;
	}
	switchInitiative();
	m_currentState = eCombatState::DualOffense2;
}

void CombatManager::doDualOffense2()
{
	//both sides rolled red
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	if(doOffense() == false) {
		m_currentState = eCombatState::DualOffense2;
		return;
	}
	m_currentState = eCombatState::DualOffenseResolve;
}

void CombatManager::doDefense()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	Weapon* defenseWeapon = defender->getPrimaryWeapon();

	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();
	if(defender->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(defender);
		if(player->pollForDefense() == false) {
			m_currentState = eCombatState::Defense;
			return;
		}

	}
	else {
		defender->doDefense(attacker, m_currentTempo == eTempo::Second);		
	}
	
	Creature::Defense defend = defender->getQueuedDefense();
	assert(defend.dice <= defenseCombatPool);
	assert(defend.dice >= 0);
	defender->reduceCombatPool(defend.dice);
	if(defend.manuever == eDefensiveManuevers::StealInitiative) {
		//need both sides to attempt to allocate dice
		m_currentState = eCombatState::StealInitiative;
		return;
	}

	
	writeMessage(defender->getName() + " defends with " + defenseWeapon->getName() +
				 " using " + to_string(defend.dice) + " action points");
	
	m_currentState = eCombatState::Resolution;
}

void CombatManager::doStealInitiative()
{
	//defender inputs offense and dice to steal initiative
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	//then input manuever
	Creature::Defense defend = defender->getQueuedDefense();
	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	int reachCost = static_cast<int>(offenseWeapon->getLength()) - static_cast<int>(defenseWeapon->getLength());
	reachCost = std::max(0, reachCost);
	//do dice to steal initiative first
	if(defender->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(defender);
		if(player->pollForOffense() == false) {
			m_currentState = eCombatState::StealInitiative;
			return;
		}

	}
	else {
		defender->doOffense(attacker, reachCost, true);
	}

	writeMessage(defender->getName() + " attempts to steal intiative using " + to_string(defend.dice) +
		" action points!");

	m_currentState = eCombatState::StolenOffense;
}

void CombatManager::doStolenOffense()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	if(attacker->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(attacker);
		if(player->pollForDefense() == false) {
			m_currentState = eCombatState::StolenOffense;
			return;
		}
	}
	else {
 		attacker->doStolenInitiative(defender);
	}

	assert(attacker->getQueuedDefense().dice <= attacker->getCombatPool());
	attacker->reduceCombatPool(attacker->getQueuedDefense().dice);
	
	writeMessage(attacker->getName() + " allocates " + to_string(attacker->getQueuedDefense().dice) +
				 " action points to contest initiative steal");

	writeMessage(defender->getName() + " " + offensiveManueverToString(defender->getQueuedOffense().manuever) + "s with " +
				 defender->getPrimaryWeapon()->getName() + " using " +
				 defender->getQueuedOffense().component->getName() + " with " +
				 to_string(defender->getQueuedOffense().dice) + " action points");	
	m_currentState = eCombatState::Resolution;
}

void CombatManager::doResolution()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	Creature::Offense attack = attacker->getQueuedOffense();
	Creature::Defense defend = defender->getQueuedDefense();

	//determine who was originally attacking
	if(defend.manuever == eDefensiveManuevers::StealInitiative) {

		//original attacker gets advantage
		int side1BTN = (m_side1 == attacker) ? m_side1->getBTN() - 1 : m_side1->getBTN();
		int side2BTN = (m_side2 == attacker) ? m_side2->getBTN() - 1 : m_side2->getBTN();
		//special case for thrust manuever, get an extra die
		int side1Dice = (m_side1->getQueuedOffense().manuever == eOffensiveManuevers::Thrust) ?
			m_side1->getQueuedDefense().dice + 1 : m_side1->getQueuedDefense().dice;
		int side2Dice = (m_side2->getQueuedOffense().manuever == eOffensiveManuevers::Thrust) ?
			m_side2->getQueuedDefense().dice + 1 : m_side2->getQueuedDefense().dice;

		int side1InitiativeSuccesses =
			DiceRoller::rollGetSuccess(side1BTN, side1Dice + m_side1->getSpeed());
		int side2InitiativeSuccesses =
			DiceRoller::rollGetSuccess(side2BTN, side2Dice + m_side2->getSpeed());
		
		Creature* originalAttackerPtr = attacker;
		if(side1InitiativeSuccesses > side2InitiativeSuccesses) {
			m_initiative = eInitiative::Side1;
		} else {
			m_initiative = eInitiative::Side2;
		}
		setSides(attacker, defender);
		writeMessage(attacker->getName() + " has taken the initiative!");

		int attackerSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(),
													   attacker->getQueuedOffense().dice);
		if(attackerSuccesses > 0) {
			if(inflictWound(attackerSuccesses, attacker->getQueuedOffense(), defender, true) == true) {
				m_currentState = eCombatState::FinishedCombat;
				return;
			}
		} else {
			writeMessage(attacker->getName() + " had no successes");			
		}

		if(defender->getQueuedOffense().dice == 0) {
			//if the attack wiped out their combat pool, do nothing
			writeMessage(defender->getName() + " had their action points eliminated by impact, their attack is dropped.");
			m_currentState = eCombatState::Offense;
			return;
		}
		
		int defendSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(),
													defender->getQueuedOffense().dice);
		if(defendSuccesses > 0) {
			if(inflictWound(defendSuccesses, defender->getQueuedOffense(), attacker) == true) {
				m_currentState = eCombatState::FinishedCombat;
				return;
			}
		} else {
			writeMessage(defender->getName() + " had no successes");
		}

		if(defendSuccesses > attackerSuccesses) {
			writeMessage(defender->getName() + " had more successes, taking initiative");
			switchInitiative();
		}
	} else {	
		//roll dice
		int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), attack.dice);
		int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defend.dice);

		int MoS = offenseSuccesses - defenseSuccesses;

		if(MoS > 0) {
			if(inflictWound(MoS, attack, defender) == true) {
				m_currentState = eCombatState::FinishedCombat;
				return;
			}
		}
		else if (MoS == 0) {
			//nothing happens
			writeMessage("no net successes");
		}
		else if (defend.manuever != eDefensiveManuevers::Dodge) {
			writeMessage("attack deflected with " + to_string(-MoS) + " successes");
			writeMessage(defender->getName() + " now has initative, becoming attacker");
			switchInitiative();
		}	
	}
	switchTempo();
	m_currentState = eCombatState::Offense;
	
}

void CombatManager::doDualOffenseResolve()
{
	//dual aggression
	Creature::Offense attack = m_side1->getQueuedOffense();
	Creature::Offense attack2 = m_side2->getQueuedOffense();

	int MoS = DiceRoller::rollGetSuccess(m_side1->getBTN(), attack.dice);
	int MoS2 = DiceRoller::rollGetSuccess(m_side2->getBTN(), attack2.dice);

	//resolve both
	bool death = false;
	if(MoS > 0) {
		if(inflictWound(MoS, attack, m_side2) == true) {
			death = true;
		}
	}
	if(MoS2 > 0) {
		if(inflictWound(MoS2, attack2, m_side1) == true) {
			death = true;
		}
	}

	switchTempo();
	
	//intiative goes to whoever got more hits
	m_currentState = eCombatState::Offense;
	if(MoS > MoS2) {
		m_initiative = eInitiative::Side1;
	} else if (MoS < MoS2) {
		m_initiative = eInitiative::Side2;
	} else {
		//reroll if no one died
		m_currentState = eCombatState::RollInitiative;
	}
	m_currentState = death == true ? eCombatState::FinishedCombat : m_currentState;
}

void CombatManager::doEndCombat()
{
	writeMessage("Combat has ended", Log::eMessageTypes::Announcement);
	m_side1 = nullptr;
	m_side2 = nullptr;
	m_currentState = eCombatState::Uninitialized;
}

bool CombatManager::inflictWound(int MoS, Creature::Offense attack, Creature* target, bool manueverFirst)
{
	eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(attack.target);

	int finalDamage = MoS + attack.component->getDamage();

	writeMessage(target->getName() + " received a level " + to_string(finalDamage) + " wound to " + bodyPartToString(bodyPart));
	Wound* wound = WoundTable::getSingleton()->getWound(attack.component->getType(), bodyPart, finalDamage);
	writeMessage(wound->getText(), Log::eMessageTypes::Damage);
	if(wound->getBTN() > target->getBTN())
	{
		writeMessage(target->getName() + " begins to struggle from the pain", Log::eMessageTypes::Alert);
	}
	target->inflictWound(wound, manueverFirst);

	if(wound->causesDeath() == true) {
		//end combat
		writeMessage(target->getName() + " has been killed", Log::eMessageTypes::Announcement);
		m_currentState = eCombatState::FinishedCombat;
		return true;
	}

	writeMessage("Wound impact causes " + target->getName() + " to lose " +
				 to_string(wound->getImpact()) + " action points!", Log::eMessageTypes::Alert);
	return false;
}

void CombatManager::switchTempo()
{
	if(m_currentTempo == eTempo::First) {
		m_currentTempo = eTempo::Second;
	} else {
		// reset combat pools
		writeMessage("Exchange has ended, combat pools have reset");
		m_currentTempo = eTempo::First;
		m_side1->resetCombatPool();
		m_side2->resetCombatPool();
	}
}

bool CombatManager::isAttackerPlayer()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	return attacker->isPlayer();
}

bool CombatManager::isDefenderPlayer()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	return defender->isPlayer();
}

void CombatManager::run()
{
	switch(m_currentState)
	{
	case eCombatState::Uninitialized:
		return;
		break;
	case eCombatState::Initialized:
		doInitialization();
		break;
	case eCombatState::RollInitiative:
		doRollInitiative();
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
	case eCombatState::DualOffense1:
		doDualOffense1();
		break;
	case eCombatState::DualOffense2:
		doDualOffense2();
		break;
	case eCombatState::Defense:
		doDefense();
		break;
	case eCombatState::Resolution:
		doResolution();
		break;
	case eCombatState::DualOffenseResolve:
		doDualOffenseResolve();
		break;
	case eCombatState::FinishedCombat:
		doEndCombat();
		break;
	}
}

void CombatManager::runUI()
{
	Game::getWindow();
}

void CombatManager::writeMessage(const std::string& str, Log::eMessageTypes type)
{
	//combat manager is not a singleton, so we can have multiple.
	//we can choose not to display combatmanager messages if we want to.
	Log::push(str, type);
}
