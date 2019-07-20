#pragma once

#include "creatureobject.h"
#include "../creatures/player.h"
#include "../combatinstance.h"

class PlayerObject : public CreatureObject
{
public:
	PlayerObject();
	~PlayerObject();
	void startCombatWith(Creature* creature);
	bool runCombat();
	CombatInstance& getCombatInstance() { return m_instance; }
	
	eCreatureFaction getFaction() const override { return eCreatureFaction::Player; }
	eCreatureRace getRace() const override { return eCreatureRace::Human; }
	std::string getDescription() const override { return "This is you"; }
	eObjectTypes getObjectType() const override { return eObjectTypes::Player; }

private:
	CombatInstance m_instance;
};
