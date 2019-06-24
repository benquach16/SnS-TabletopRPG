#include <iostream>
#include "creatures/utils.h"
#include "weapons/utils.h"
#include "combatmanager.h"

#include <assert.h>

using namespace std;

CombatManager::CombatManager(): m_initiative(0), m_side1(nullptr), m_side2(nullptr), m_currentTempo(eTempo::First)
{
}

void CombatManager::run()
{
	//do other stuff at some point
	if(m_side1 == nullptr || m_side2 == nullptr) return;
	
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

	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	int offenseCombatPool = attacker->getProficiency(offenseWeapon->getType()) + attacker->getReflex();
	eOffensiveManuevers offense;
	int offenseDice;
	eHitLocations target;
	Component* offenseComponent = nullptr;

	attacker->doOffense(defender, offenseCombatPool, offense, offenseDice, target, offenseComponent);

	assert(offenseComponent != nullptr);

	cout << "attacker attacks with " << offenseWeapon->getName() << " using "
		 << offenseComponent->getName() << " with " << offenseDice << " dice" << endl;

	eDefensiveManuevers defense = eDefensiveManuevers::Parry;
	int defenseDice = 1;
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();

	cout << "defender defends with " << defenseWeapon->getName() << " using " << defenseDice << " dice" << endl;

	int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), offenseDice);
	int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defenseDice);

	int MoS = offenseSuccesses - defenseSuccesses;

	if(MoS > 0) {
		cout << "got " << MoS << " net successes on attack" << endl;
		eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(target);

		int finalDamage = MoS + offenseComponent->getDamage();
		cout << "inflicted level " << finalDamage << " wound to " << bodyPartToString(bodyPart) << endl;
	}
	else if (defense != eDefensiveManuevers::Dodge) {
		cout << "attack deflected" << endl;
		m_initiative = 1;
	}

	if(m_currentTempo == eTempo::First) {
		m_currentTempo = eTempo::Second;
	} else {
		// reset combat pools
		m_currentTempo = eTempo::First;
	}
}
