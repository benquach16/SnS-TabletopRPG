#include <iostream>
#include "creatures/utils.h"
#include "weapons/utils.h"
#include "combatmanager.h"

#include <assert.h>

using namespace std;

CombatManager::CombatManager(): m_initiative(0), m_side1(nullptr), m_side2(nullptr), m_currentTempo(eTempo::First),
								m_side1CombatPool(0), m_side2CombatPool(0)  
{
}

void CombatManager::run()
{
	//do other stuff at some point
	if(m_side1 == nullptr || m_side2 == nullptr) return;
	m_side1->resetCombatPool();
	m_side2->resetCombatPool();

	do {
	//get offensive manuever and dice from side 1
	//then get defensive manuever and dice from side 2
	//then resolve		
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	if(m_initiative == 0) {
		attacker = m_side1;
		defender = m_side2;
	}
	else if(m_initiative == 1) {
		attacker = m_side2;
		defender = m_side1;
	}
	
	cout << attacker->getName() << " has initiative" << endl;
	
	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	
	int offenseCombatPool = attacker->getCombatPool();
	eOffensiveManuevers offense;
	int offenseDice;
	eHitLocations target;
	Component* offenseComponent = nullptr;

	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(offenseWeapon->getLength());
	reachCost = std::max(0, reachCost);
	cout << "Reach cost of " << reachCost << endl;
	attacker->doOffense(defender, offenseCombatPool, reachCost, offense, offenseDice, target, offenseComponent);
	assert(offenseDice <= offenseCombatPool);
	attacker->reduceCombatPool(offenseDice);
	
	assert(offenseComponent != nullptr);

	cout << attacker->getName() << " attacks with " << offenseWeapon->getName() << " using "
		 << offenseComponent->getName() << " with " << offenseDice << " dice" << endl;

	eDefensiveManuevers defense;
	int defenseDice;

	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();
	defender->doDefense(m_currentTempo == eTempo::Second, offenseDice, defenseCombatPool, defense, defenseDice);
	assert(defenseDice <= defenseCombatPool);
	defender->reduceCombatPool(defenseDice);
	
	cout << defender->getName() << " defends with " << defenseWeapon->getName() << " using " << defenseDice << " dice" << endl;

	int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), offenseDice);
	int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defenseDice);

	int MoS = offenseSuccesses - defenseSuccesses;

	if(MoS > 0) {
		cout << "got " << MoS << " net successes on attack" << endl;
		eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(target);

		int finalDamage = MoS + offenseComponent->getDamage();

		
		cout << "inflicted level " << finalDamage << " " << damageTypeToString(offenseComponent->getType())
			 << " wound to " << bodyPartToString(bodyPart) << endl;
		Wound* wound = WoundTable::getSingleton()->getWound(offenseComponent->getType(), bodyPart, finalDamage);
		cout << wound->getText() << endl;
		defender->inflictWound(wound);
	}
	else if (MoS == 0) {
		//nothing happens
		cout << "no net successes!" << endl;
	}
	else if (defense != eDefensiveManuevers::Dodge) {
		cout << "attack deflected with " << -MoS <<  endl;
		cout << defender->getName() << " now has initiative, becoming attacker" << endl;
		m_initiative = 1;
	}

	cout << attacker->getName() << " has " << attacker->getCombatPool() << " dice left" << endl;
	cout << defender->getName() << " has " << defender->getCombatPool() << " dice left" << endl;
	if(m_currentTempo == eTempo::First) {

		m_currentTempo = eTempo::Second;
	} else {
		// reset combat pools
		cout << "Combat pools have reset" << endl;
		m_currentTempo = eTempo::First;
		m_side1->resetCombatPool();
		m_side2->resetCombatPool();
	}

	} while(m_currentTempo != eTempo::First);
}
