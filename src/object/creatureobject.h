#pragma once

#include <map>

#include "object.h"
#include "../creatures/creature.h"
#include "../components/aicontroller.h"
#include "types.h"

class Level;

class CreatureObject : public Object
{
public:
	CreatureObject(Creature* creature);
	virtual ~CreatureObject();
	bool hasCollision() const override { return true; }
	bool deleteMe() const override { return m_creature->getCreatureState() == eCreatureState::Dead; }
	Creature* getCreatureComponent() const { return m_creature; }
	const std::string getName() const { return m_creature->getName(); }
	virtual eCreatureFaction getFaction() const { return m_creatureFaction; }
	virtual eCreatureRace getRace() const = 0;
	virtual eObjectTypes getObjectType() const { return eObjectTypes::Creature; }

	virtual void run(const Level*);

protected:
	Creature* m_creature;
	eCreatureFaction m_creatureFaction;
	eCreatureRace m_creatureRace;

	AIController m_controller;

	std::map<int, int> m_inventory;
	int m_money;

	int m_thirst;
	int m_hunger;
	int m_exhaustion;
};
