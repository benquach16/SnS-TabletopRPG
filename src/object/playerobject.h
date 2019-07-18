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
	void runCombat() { m_instance.run(); }
	CombatInstance& getCombatInstance() { return m_instance; }
	
private:
	CombatInstance m_instance;
};
