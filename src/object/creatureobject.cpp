#include "creatureobject.h"

CreatureObject::CreatureObject(Creature* creature) : m_creature(creature),
													 m_creatureFaction(eCreatureFaction::None)
{
}

CreatureObject::~CreatureObject()
{
	if(m_creature != nullptr) {
		delete m_creature;
		m_creature = nullptr;
	}
}

void CreatureObject::run(const Level* level)
{
	
}
