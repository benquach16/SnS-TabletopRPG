#pragma once

#include <stdlib.h>
#include <vector>

namespace DiceRoller {
	static constexpr int cDiceSides = 6;

	static inline int roll() { return rand()%cDiceSides + 1; }
	static bool rollGetSuccess(int BTN) { return roll() >= BTN; }

	static std::vector<int> roll(int number);
	static int rollGetSuccess(int BTN, int number);
	
};

