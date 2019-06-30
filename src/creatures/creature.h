#pragma once

#include "wound.h"
#include "types.h"
#include "../weapons/types.h"
#include "../weapons/weapon.h"

#include <vector>
#include <map>

static constexpr int cBaseBTN = 3;

class Creature
{
public:
	Creature();
	
	int getBrawn() { return m_brawn; }
	int getAgility() { return m_agility; }
	int getCunning() { return m_cunning; }
	int getPerception() { return m_perception; }
	int getWill() { return m_will; }
	
	int getGrit() { return (m_brawn + m_will)/2; }
	int getKeen() { return (m_cunning + m_perception)/2; }
	int getReflex() { return (m_agility + m_cunning)/2; }
	int getSpeed() { return (m_agility + m_brawn)/2; }

	int getBTN() { return m_BTN; }
	void setBTN(int BTN) { m_BTN = BTN; }

	Weapon* getPrimaryWeapon();
	void setWeapon(int id) { m_primaryWeaponId = id; }

	void setName(const std::string& name) { m_name = name; }
	std::string getName() { return m_name; }

	int getProficiency(eWeaponTypes type) { return m_proficiencies[type]; }

	void inflictWound(Wound* wound);

	// for current weapon
	int getCombatPool() { return m_combatPool; }
	void resetCombatPool();
	void reduceCombatPool(int num) { m_combatPool -= num; }

	// AI functions
	void doOffense(Creature* target, int reachCost,
				   eOffensiveManuevers& outOffense, int& outDice,
				   eHitLocations& outLocation, Component*& outComponent);

	void doDefense(bool isLastTempo, int diceAllocated, eDefensiveManuevers& outDefense, int& outDice);

	
protected:
	std::vector<eHitLocations> m_hitLocations;
	std::map<eBodyParts, int> m_armor;
	std::vector<Wound*> m_wounds;

	std::string m_name;

	//index
	int m_primaryWeaponId;
	
	int m_bloodLoss;
	int m_BTN;

	bool m_isPlayer;

	//stats
	int m_brawn;
	int m_agility;
	int m_cunning;
	int m_perception;
	int m_will;

	int m_combatPool;

	std::map<eWeaponTypes, int> m_proficiencies;
	
};
