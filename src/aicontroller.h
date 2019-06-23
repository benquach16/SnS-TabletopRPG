#pragma once

#include "weapons/types.h"
#include "creatures/creature.h"

class AIManager
{
public:
	void doDefense(eOffensiveManuevers manuever, int diceAllocated);
	void doOffense(Creature* target);
};
