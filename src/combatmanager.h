#pragma once

#include "creatures/creature.h"
#include "weapons/types.h"
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
	void doDefense();
	void doResolution();
	void setSide1(Creature* creature);
	void setSide2(Creature* creature);
	void resetSides() { m_side1 = nullptr; m_side2 = nullptr; }
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
	int m_initiative;

	Creature* m_side1;
	Creature* m_side2;
};
