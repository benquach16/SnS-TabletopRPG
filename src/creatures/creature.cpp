#include "creature.h"

Creature::Creature() : m_BTN(cBaseBTN), m_brawn(1), m_agility(1),
					   m_cunning(1), m_perception(1), m_will(1), m_primaryWeaponId(0)
{
	
}

Weapon* Creature::getPrimaryWeapon()
{
	return WeaponTable::getSingleton()->get(m_primaryWeaponId);
}

void Creature::doOffense(Creature* target, int diceLeft,
						 eOffensiveManuevers& outOffense, int& outDice,
						 eHitLocations& outLocation, Component*& outComponent)
{
	Weapon* weapon = getPrimaryWeapon();

	outComponent = weapon->getBestAttack();	
	outOffense = eOffensiveManuevers::Thrust;

	if(outComponent->getAttack() == eAttacks::Swing) {
		outOffense = eOffensiveManuevers::Swing;
	}
	outDice = diceLeft;
	outLocation = eHitLocations::Chest;

}
							 
