#include <iostream>

#include "playerobject.h"

PlayerObject::PlayerObject() : CreatureObject(new Player)
{
	m_creature->setWeapon(1040);
	m_creature->equipArmor(2041);
	m_creature->equipArmor(2052);
	m_creature->equipArmor(2044);
	m_creature->equipArmor(2040);
	m_creature->equipArmor(2046);
	m_creature->setName("John");

	m_inventory[0] = 50;
	m_inventory[1] = 5;
	m_inventory[2] = 1;
	m_inventory[3] = 1;
	m_inventory[4] = 1;
}

PlayerObject::~PlayerObject()
{

}

void PlayerObject::startCombatWith(Creature* creature)
{
	assert(m_instance.getState() == eCombatState::Uninitialized);
	std::cout << static_cast<int>(creature->getCreatureState()) << std::endl;
	m_instance.initCombat(m_creature, creature);
}

bool PlayerObject::runCombat()
{
	m_instance.run();
	return m_instance.getState() != eCombatState::Uninitialized;
}
