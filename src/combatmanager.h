#pragma once

#include <vector>
#include <string>
#include <set>

#include "combatinstance.h"
#include "creatures/creature.h"

enum class eCombatManagerState {
	RunCombat,
	PositioningRoll
};

class CombatManager
{
public:
	static constexpr float cTick = 0.6;
	CombatManager(Creature* creature);
	~CombatManager();
	bool run(float tick);
	CombatInstance* getCurrentInstance() const;
	void startCombatWith(Creature* creature);
	eCombatManagerState getState() const { return m_currentState; }
private:

	enum class eOutnumberedSide {
		None,
		Side1,
		Side2
	};
	void doRunCombat();
	void doPositionRoll();
	void cleanup();
	void switchInitiative() { m_currentTempo = (m_currentTempo == eTempo::First) ? eTempo::Second : eTempo::First; }
	void refreshInstances();
	void writeMessage(const std::string& str, Log::eMessageTypes type = Log::eMessageTypes::Standard);
	
	std::vector<CombatInstance*> m_instances;
	std::set<Creature*> m_queuedCreatures;
	std::vector<int> m_activeInstances;
	int m_currentId;
	int m_instanceId;
	bool m_positionDone;
	bool m_allowNewCreatures;
	Creature* m_mainCreature;
	bool m_isPlayers;
	eOutnumberedSide m_side;
	eTempo m_currentTempo;
	eCombatManagerState m_currentState;
};
