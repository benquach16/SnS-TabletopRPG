#include <iostream>
#include <algorithm>

#include "combatmanager.h"
#include "dice.h"
#include "log.h"

using namespace std;

CombatManager::CombatManager(Creature* creature) : m_currentId(0), m_mainCreature(creature),
												   m_side(eOutnumberedSide::None), m_currentTempo(eTempo::First),
												   m_currentState(eCombatManagerState::RunCombat),
												   m_positionDone(false)
{
	m_isPlayers = creature->isPlayer();
}

CombatManager::~CombatManager()
{
	cleanup();
}

void CombatManager::cleanup()
{
	for(auto it : m_instances) {
		delete it;
	}
	m_instances.clear();
	m_currentTempo = eTempo::First;
	m_side = eOutnumberedSide::None;
	m_currentId = 0;
}

bool CombatManager::run()
{
	if(m_mainCreature->isConscious() == false) {
		//cleanup, we don't need this anymore
		cleanup();
		//cout << "creature dead" << endl;
		return false;
	}
	
	if(m_instances.size() == 0) {
		m_currentId = 0;
		//cout << "no more instances" << endl;
		return false;
	}

	switch(m_currentState) {
	case eCombatManagerState::RunCombat:
		doRunCombat();
		break;
	case eCombatManagerState::PositioningRoll:
		doPositionRoll();
		break;
	}

	return true;

}

void CombatManager::doRunCombat()
{
	if(m_instances.size() > 1) {
		//force tempo
		m_instances[m_currentId]->forceTempo(eTempo::First);
		
		//do positioning roll
		if(m_currentId == 0 && m_positionDone == false && m_currentTempo == eTempo::First) {
			m_currentState = eCombatManagerState::PositioningRoll;
			return;
		}
		//force outnumbered side to be defensive
		//player should always be side 1
		if(m_isPlayers == true) {
			for(auto it : m_instances) {
				//bug : this state causes ui skips, make a seperate state
				if(it->getState() == eCombatState::RollInitiative) {
					//it->forceInitiative(eInitiative::Side2);
				}
				
			}
		}
		
	}
	
	bool change = m_instances[m_currentId]->getState() == eCombatState::PostResolution;
	
	//for loop here is O(slow) but should be fast enough for our cases since we dont expect N to be large
	if(find(m_activeInstances.begin(), m_activeInstances.end(), m_currentId) != m_activeInstances.end()) {
		m_instances[m_currentId]->run();
	} else {
		change = true;
	}

	if(m_instances[m_currentId]->getState() == eCombatState::Uninitialized) {		
		delete m_instances[m_currentId];
		m_instances.erase(m_instances.begin() + m_currentId);
		if(m_currentId == m_instances.size()) {
			m_currentId = 0;
		}
		writeMessage("Combatant has been killed, refreshing combat pools");
		refreshInstances();
		m_currentTempo = eTempo::First;
		
	}
	//since we just deleted, make sure we clear if we don't have any more combat
	if(m_instances.size() == 0) {
		m_currentId = 0;
	}
	if(change == true) {
		m_positionDone = false;
		cout << "swapping" << endl;
		if(m_currentId < m_instances.size() - 1) {
			m_currentId++;
		} else {
			if(m_currentTempo == eTempo::Second && m_instances.size() > 1) {
				writeMessage("Exchanges have ended, combat pools for all combatants have reset");
				refreshInstances();
			}
			switchInitiative();
			m_currentId = 0;
		}
	}
}

void CombatManager::doPositionRoll()
{
	//do player
	if(m_mainCreature->getHasPosition() == false) {
		m_currentState = eCombatManagerState::PositioningRoll;
		return;
	}
	for(auto it : m_instances) {
		// do positioning roll
		it->getSide2()->doPositionRoll(m_mainCreature);
	}
	m_activeInstances.clear();
	//now roll
	int mainSuccesses = DiceRoller::rollGetSuccess(m_mainCreature->getBTN(), m_mainCreature->getQueuedPosition().dice);
	cout << "mSuc" << mainSuccesses << endl;
	for(int i = 0; i < m_instances.size(); ++i) {
		Creature *creature = m_instances[i]->getSide2();
		int successes = DiceRoller::rollGetSuccess(creature->getBTN(), creature->getQueuedPosition().dice);
		if(successes > mainSuccesses) {
			cout << successes << endl;
			writeMessage(creature->getName() + " kept up with " + m_mainCreature->getName() + "'s footwork");
			m_activeInstances.push_back(i);
		}
	}
	//have to have at least one
	if(m_activeInstances.size() == 0) {
		writeMessage("No combatants kept up with footwork, initiating duel");
		m_activeInstances.push_back(0);
	}
	m_currentState = eCombatManagerState::RunCombat;
	m_positionDone = true;
}

void CombatManager::startCombatWith(Creature* creature)
{
	//if a creature initiatives combat against another creature, but is not the main creature then we spin up another combatmanger
	CombatInstance* instance = new CombatInstance;
	if(m_instances.size() == 1) {
		//ptr compares are gross, do ID compares
		if(creature == m_instances[0]->getSide1()) {
			m_side = eOutnumberedSide::Side1;
		} else {
			m_side = eOutnumberedSide::Side2;
		}
	}
	writeMessage("Combat started between " + m_mainCreature->getName() + " and " + creature->getName(), Log::eMessageTypes::Announcement);

	instance->initCombat(m_mainCreature, creature);
	m_instances.push_back(instance);
}

CombatInstance* CombatManager::getCurrentInstance() const
{
	assert(m_currentId < m_instances.size());
	return m_instances[m_currentId];
}

void CombatManager::refreshInstances()
{
	for(auto it: m_instances) {
		it->forceRefresh();
	}
}

void CombatManager::writeMessage(const std::string& str, Log::eMessageTypes type)
{
	//combat manager is not a singleton, so we can have multiple.
	//we can choose not to display combatmanager messages if we want to.
	Log::push(str, type);
}
