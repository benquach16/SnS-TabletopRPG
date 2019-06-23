#pragma once

#include "weapons/types.h"

class AIManager
{
public:
	void doDefense(eOffensiveManuevers manuever, int diceAllocated);
	void doOffense();	
};
