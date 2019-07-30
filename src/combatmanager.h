#pragma once

#include <vector>
#include <string>

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
	void writeMessage(const std::string& str);
	std::vector<CombatInstance*> m_instances;
	int m_currentId;
	Creature* m_mainCreature;
	bool m_isPlayers;
};
