#pragma once

#include "wound.h"

#include <vector>

static constexpr int cBaseBTN = 3;

class Creature
{
public:
private:
	std::vector<Wound> m_wounds;
	int m_bloodLoss;
	int m_BTN;

	//stats
	int m_brawn;
	int m_agility;
	int m_cunning;
	int m_perception;
};
