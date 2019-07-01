#pragma once

#include "creatures/creature.h"
#include "weapons/types.h"
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
	Offense,
	Defense,
	Resolution,
	FinishedCombat
};

class CombatManager
{
public:

	
	CombatManager();
	void run();

	void initCombat(Creature* side1, Creature* side2);
	void doInitialization();
	void doOffense();
	void doOffensePlayer();
	void doDefense();
	void doDefensePlayer();
	void doResolution();
	void doEndCombat();
	void setSide1(Creature* creature);
	void setSide2(Creature* creature);

	void writeMessage(const std::string& str, Log::eMessageTypes type = Log::eMessageTypes::Standard);
private:
	struct offense {
		eOffensiveManuevers offense;
		int offenseDice;
		eHitLocations target;
		Component* offenseComponent = nullptr;
	};

	struct defense {
		eDefensiveManuevers defense;
		int defenseDice;
	};

	offense m_offense;
	defense m_defense;
	
	eCombatState m_currentState;
	
	eTempo m_currentTempo;
	eInitiative m_initiative;

	Creature* m_side1;
	Creature* m_side2;
};
