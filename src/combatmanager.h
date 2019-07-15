#pragma once

#include <vector>
#include "combatinstance.h"
#include "creatures/creature.h"

class CombatManager
{
public:
	CombatManager();
	~CombatManager();
	void run();
	CombatInstance* initializeCombat(Creature* side1, Creature* side2);
private:
	typedef std::pair<Creature*, Creature*> CreaturePair;
	
	std::map<CreaturePair, CombatInstance*> m_instances;
};
