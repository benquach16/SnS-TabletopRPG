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
						 eOffensiveManuevers& outOffense, int& outDice, eHitLocations& outLocation)
{
	outOffense = eOffensiveManuevers::Thrust;
	outDice = diceLeft;
	outLocation = eHitLocations::Head;
}
							 
