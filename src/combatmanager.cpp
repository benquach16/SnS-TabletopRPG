#include <iostream>
#include "utils.h"
#include "combatmanager.h"

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
		Creature* attacker = m_side1;
		Creature* defender = m_side2;
	}
	else if(m_initiative == 1) {
		Creature* attacker = m_side2;
		Creature* defender = m_side1;
	}

	Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	int offenseCombatPool = attacker->getProficiency(offenseWeapon->getType()) + attacker->getReflex();
	eOffensiveManuevers offense = eOffensiveManuevers::Thrust;
	int offenseDice = 7;
	eHitLocations target = eHitLocations::Head;

	eDefensiveManuevers defense = eDefensiveManuevers::Parry;
	int defenseDice = 7;
	Weapon* defenseWeapon = defender->getPrimaryWeapon();
	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();

	int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), offenseDice);
	int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defenseDice);

	int MoS = offenseSuccesses - defenseSuccesses;

	if(MoS > 0) {
		cout << "got " << MoS << " net successes on attack" << endl;
		eBodyParts bodyPart = WoundTable::getSingleton()->getThrust(target);
		cout << bodyPartToString(bodyPart) << endl;
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
