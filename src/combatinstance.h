// This is a complex class, since this combat in TROS-likes is complicated
// This flow specifically follows the combat system in the rulebook.
// Without reading the rulebook, this will not make sense to you.

#pragma once

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
	Uninitialized,
	Initialized,
	RollInitiative,
	ResetState, // used only for ui to observe combat manager state
	DualOffense1,
	DualOffense2,
	Offense,
	StolenOffense,
	Defense,
	ParryLinked,
	StealInitiative,
	Resolution,
	DualOffenseResolve,
	FinishedCombat
};

class CombatInstance
{
public:	
	CombatInstance();
	void run();
	void runUI();

	void initCombat(Creature* side1, Creature* side2);

	eCombatState getState() const { return m_currentState; }
	Creature* getSide1() const { return m_side1; }
	Creature* getSide2() const { return m_side2; }
	bool isAttackerPlayer();
	bool isDefenderPlayer();
private:
	void doInitialization();
	void doRollInitiative();
	void doResetState();
	void doDualOffense1();
	void doDualOffense2();
	void doStolenOffense();
	//doOffense has a return type to poll player input for dual offense
	bool doOffense();
	void doDefense();
	void doParryLinked();
	void doStealInitiative();
	void doResolution();
	//this is a special case because if both sides roll to attack since both of their attacks resolve at the same time
	void doDualOffenseResolve();
	void doEndCombat();
	void setSides(Creature*& attacker, Creature*& defender);

	void switchInitiative() { m_initiative = m_initiative == eInitiative::Side1 ? eInitiative::Side2 : eInitiative::Side1; }
	void switchTempo();

	bool inflictWound(int MoS, Creature::Offense attack, Creature* target, bool manueverFirst = false);
	void writeMessage(const std::string& str, Log::eMessageTypes type = Log::eMessageTypes::Standard);
	
	eCombatState m_currentState;

	eLength m_currentReach;

	eTempo m_currentTempo;
	eInitiative m_initiative;

	int m_dualWhiteTimes;

	Creature* m_side1;
	Creature* m_side2;
};
