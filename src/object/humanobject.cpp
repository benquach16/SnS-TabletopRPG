#include "humanobject.h"

HumanObject::HumanObject() : CreatureObject(new Human)
{
	m_creature->setWeapon(41); //arming sword
	m_creature->equipArmor(41); //chainmail
	m_creature->equipArmor(42); //sallet
	m_creature->setName("Sam");
}

HumanObject::~HumanObject()
{
}
