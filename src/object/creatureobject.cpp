#include "creatureobject.h"

CreatureObject::CreatureObject(Creature* creature) : m_creature(creature)
{
}

CreatureObject::~CreatureObject()
{
	if(m_creature != nullptr) {
		delete m_creature;
	}
}
