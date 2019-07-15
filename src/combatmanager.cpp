#include "combatmanager.h"

using namespace std;

CombatManager::CombatManager()
{
}

CombatManager::~CombatManager()
{
	for(auto it : m_instances) {
		delete it.second;
	}
	m_instances.clear();
}

void CombatManager::run()
{
	for(auto it : m_instances) {
		it.second->run();
	}
}

CombatInstance* CombatManager::initializeCombat(Creature* side1, Creature* side2)
{
	CreaturePair pair(side1, side2);

	if(m_instances.find(pair) == m_instances.end()) {
		m_instances[pair] = new CombatInstance;
		m_instances[pair]->initCombat(side1, side2);
	}
}
