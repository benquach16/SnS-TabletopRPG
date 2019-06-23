#pragma once

#include "creatures/creature.h"
#include "weapons/types.h"
#include "dice.h"

enum class eTempo : unsigned
{
	First,
	Second
};

class CombatManager
{
public:
	CombatManager();
	void run();

	void setSide1(Creature* creature) { m_side1 = creature; }
	void setSide2(Creature* creature) { m_side2 = creature; }
	void resetSides() { m_side1 = nullptr; m_side2 = nullptr; }
private:
	eTempo m_currentTempo;
	int m_initiative;
	Creature* m_side1;
	Creature* m_side2;
};
