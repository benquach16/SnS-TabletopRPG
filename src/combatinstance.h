// This is a complex class, since this combat in TROS-likes is complicated
// This flow specifically follows the combat system in the rulebook.
// Without reading the rulebook, this will not make sense to you.

#pragma once

#include <queue>
#include <unordered_map>

#include "creatures/creature.h"
#include "items/types.h"
#include "log.h"
#include "dice.h"

enum class eTempo : unsigned
{
	First,
	Second
};

enum class eInitiative : unsigned
{
	Side1,
	Side2
};

enum class eCombatState : unsigned
{
	Uninitialized = 0,
	Initialized = 1,
	RollInitiative = 2,
	PreexchangeActions = 3,
	ResetState = 4, // used only for ui to observe combat manager state
	DualOffenseStealInitiative = 5,
	DualOffense1 = 6,
	DualOffense2 = 7,
	DualOffenseSecondInitiative = 8,
	Offense = 9,
	StolenOffense = 10,
	Defense = 11,
	ParryLinked = 12,
	StealInitiative = 13,
	PostDefense = 14,
	Resolution = 15,
	DualOffenseResolve = 16,
	PostResolution = 17,
	FinishedCombat = 18
};

class CombatInstance
{
public:	
	CombatInstance();
	void run();
	void runUI();

	void initCombat(Creature* side1, Creature* side2);
	void forceInitiative(eInitiative initiative);
	eCombatState getState() const { return m_currentState; }
	Creature* getSide1() const { return m_side1; }
	Creature* getSide2() const { return m_side2; }
	eLength getCurrentReach() const { return m_currentReach; }
	bool isAttackerPlayer();
	bool isDefenderPlayer();
	void forceRefresh();
	void forceTempo(eTempo tempo) { m_currentTempo = tempo; }
private:
	void doInitialization();
	void doRollInitiative();
	void doPreexchangeActions();
	void doResetState();
	void doDualOffense1();
	void doDualOffenseStealInitiative();
	void doDualOffense2();
	void doDualOffenseSecondInitiative();
	void doStolenOffense();
	//doOffense has a return type to poll player input for dual offense
	bool doOffense();
	void doDefense();
	void doParryLinked();
	void doStealInitiative();
	void doPostDefense();
	void doResolution();
	//this is a special case because if both sides roll to attack since both of their attacks resolve at the same time
	void doDualOffenseResolve();
	void doPostResolution();
	void doEndCombat();
	void setSides(Creature*& attacker, Creature*& defender);

	void switchInitiative() { m_initiative = m_initiative == eInitiative::Side1 ? eInitiative::Side2 : eInitiative::Side1; }
	void switchTempo();

	bool inflictWound(int MoS, Offense attack, Creature* target, bool manueverFirst = false);
	void writeMessage(const std::string& str, Log::eMessageTypes type = Log::eMessageTypes::Standard);
	void outputReachCost(int cost, Creature* attacker);
	
	eCombatState m_currentState;

	eLength m_currentReach;

	eTempo m_currentTempo;
	eInitiative m_initiative;

	int m_dualWhiteTimes;
	bool m_dualRedThrow;
	bool m_inWind;
	bool m_inGrapple;

	Creature* m_side1;
	Creature* m_side2;

	int m_numTempos;
};
