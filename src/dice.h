#pragma once

#include <stdlib.h>
#include <vector>

namespace DiceRoller {
	constexpr int cDiceSides = 6;

	inline int roll() { return rand()%cDiceSides + 1; }
	//bool rollGetSuccess(int BTN) { return roll() >= BTN; }

	std::vector<int> roll(int number);
	int rollGetSuccess(int BTN, int number);
	
};

