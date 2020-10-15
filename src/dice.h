#pragma once

#include <random>
#include <stdlib.h>
#include <vector>

#include "3rdparty/random.hpp"

namespace DiceRoller {
constexpr int cDiceSides = 10;

inline int roll() { return effolkronium::random_static::get(1, cDiceSides); }
inline int rollSides(int sides) { return effolkronium::random_static::get(1, sides); }
// bool rollGetSuccess(int BTN) { return roll() >= BTN; }

std::vector<int> roll(int number);
int rollGetSuccess(int BTN, int number);

}
