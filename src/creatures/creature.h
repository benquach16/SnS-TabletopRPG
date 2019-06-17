#pragma once

#include "wound.h"
#include "types.h"
#include "../weapons/types.h"

#include <vector>
#include <map>

static constexpr int cBaseBTN = 3;

class Creature
{
public:
	int getBrawn() { return m_brawn; }
	int getAgility() { return m_agility; }
	int getCunning() { return m_cunning; }
	int getPerception() { return m_perception; }
	int getWill() { return m_will; }
	
	int getGrit() { return (m_brawn + m_will)/2; }
	int getKeen() { return (m_cunning + m_perception)/2; }
	int getReflex() { return (m_agility + m_cunning)/2; }
	int getSpeed() { return (m_agility + m_brawn)/2; }

protected:
	std::vector<eHitLocations> m_hitLocations;
	std::vector<Wound> m_wounds;
	int m_bloodLoss;
	int m_BTN;

	//stats
	int m_brawn;
	int m_agility;
	int m_cunning;
	int m_perception;
	int m_will;

	std::map<eWeaponTypes, int> m_proficiencies;
	
};
