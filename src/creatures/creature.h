#pragma once

#include "wound.h"
#include "types.h"
#include "../items/types.h"
#include "../items/weapon.h"
#include "../items/armor.h"

#include <vector>
#include <map>

static constexpr int cBaseBTN = 3;

class Creature
{
public:
	Creature();
	
	int getBrawn() const { return m_brawn; }
	int getAgility() const { return m_agility; }
	int getCunning() const { return m_cunning; }
	int getPerception() const { return m_perception; }
	int getWill() const { return m_will; }
	
	int getGrit() const { return (m_brawn + m_will)/2; }
	int getKeen() const { return (m_cunning + m_perception)/2; }
	int getReflex() const { return (m_agility + m_cunning)/2; }
	int getSpeed() const { return (m_agility + m_brawn)/2; }

	int getBTN() { return m_BTN; }
	void setBTN(int BTN) { m_BTN = BTN; }

	Weapon* getPrimaryWeapon();
	void setWeapon(int id) { m_primaryWeaponId = id; }

	void setName(const std::string& name) { m_name = name; }
	std::string getName() const { return m_name; }

	int getProficiency(eWeaponTypes type) { return m_proficiencies[type]; }

	void inflictWound(Wound* wound, bool manueverFirst = false);

	int getSuccessRate() const;

	void applyArmor(Armor* armor);
	ArmorSegment getArmorAtPart(eBodyParts part);

	// for current weapon
	int getCombatPool() const { return m_combatPool; }
	void resetCombatPool();
	void reduceCombatPool(int num) { assert(num>=0); m_combatPool -= num; }
	void reduceOffenseDie(int num) { m_currentOffense.dice -= num; }

	// AI functions
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
	
protected:
	std::vector<eHitLocations> m_hitLocations;
	std::map<eBodyParts, ArmorSegment> m_armorValues;
	std::vector<Wound*> m_wounds;
	std::vector<Armor*> m_armor;

	std::string m_name;

	Offense m_currentOffense;
	Defense m_currentDefense;

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
