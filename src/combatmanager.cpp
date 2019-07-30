#include <iostream>

#include "combatmanager.h"

using namespace std;

CombatManager::CombatManager(Creature* creature) : m_currentId(0), m_mainCreature(creature)
{
}

CombatManager::~CombatManager()
{
	for(auto it : m_instances) {
		delete it;
	}
	m_instances.clear();
}

bool CombatManager::run()
{
	if(m_mainCreature->isConscious() == false) {
		//cleanup, we don't need this anymore
		cout << "creature dead" << endl;
		return false;
	}
	
	if(m_instances.size() == 0) {
		cout << "no more instances" << endl;
		return false;
	}
	if(m_instances.size() > 1 && m_currentId == 0) {
		//do positioning roll
	}
	
	bool change = m_instances[m_currentId]->getState() == eCombatState::PostCombat;
	m_instances[m_currentId]->run();

	if(m_instances[m_currentId]->getState() == eCombatState::Uninitialized) {
		delete m_instances[m_currentId];
		m_instances.erase(m_instances.begin() + m_currentId);
	}
	//since we just deleted, make sure we clear if we don't have any more combat
	if(m_instances.size() == 0) {
		cout << "no more instances" << endl;
		return false;
	}
	if(change == true) {
		cout << "swapping" << endl;
		if(m_currentId < m_instances.size() - 1) {
			m_currentId++;
		} else {
			m_currentId = 0;
		}
	}
	return true;

}

void CombatManager::startCombatWith(Creature* creature)
{
	CombatInstance* instance = new CombatInstance;
	instance->initCombat(m_mainCreature, creature);
	m_instances.push_back(instance);
}

CombatInstance* CombatManager::getCurrentInstance() const
{
	assert(m_currentId < m_instances.size());
	return m_instances[m_currentId];
}
