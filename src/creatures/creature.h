#pragma once

#include "wound.h"
#include "types.h"
#include "../items/types.h"
#include "../items/weapon.h"
#include "../items/armor.h"

#include <vector>
#include <map>
#include <iostream>

static constexpr int cBaseBTN = 3;
//different based on race
static constexpr int cBaseBloodLoss = 8;

enum class eCreatureType {
	Human
};

enum class eCreatureState {
	Idle,
	InCombat,
	Dead,
	Unconcious
};

enum class eCreatureStance {
	Standing,
	Prone
};

class Creature
{
public:
	Creature();

	virtual eCreatureType getCreatureType() = 0;
	
	int getBrawn() const { return m_brawn; }
	int getAgility() const { return m_agility; }
	int getCunning() const { return m_cunning; }
	int getPerception() const { return m_perception; }
	int getWill() const { return m_will; }
	
	int getGrit() const { return (m_brawn + m_will)/2; }
	int getKeen() const { return (m_cunning + m_perception)/2; }
	int getReflex() const { return (m_agility + m_cunning)/2; }
	int getSpeed() const { return (m_agility + m_brawn)/2; }

	int getBTN() const { return m_BTN; }
	int getBloodLoss() const { return m_bloodLoss; }
	void setBTN(int BTN) { m_BTN = BTN; }

	const Weapon* getPrimaryWeapon() const;
	std::vector<const Armor*> getArmor() const;
	void setWeapon(int id);

	void setName(const std::string& name) { m_name = name; }
	std::string getName() const { return m_name; }

	int getProficiency(eWeaponTypes type) { return m_proficiencies[type]; }

	void inflictWound(Wound* wound, bool manueverFirst = false);

	int getSuccessRate() const;

	void equipArmor(int id);
	ArmorSegment getArmorAtPart(eBodyParts part);

	// for current weapon
	int getCombatPool() const { return m_combatPool; }
	void resetCombatPool();
	void reduceCombatPool(int num) { assert(num>=0); m_combatPool -= num; }
	void setBonusDice(int num) { m_bonusDice = num; }
	void addAndResetBonusDice();
	void reduceOffenseDie(int num) { m_currentOffense.dice -= num; m_currentOffense.dice = std::max(0, m_currentOffense.dice); }
	const std::vector<eHitLocations>& getHitLocations() const { return m_hitLocations; }

	// AI functions
	//move these to ai combat controller
	virtual bool isPlayer() { return false; }
	void doOffense(const Creature* target, int reachCost, bool allin = false);

	void doDefense(const Creature* attacker, bool isLastTempo);

	void doStolenInitiative(const Creature* defender);

	bool stealInitiative(const Creature* attacker, int& outDie);

	eInitiativeRoll doInitiative();

	struct Offense {
		eOffensiveManuevers manuever;
		int dice;
		eHitLocations target;
		Component* component = nullptr;
	};
	//this is reused in stealing initiative to hold die allocated to stealing
	struct Defense {
		eDefensiveManuevers manuever;
		int dice;
	};

	Offense getQueuedOffense() const { return m_currentOffense; }
	Defense getQueuedDefense() const { return m_currentDefense; }

	eCreatureState getCreatureState() { return m_currentState; }
	
protected:
	eCreatureState m_currentState;
	
	void clearArmor();
	void applyArmor();
	
	std::vector<eHitLocations> m_hitLocations;
	std::map<eBodyParts, ArmorSegment> m_armorValues;
	std::vector<Wound*> m_wounds;
	std::vector<int> m_armor;

	std::string m_name;

	Offense m_currentOffense;
	Defense m_currentDefense;

	//index
	int m_primaryWeaponId;
	
	int m_bloodLoss;
	int m_BTN;
	float m_AP;

	bool m_isPlayer;

	//stats
	int m_brawn;
	int m_agility;
	int m_cunning;
	int m_perception;
	int m_will;

	int m_combatPool;
	int m_bonusDice;

	std::map<eWeaponTypes, int> m_proficiencies;
	
};
