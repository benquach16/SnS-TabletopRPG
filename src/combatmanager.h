#pragma once

#include <vector>
#include "combatinstance.h"
#include "creatures/creature.h"

class CombatManager
{
public:
	CombatManager(Creature* creature);
	~CombatManager();
	bool run();
	CombatInstance* getCurrentInstance() const;
	void startCombatWith(Creature* creature);
private:
	std::vector<CombatInstance*> m_instances;
	int m_currentId;
	Creature* m_mainCreature;
};
