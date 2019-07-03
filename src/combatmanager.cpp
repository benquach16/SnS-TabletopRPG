#include <iostream>
#include "creatures/utils.h"
#include "weapons/utils.h"
#include "combatmanager.h"
#include "game.h"

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

	m_currentState = eCombatState::Offense;
}

void CombatManager::doRollInitiative()
{
	//get initiative rolls from both sides to determine roles.
	eInitiativeRoll side1 = m_side1->doInitiative();
	eInitiativeRoll side2 = m_side2->doInitiative();
	if(side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Defend) {
		//repeat
		writeMessage("Both sides chose to defend, rolling for initiative again");
		m_currentState = eCombatState::RollInitiative;
		return;
	} else if(side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Defend) {
		writeMessage(m_side1->getName() + " has initiative");
		m_initiative = eInitiative::Side1;
		m_currentState = eCombatState::Offense;
		return;
	} else if(side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Attack) {
		writeMessage(m_side2->getName() + " has initiative");
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

		m_initiative = side1Successes > side2Successes ? eInitiative::Side1 : eInitiative::Side2;
		return;
	}

	m_currentState = eCombatState::RollInitiative;
}

void CombatManager::doOffense()
{
	//get offensive manuever and dice from side 1
	//then get defensive manuever and dice from side 2
	//then resolve		
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	if(attacker->isPlayer() == true)
	{
		//wait until we get input from player
		m_currentState = eCombatState::Offense;
	}
	
	if(attacker->getCombatPool() <= 0 && defender->getCombatPool() > 0) {
		writeMessage(attacker->getName() + " has no more action points! Initiative swaps to defender");
		switchInitiative();
		setSides(attacker, defender);
	}
   
	if(attacker->isPlayer() == true) {
		doOffensePlayer();
		return;
	}
	
	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	
	int offenseCombatPool = attacker->getCombatPool();

	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(offenseWeapon->getLength());

	reachCost = std::max(0, reachCost);
	attacker->doOffense(defender, reachCost);
	Creature::Offense attack = attacker->getQueuedOffense();
	
	assert(attack.component != nullptr);
	assert(attack.dice <= offenseCombatPool);
	attacker->reduceCombatPool(attack.dice);
	
	writeMessage(attacker->getName() + " " + offensiveManueverToString(attack.manuever) + "s with " +
				 offenseWeapon->getName() + " using " +
				 attack.component->getName() + " with " +
				 to_string(attack.dice) + " action points");
	
	m_currentState = eCombatState::Defense;
}

void CombatManager::doDualOffense()
{
	//both sides rolled red
}

void CombatManager::doOffensePlayer()
{
	//in this case, we wait for player input before switching states
	m_currentState = eCombatState::Offense;
}

void CombatManager::doDefense()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	if(defender->isPlayer() == true) {
		//wait until player inputs
		m_currentState = eCombatState::Defense;
	}
	Weapon* defenseWeapon = defender->getPrimaryWeapon();

	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();
	defender->doDefense(m_currentTempo == eTempo::Second, attacker->getQueuedOffense().dice);
	Creature::Defense defend = defender->getQueuedDefense();
	assert(defend.dice <= defenseCombatPool);
	defender->reduceCombatPool(defend.dice);
	
	writeMessage(defender->getName() + " defends with " + defenseWeapon->getName() +
				 " using " + to_string(defend.dice) + " action points");
	
	m_currentState = eCombatState::Resolution;
}

void CombatManager::doDefensePlayer()
{
	m_currentState = eCombatState::Defense;
}

void CombatManager::doResolution()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	Creature::Offense attack = attacker->getQueuedOffense();
	Creature::Defense defend = defender->getQueuedDefense();
	
	//roll dice
	int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), attack.dice);
	int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defend.dice);

	int MoS = offenseSuccesses - defenseSuccesses;

	if(MoS > 0) {
		eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(attack.target);

		int finalDamage = MoS + attack.component->getDamage();

		writeMessage("inflicted level " + to_string(finalDamage) + " wound to " + bodyPartToString(bodyPart));
		Wound* wound = WoundTable::getSingleton()->getWound(attack.component->getType(), bodyPart, finalDamage);
		writeMessage(wound->getText(), Log::eMessageTypes::Damage);
		if(wound->getBTN() > defender->getBTN())
		{
			writeMessage(defender->getName() + " begins to struggle from the pain", Log::eMessageTypes::Alert);
		}
		defender->inflictWound(wound);

		if(wound->causesDeath() == true) {
			//end combat
			writeMessage(defender->getName() + " has been killed", Log::eMessageTypes::Announcement);
			m_currentState = eCombatState::FinishedCombat;
			return;
		}

		writeMessage("Wound impact causes " + defender->getName() + " to lose " +
					 to_string(wound->getImpact()) + " action points!", Log::eMessageTypes::Alert);

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
	if(m_currentTempo == eTempo::First) {
		m_currentTempo = eTempo::Second;
	} else {
		// reset combat pools
		writeMessage("Exchange has ended, combat pools have reset");
		m_currentTempo = eTempo::First;
		m_side1->resetCombatPool();
		m_side2->resetCombatPool();
	}	
	m_currentState = eCombatState::Offense;
}

void CombatManager::doEndCombat()
{
	writeMessage("Combat has ended", Log::eMessageTypes::Announcement);
	m_side1 = nullptr;
	m_side2 = nullptr;
	m_currentState = eCombatState::Uninitialized;
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
	case eCombatState::Offense:
		doOffense();
		break;
	case eCombatState::Defense:
		doDefense();
		break;
	case eCombatState::Resolution:
		doResolution();
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
