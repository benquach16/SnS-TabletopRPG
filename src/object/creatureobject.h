#pragma once

#include "object.h"
#include "../creatures/creature.h"
#include "types.h"

class CreatureObject : public Object
{
public:
	CreatureObject(Creature* creature);
	virtual ~CreatureObject();
	bool hasCollision() const override { return true; }
	bool deleteMe() const override { return m_creature->getCreatureState() == eCreatureState::Dead; }
	Creature* getCreatureComponent() const { return m_creature; }
	virtual eCreatureFaction getFaction() const { return m_creatureFaction; }
	virtual eCreatureRace getRace() const = 0;
	virtual eObjectTypes getObjectType() const { return eObjectTypes::Creature; }

protected:
	Creature* m_creature;
	eCreatureFaction m_creatureFaction;
	eCreatureRace m_creatureRace;
};
