#include <iostream>
#include "creatures/utils.h"
#include "weapons/utils.h"
#include "combatmanager.h"

#include <assert.h>

using namespace std;

CombatManager::CombatManager(): m_initiative(eInitiative::Side1), m_side1(nullptr), m_side2(nullptr),
								m_currentTempo(eTempo::First), m_currentState(eCombatState::Uninitialized)
{
}

void CombatManager::setSide1(Creature* creature)
{
	m_side1 = creature;
	m_side1->resetCombatPool();
}

void CombatManager::setSide2(Creature* creature)
{
	m_side2 = creature;
	m_side2->resetCombatPool();
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

void CombatManager::doOffense()
{
	//get offensive manuever and dice from side 1
	//then get defensive manuever and dice from side 2
	//then resolve		
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	if(m_initiative == eInitiative::Side1) {
		attacker = m_side1;
		defender = m_side2;
	}
	else if(m_initiative == eInitiative::Side2) {
		attacker = m_side2;
		defender = m_side1;
	}
	writeMessage(attacker->getName() + " has initiative");
	
	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	
	int offenseCombatPool = attacker->getCombatPool();

	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(offenseWeapon->getLength());
	reachCost = std::max(0, reachCost);

	attacker->doOffense(defender, reachCost,
						m_offense.offense, m_offense.offenseDice, m_offense.target, m_offense.offenseComponent);
	assert(m_offense.offenseComponent != nullptr);
	assert(m_offense.offenseDice <= offenseCombatPool);
	
	attacker->reduceCombatPool(m_offense.offenseDice);
	
	writeMessage(attacker->getName() + " attacks with " + offenseWeapon->getName() + " using " +
			  m_offense.offenseComponent->getName()
			  + " with " + to_string(m_offense.offenseDice) + " dice");
	
	m_currentState = eCombatState::Defense;
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
	if(m_initiative == eInitiative::Side1) {
		attacker = m_side1;
		defender = m_side2;
	}
	else if(m_initiative == eInitiative::Side2) {
		attacker = m_side2;
		defender = m_side1;
	}
	Weapon* defenseWeapon = defender->getPrimaryWeapon();

	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();
	defender->doDefense(m_currentTempo == eTempo::Second, m_offense.offenseDice,
						m_defense.defense, m_defense.defenseDice);
	assert(m_defense.defenseDice <= defenseCombatPool);
	defender->reduceCombatPool(m_defense.defenseDice);
	
	writeMessage(defender->getName() + " defends with " + defenseWeapon->getName() + " using " + to_string(m_defense.defenseDice) + " dice");
	
	m_currentState = eCombatState::Resolution;
}

void CombatManager::doResolution()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	if(m_initiative == eInitiative::Side1) {
		attacker = m_side1;
		defender = m_side2;
	}
	else if(m_initiative == eInitiative::Side2) {
		attacker = m_side2;
		defender = m_side1;
	}
	
	//roll dice
	int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), m_offense.offenseDice);
	int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), m_defense.defenseDice);

	int MoS = offenseSuccesses - defenseSuccesses;

	if(MoS > 0) {
		eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(m_offense.target);

		int finalDamage = MoS + m_offense.offenseComponent->getDamage();
		
		cout << "inflicted level " << finalDamage << " " << damageTypeToString(m_offense.offenseComponent->getType())
			 << " wound to " << bodyPartToString(bodyPart) << endl;
		writeMessage("inflicted level " + to_string(finalDamage) + " wound to " + bodyPartToString(bodyPart));
		Wound* wound = WoundTable::getSingleton()->getWound(m_offense.offenseComponent->getType(), bodyPart, finalDamage);
		writeMessage(wound->getText(), Log::eMessageTypes::Damage);
		cout << wound->getText() << endl;
		defender->inflictWound(wound);

		if(wound->causesDeath() == true) {
			//end combat
			writeMessage(defender->getName() + " has been killed", Log::eMessageTypes::Announcement);
			m_currentState = eCombatState::FinishedCombat;
			return;
		}
	}
	else if (MoS == 0) {
		//nothing happens
		writeMessage("no net successes");
	}
	else if (m_defense.defense != eDefensiveManuevers::Dodge) {
		writeMessage("attack deflected with " + to_string(-MoS));
		writeMessage(defender->getName() + " now has initative, becoming attacker");
		
		m_initiative = m_initiative == eInitiative::Side1 ? eInitiative::Side2 : eInitiative::Side1;
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

void CombatManager::writeMessage(const std::string& str, Log::eMessageTypes type)
{
	//combat manager is not a singleton, so we can have multiple.
	//we can choose not to display combatmanager messages if we want to.
	Log::push(str, type);
}
