#include "humanobject.h"
#include "utils.h"

HumanObject::HumanObject() : CreatureObject(new Human)
{
	m_creature->setWeapon(1041); //arming sword
	m_creature->equipArmor(2041); //chainmail
	m_creature->equipArmor(2051); //maille coif
	m_creature->setName("Sam");

	m_inventory[0] = 10;
	m_inventory[1041] = 1;
	m_inventory[2041] = 1;
	m_inventory[2051] = 1;
	
	m_creatureFaction = eCreatureFaction::Bandit;
}

HumanObject::~HumanObject()
{
}

std::string HumanObject::getDescription() const
{
	std::string ret;
	ret += m_creature->getName() + ", human " + factionToString(m_creatureFaction) + " armed with " +
		m_creature->getPrimaryWeapon()->getName();

	return ret;
}
