#pragma once

#include <stdlib.h>
#include <random>
#include <vector>

#include "3rdparty/random.hpp"

namespace DiceRoller {
	constexpr int cDiceSides = 6;

	inline int roll() {
		return effolkronium::random_static::get(1,cDiceSides);
	}
	//bool rollGetSuccess(int BTN) { return roll() >= BTN; }

	std::vector<int> roll(int number);
	int rollGetSuccess(int BTN, int number);
	
};

