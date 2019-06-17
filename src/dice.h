#pragma once

#include <stdlib.h>

class Dice {
	static constexpr int diceSides = 6;
public:
	int roll() { return rand()%diceSides + 1; }
};
