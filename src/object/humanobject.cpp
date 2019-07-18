#include "humanobject.h"
#include "utils.h"

HumanObject::HumanObject() : CreatureObject(new Human)
{
	m_creature->setWeapon(41); //arming sword
	m_creature->equipArmor(41); //chainmail
	m_creature->equipArmor(42); //sallet
	m_creature->setName("Sam");
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
