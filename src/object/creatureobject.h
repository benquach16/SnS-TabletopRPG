#pragma once

#include "object.h"
#include "../creatures/creature.h"

class CreatureObject : public Object
{
public:
	CreatureObject(Creature* creature);
	virtual ~CreatureObject();
	Creature* getCreatureComponent() { return m_creature; }
protected:
	Creature* m_creature;
};
