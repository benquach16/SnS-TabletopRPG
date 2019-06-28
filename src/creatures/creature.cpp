#include "creature.h"

using namespace std;

Creature::Creature() : m_BTN(cBaseBTN), m_brawn(1), m_agility(1),
					   m_cunning(1), m_perception(1), m_will(1), m_primaryWeaponId(0), m_combatPool(0)
{
	
}

Weapon* Creature::getPrimaryWeapon()
{
	return WeaponTable::getSingleton()->get(m_primaryWeaponId);
}

void Creature::inflictWound(Wound* wound)
{
	m_BTN = max(m_BTN, wound->getBTN());
	m_combatPool -= wound->getImpact();
	m_wounds.push_back(wound);
}

void Creature::resetCombatPool()
{
	Weapon* weapon = getPrimaryWeapon();
	m_combatPool = getProficiency(weapon->getType()) + getReflex();
}

void Creature::doOffense(Creature* target, int diceLeft, int reachCost,
						 eOffensiveManuevers& outOffense, int& outDice,
						 eHitLocations& outLocation, Component*& outComponent)
{
	Weapon* weapon = getPrimaryWeapon();
	m_combatPool -= reachCost;
	outComponent = weapon->getBestAttack();	
	outOffense = eOffensiveManuevers::Thrust;

	if(outComponent->getAttack() == eAttacks::Swing) {
		outOffense = eOffensiveManuevers::Swing;
	}
	outDice = m_combatPool / 2;
	outLocation = eHitLocations::Head;
}


void Creature::doDefense(bool isLastTempo, int diceAllocated, int diceLeft, eDefensiveManuevers& outDefense, int& outDice)
{
	outDefense = eDefensiveManuevers::Parry;
	if(isLastTempo == true) {
		//use all dice because we're going to refresh anyway
		outDice = m_combatPool;
		return;
	}
	outDice = std::min(diceAllocated + 1, diceLeft);
}
							 
