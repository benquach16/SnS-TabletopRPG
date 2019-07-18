#include "playerobject.h"

PlayerObject::PlayerObject() : CreatureObject(new Player)
{
	m_creature->setWeapon(40);
	m_creature->equipArmor(41);
	m_creature->equipArmor(42);
	m_creature->setName("John");
}

PlayerObject::~PlayerObject()
{

}

void PlayerObject::startCombatWith(Creature* creature)
{
	assert(m_instance.getState() == eCombatState::Uninitialized);
	m_instance.initCombat(m_creature, creature);
}

bool PlayerObject::runCombat()
{
	m_instance.run();
	return m_instance.getState() != eCombatState::FinishedCombat;
}
