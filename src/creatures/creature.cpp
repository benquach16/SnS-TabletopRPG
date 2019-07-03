#include "creature.h"
#include "../3rdparty/random.hpp"

using namespace std;

Creature::Creature() : m_BTN(cBaseBTN), m_brawn(1), m_agility(1),
					   m_cunning(1), m_perception(1), m_will(1), m_primaryWeaponId(0), m_combatPool(0)
{
	
}

Weapon* Creature::getPrimaryWeapon()
{
	return WeaponTable::getSingleton()->get(m_primaryWeaponId);
}

void Creature::inflictWound(Wound* wound)
{
	m_BTN = max(m_BTN, wound->getBTN());
	m_combatPool -= wound->getImpact();
	m_wounds.push_back(wound);
	m_bloodLoss;
}

void Creature::resetCombatPool()
{
	//carryover impact damage across tempos
	Weapon* weapon = getPrimaryWeapon();
	int carry = m_combatPool;
	carry = min(0, carry);
	m_combatPool = getProficiency(weapon->getType()) + getReflex() + carry;
}

void Creature::doOffense(Creature* target, int reachCost)
{
	Weapon* weapon = getPrimaryWeapon();
	//we shouldn't be able to pgo below 0 with this
	m_combatPool -= reachCost;

	m_currentOffense.manuever = eOffensiveManuevers::Thrust;
	m_currentOffense.component = weapon->getBestAttack();
	if(m_currentOffense.component->getAttack() == eAttacks::Swing) {
		m_currentOffense.manuever = eOffensiveManuevers::Swing;
	}
	m_currentOffense.target = eHitLocations::Head;
	int dice = m_combatPool / 2 + effolkronium::random_static::get(0, m_combatPool/3)
		- effolkronium::random_static::get(0, m_combatPool/3);

	//bound
	dice = max(0, dice);
	dice = min(dice, m_combatPool);
	m_currentOffense.dice = dice;
}


void Creature::doDefense(bool isLastTempo, int diceAllocated)
{
	m_currentDefense.manuever = eDefensiveManuevers::Parry;
	if(isLastTempo == true) {
		//use all dice because we're going to refresh anyway
		m_currentDefense.dice = m_combatPool;
		return;
	}
	int dice = std::min(diceAllocated + effolkronium::random_static::get(0, m_combatPool/3)
					   - effolkronium::random_static::get(0, m_combatPool/3)
					   , m_combatPool);
	dice = max(0, dice);
	m_currentDefense.dice = dice;
}

eInitiativeRoll Creature::doInitiative()
{
	//do random for now
	//this should be based on other creatures weapon length and armor and stuff
	if(effolkronium::random_static::get(0, 1) == 1){
		return eInitiativeRoll::Attack;
	}
	return eInitiativeRoll::Defend;
}
							 
