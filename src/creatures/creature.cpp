#include <iostream>

#include "creature.h"
#include "../3rdparty/random.hpp"
#include "../dice.h"

using namespace std;

Creature::Creature() : m_BTN(cBaseBTN), m_brawn(1), m_agility(1),
					   m_cunning(1), m_perception(1), m_will(1), m_primaryWeaponId(0), m_combatPool(0),
					   m_currentState(eCreatureState::Idle), m_bonusDice(0), m_bloodLoss(0)
{
	
}

Weapon* Creature::getPrimaryWeapon() const
{
	return WeaponTable::getSingleton()->get(m_primaryWeaponId);
}

std::vector<const Armor*> Creature::getArmor() const
{
	std::vector<const Armor*> ret;
	for (int i = 0; i < m_armor.size(); ++i)
	{
		ret.push_back(ArmorTable::getSingleton()->get(m_armor[i]));
	}
	return ret;
}

void Creature::inflictWound(Wound* wound, bool manueverFirst)
{
	m_BTN = max(m_BTN, wound->getBTN());
	if(manueverFirst == true) {
		if(wound->getImpact() > m_currentOffense.dice) {
			int diff = wound->getImpact() - m_currentOffense.dice;
			m_currentOffense.dice = 0;
			m_combatPool -= diff;
		}
		else {			
			m_currentOffense.dice -= wound->getImpact();
		}
	}
	else {
		m_combatPool -= wound->getImpact();
	}
	m_wounds.push_back(wound);

	if(wound->causesDeath() == true) {
		m_currentState = eCreatureState::Dead;
	}
	if(wound->immediateKO() == true) {
		m_currentState = eCreatureState::Unconcious;
	}
	set<eEffects> effects = wound->getEffects();
	auto BL1 = effects.find(eEffects::BL1);
	auto BL2 = effects.find(eEffects::BL2);
	auto BL3 = effects.find(eEffects::BL3);
	if(BL1 != effects.end()) {
		m_bloodLoss++;
	}
	if(BL2 != effects.end()) {
		m_bloodLoss+=2;
	}
	if(BL3 != effects.end()) {
		m_bloodLoss+=3;
	}

	if(m_bloodLoss >= cBaseBloodLoss) {
		m_currentState = eCreatureState::Dead;
	}
}

int Creature::getSuccessRate() const {
	float sides = static_cast<float>(DiceRoller::cDiceSides);
	float btn = static_cast<float>(DiceRoller::cDiceSides - m_BTN) + 1.f;

	float val = btn / sides;
	val *= 100;
	return static_cast<int>(val);
}

ArmorSegment Creature::getArmorAtPart(eBodyParts part)
{
	return m_armorValues[part];
}

void Creature::equipArmor(int id)
{
	const Armor* armor = ArmorTable::getSingleton()->get(id);
	assert(armor != nullptr);

	//make sure it doesnt overlap with another armor
	for(int i : m_armor) {
		const Armor* equippedArmor = ArmorTable::getSingleton()->get(i);
		assert(armor->isOverlapping(equippedArmor) == false);
	}

	m_armor.push_back(id);
	applyArmor();
}

void Creature::resetCombatPool()
{
	//carryover impact damage across tempos
	Weapon* weapon = getPrimaryWeapon();
	int carry = m_combatPool;
	carry = min(0, carry);
	m_combatPool = getProficiency(weapon->getType()) + getReflex() + carry;
}

void Creature::addAndResetBonusDice()
{
	m_currentOffense.dice += m_bonusDice;
	m_bonusDice = 0;
}

void Creature::doOffense(const Creature* target, bool allin)
{
	Weapon* weapon = getPrimaryWeapon();

	m_currentOffense.manuever = eOffensiveManuevers::Thrust;
	m_currentOffense.component = weapon->getBestAttack();
	if(m_currentOffense.component->getAttack() == eAttacks::Swing) {
		m_currentOffense.manuever = eOffensiveManuevers::Swing;
	}

	//replace me
	m_currentOffense.target = target->getHitLocations()[rand()%target->getHitLocations().size() - 1];
	int dice = m_combatPool / 2 + effolkronium::random_static::get(0, m_combatPool/3)
		- effolkronium::random_static::get(0, m_combatPool/3);

	//bound
	dice = max(0, dice);
	dice = min(dice, m_combatPool);
	//never issue 0 dice for attack
	if(m_combatPool > 0 && dice == 0) {
		dice = 1;
	}
	if(allin == true) {
		m_currentOffense.dice = m_combatPool;
	} else {
		m_currentOffense.dice = dice;
	}
}


void Creature::doDefense(const Creature* attacker, bool isLastTempo)
{
	int diceAllocated = attacker->getQueuedOffense().dice;

	m_currentDefense.manuever = eDefensiveManuevers::Parry;

	int stealDie = 0;
	if(stealInitiative(attacker, stealDie) == true) {
		m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
		m_currentDefense.dice = stealDie;
		return;
	}
	if(isLastTempo == true) {
		//use all dice because we're going to refresh anyway
		m_currentDefense.dice = m_combatPool;
		m_currentDefense.dice = max(m_currentDefense.dice, 0);
		return;
	}
	int dice = std::min(diceAllocated + effolkronium::random_static::get(0, m_combatPool/3)
					   - effolkronium::random_static::get(0, m_combatPool/3)
					   , m_combatPool);
	dice = min(m_combatPool, dice);
	dice = max(dice, 0);
	m_currentDefense.dice = dice;
}

bool Creature::stealInitiative(const Creature* attacker, int& outDie)
{
	int diceAllocated = attacker->getQueuedOffense().dice;

	int combatPool = attacker->getCombatPool() + attacker->getSpeed();

	constexpr int bufferDie = 2;
	if((combatPool * 1.5) + bufferDie < m_combatPool + getSpeed()) {
		int diff = (getSpeed() - attacker->getSpeed()) * 1.5;
		int dice = diff + bufferDie;
		if(m_combatPool > dice) {
			outDie = dice;
			return true;
		}

	}
	return false;
}

void Creature::doStolenInitiative(const Creature* defender)
{
	m_currentDefense.manuever = eDefensiveManuevers::StealInitiative;
	Defense defend = defender->getQueuedDefense();
	m_currentDefense.dice = min(m_combatPool, defend.dice);
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

void Creature::clearArmor()
{
	m_AP = 0;
	for(auto it : m_armorValues) {
		it.second.AV = 0;
		it.second.isMetal = false;
		it.second.isRigid = false;
		it.second.type = eArmorTypes::None;
	}
}

void Creature::applyArmor()
{
	clearArmor();
	for(int i : m_armor) {
		const Armor* armor = ArmorTable::getSingleton()->get(i);
		for(auto it : armor->getCoverage()) {
			m_AP += armor->getAP();
			m_armorValues[it].AV = max(m_armorValues[it].AV, armor->getAV());
			m_armorValues[it].isMetal = m_armorValues[it].isMetal || armor->isMetal();
			m_armorValues[it].isRigid = m_armorValues[it].isRigid || armor->isRigid();
		}
	}
}
							 
