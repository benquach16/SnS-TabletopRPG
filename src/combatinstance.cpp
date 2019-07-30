//I have lots of redundant code, refactor me!

#include <iostream>
#include <assert.h>

#include "creatures/utils.h"
#include "items/utils.h"
#include "combatinstance.h"
#include "game.h"
#include "creatures/player.h"

using namespace std;

constexpr int cMinFatigueTempos = 3;

CombatInstance::CombatInstance(): m_initiative(eInitiative::Side1), m_side1(nullptr), m_side2(nullptr),
								  m_currentTempo(eTempo::First), m_currentState(eCombatState::Uninitialized), 
								  m_currentReach(eLength::Hand), m_dualWhiteTimes(0), m_dualRedThrow(false),
								  m_numTempos(0)
{
}

void CombatInstance::setSides(Creature*& attacker, Creature*& defender)
{
	if(m_initiative == eInitiative::Side1) {
		attacker = m_side1;
		defender = m_side2;
	}
	else if(m_initiative == eInitiative::Side2) {
		attacker = m_side2;
		defender = m_side1;
	}
	assert(attacker != nullptr);
	assert(defender != nullptr);
}

void CombatInstance::initCombat(Creature* side1, Creature* side2)
{
	assert(side1 != nullptr);
	assert(side2 != nullptr);
	assert(m_currentState == eCombatState::Uninitialized);
	
	m_side1 = side1;
	m_side1->resetCombatPool();
	m_side2 = side2;
	m_side2->resetCombatPool();

	m_side1->setInCombat();
	m_side2->setInCombat();

	m_side1->setStand();
	m_side2->setStand();

	m_currentTempo = eTempo::First;
	m_initiative = eInitiative::Side1;
	m_currentState = eCombatState::Initialized;
	m_numTempos = 0;
}

void CombatInstance::forceInitiative(eInitiative initiative)
{
	m_initiative = initiative;
	m_currentState = eCombatState::Offense;
}	

void CombatInstance::doInitialization()
{
	assert(m_side1 != nullptr);
	assert(m_side2 != nullptr);
	assert(m_currentState == eCombatState::Initialized);
	
	writeMessage("Combat between " + m_side1->getName() + " and " + m_side2->getName(), Log::eMessageTypes::Announcement);
	writeMessage(m_side1->getName() + " is using " + m_side1->getPrimaryWeapon()->getName() + " and " +
			  m_side2->getName() + " is using " + m_side2->getPrimaryWeapon()->getName());

	//ugly implicit casting
	m_currentReach = max(m_side1->getPrimaryWeapon()->getLength(), m_side2->getPrimaryWeapon()->getLength());
	m_dualWhiteTimes = 0;
	m_currentState = eCombatState::RollInitiative;
}

void CombatInstance::doRollInitiative()
{
	if(m_side1->isPlayer() == true) {
		Player* player = static_cast<Player*>(m_side1);
		if(player->pollForInitiative() == false) {
			m_currentState = eCombatState::RollInitiative;
			return;			
		}
	}
	
	//get initiative rolls from both sides to determine roles.
	eInitiativeRoll side1 = m_side1->doInitiative(m_side2);
	eInitiativeRoll side2 = m_side2->doInitiative(m_side1);
	if(m_side1->isPlayer() == true) {
		Player* player = static_cast<Player*>(m_side1);
		side1 = player->getInitiative();
	}
	
	if(side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Defend) {
		//repeat
		writeMessage("Both sides chose to defend, deciding initiative again");
		if(m_dualWhiteTimes > 1) {
			writeMessage("Defense chosen too many times, initiative going to willpower contest");
			int side1Successes = DiceRoller::rollGetSuccess(m_side1->getBTN(), m_side1->getWill());
			int side2Successes = DiceRoller::rollGetSuccess(m_side2->getBTN(), m_side2->getWill());

			m_initiative = side1Successes < side2Successes ? eInitiative::Side1 : eInitiative::Side2;

			if(m_initiative == eInitiative::Side1) {
				writeMessage(m_side1->getName() + " takes initiative");
			} else {
				writeMessage(m_side2->getName() + " takes initiative");
			}
			m_currentState = eCombatState::Offense;
			return;
		}
		m_dualWhiteTimes++;
		m_currentState = eCombatState::ResetState;
		return;
	} else if(side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Defend) {
		writeMessage(m_side1->getName() + " chose to attack and " + m_side2->getName() + " is defending");
		m_initiative = eInitiative::Side1;
		m_currentState = eCombatState::Offense;
		return;
	} else if(side1 == eInitiativeRoll::Defend && side2 == eInitiativeRoll::Attack) {
		writeMessage(m_side2->getName() + " chose to attack and " + m_side1->getName() + " is defending");
		m_initiative = eInitiative::Side2;
		m_currentState = eCombatState::Offense;		
		return;
	} else if(side1 == eInitiativeRoll::Attack && side2 == eInitiativeRoll::Attack) {
		writeMessage("Both sides chose to attack, no defense can be done by either side.");
		// no defense here!
		// hopefully you don't die horribly
		// roll speed to determine who goes first
		int side1Successes = DiceRoller::rollGetSuccess(m_side1->getBTN(), m_side1->getSpeed());
		int side2Successes = DiceRoller::rollGetSuccess(m_side2->getBTN(), m_side2->getSpeed());

		m_initiative = side1Successes < side2Successes ? eInitiative::Side1 : eInitiative::Side2;
		
		if(m_initiative == eInitiative::Side1) {
			writeMessage(m_side1->getName() + " declares their attack first");
		} else {
			writeMessage(m_side2->getName() + " declares their attack first");
		}
		m_dualRedThrow = true;
		m_currentState = eCombatState::DualOffense1;
		return;
	}

	m_currentState = eCombatState::RollInitiative;
}

void CombatInstance::doPreexchangeActions()
{
	m_currentState = eCombatState::Offense;
}

void CombatInstance::doResetState()
{
	m_currentState = eCombatState::RollInitiative;
}

bool CombatInstance::doOffense()
{
	//get offensive manuever and dice from side 1
	//then get defensive manuever and dice from side 2
	//then resolve		
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	
	if(attacker->getCombatPool() <= 0 && defender->getCombatPool() > 0) {
		writeMessage(attacker->getName() + " has no more action points! Initiative swaps to defender");
		switchInitiative();
		setSides(attacker, defender);
	}
	if(attacker->getCombatPool() <= 0 && defender->getCombatPool() <= 0) {
		writeMessage("Neither side has any action points left, starting new exchange and resetting combat pools");
		m_currentTempo = eTempo::First;
		attacker->resetCombatPool();
		defender->resetCombatPool();
		attacker->clearCreatureManuevers();
		defender->clearCreatureManuevers();
	}
	
	const Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	const Weapon* defenseWeapon = defender->getPrimaryWeapon();
	
	int offenseCombatPool = attacker->getCombatPool();

	int reachCost = static_cast<int>(m_currentReach) - static_cast<int>(offenseWeapon->getLength());
	reachCost = abs(reachCost);
	if(attacker->isPlayer() == true)
	{
		//wait until we get input from player
		Player* player = static_cast<Player*>(attacker);
		if(player->pollForOffense() == false) {
			m_currentState = eCombatState::Offense;
			return false;
		} 
	}
	else {
		attacker->doOffense(defender, reachCost, false, m_dualRedThrow);
	}
	if(reachCost != 0) {
		writeMessage("Weapon length difference causes reach cost of " + to_string(reachCost) +
					 " action points", Log::eMessageTypes::Announcement);
		attacker->reduceOffenseDie(reachCost);
		attacker->reduceCombatPool(min(reachCost, offenseCombatPool));
	}
	Offense attack = attacker->getQueuedOffense();
	
	assert(attack.component != nullptr);
	assert(attack.dice <= offenseCombatPool);
	assert(attack.dice >= 0);
	attacker->reduceCombatPool(attack.dice);

	attacker->addAndResetBonusDice();
	
	writeMessage(attacker->getName() + " " + offensiveManueverToString(attack.manuever) + "s with " +
				 offenseWeapon->getName() + " at " + hitLocationToString(attack.target) + " using " +
				 attack.component->getName() + " with " +
				 to_string(attacker->getQueuedOffense().dice) + " action points");
	
	m_currentState = eCombatState::Defense;
	return true;
}

void CombatInstance::doDualOffense1()
{
	//both sides rolled red
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	//person who rolled better on speed goes second
	//allow stealing of initiative here
	if(doOffense() == false) {
		m_currentState = eCombatState::DualOffense1;
		return;
	}

	Defense defense = attacker->getQueuedDefense();
	if(defense.manuever == eDefensiveManuevers::StealInitiative) {
		writeMessage(attacker->getName() + " allocates " + to_string(defense.dice) + " to steal initiative");
		m_currentState = eCombatState::DualOffenseStealInitiative;
		return;
	}
	switchInitiative();
	
	m_currentState = eCombatState::DualOffense2;
}

void CombatInstance::doDualOffenseStealInitiative()
{
	Creature *attacker = nullptr;
	Creature *defender = nullptr;
	setSides(attacker, defender);

	const Weapon* defenseWeapon = defender->getPrimaryWeapon();

	int reachCost = static_cast<int>(m_currentReach) - static_cast<int>(defenseWeapon->getLength());
	reachCost = abs(reachCost);
	if (defender->isPlayer() == true) {
		//wait until player inputs
		Player *player = static_cast<Player *>(defender);

		//two staged ui, piggyback off of existing code
		//this may cause isseus so add another ui state if it does
		if(player->pollForDefense() == true) {
			player->reduceCombatPool(player->getQueuedDefense().dice);
			cout << "Two stage UI" << endl;
		}
		if (player->pollForOffense() == false) {
			m_currentState = eCombatState::DualOffenseStealInitiative;
			return;
		}
		
	}
	else {
		//confusing nomenclature
		defender->doOffense(attacker, reachCost, false, true);		
	}
	
	Offense offense = defender->getQueuedOffense();
	assert(offense.dice <= defender->getCombatPool());
	defender->reduceCombatPool(offense.dice);

	if(reachCost != 0) {
		writeMessage("Weapon length difference causes reach cost of " + to_string(reachCost) +
					 " action points", Log::eMessageTypes::Announcement);
		defender->reduceOffenseDie(reachCost);
		defender->reduceCombatPool(min(reachCost, defender->getCombatPool()));
	}
	
	Defense defense = defender->getQueuedDefense();
	writeMessage(defender->getName() + " allocates " + to_string(defense.dice) + " for initiative");
	writeMessage(defender->getName() + " " + offensiveManueverToString(defender->getQueuedOffense().manuever) + "s with " +
				 defender->getPrimaryWeapon()->getName() + " at " + hitLocationToString(defender->getQueuedOffense().target) + " using " +
				 defender->getQueuedOffense().component->getName() + " with " +
				 to_string(defender->getQueuedOffense().dice) + " action points");
	switchInitiative();
	m_currentState = eCombatState::Resolution;
}

void CombatInstance::doDualOffense2()
{
	//both sides rolled red
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	if(doOffense() == false) {
		m_currentState = eCombatState::DualOffense2;
		return;
	}
	Defense defense = attacker->getQueuedDefense();
	if(defense.manuever == eDefensiveManuevers::StealInitiative) {
		writeMessage(attacker->getName() + " allocates " + to_string(defense.dice) + " for initiative");
		//attacker->reduceCombatPool(defense.dice);
	    //defender already declared initiative roll, just go to resolution
		if(defender->getQueuedDefense().manuever == eDefensiveManuevers::StealInitiative) {
			m_currentState = eCombatState::Resolution;
			return;
		}
		m_currentState = eCombatState::DualOffenseSecondInitiative;
		return;
	}
	
	m_currentState = eCombatState::DualOffenseResolve;
}

void CombatInstance::doDualOffenseSecondInitiative()
{
	//both sides rolled red
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	//defender is person who went first, they need to respond with dice allocation
	if (defender->isPlayer() == true) {
		//wait until player inputs
		Player *player = static_cast<Player *>(defender);
		if (player->pollForDefense() == false) {
			m_currentState = eCombatState::DualOffenseSecondInitiative;
			return;
		}
	}
	else {
		//confusing nomenclature
		defender->doStolenInitiative(attacker, true);
		//hacky way because of how dice is removed for now
		Defense defense = defender->getQueuedDefense();
		defender->reduceCombatPool(defense.dice);
	}

	Defense defense = defender->getQueuedDefense();
	writeMessage(defender->getName() + " allocates " + to_string(defense.dice) + " action points for initiative");
	m_currentState = eCombatState::Resolution;
}

void CombatInstance::doDefense()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	const Weapon* defenseWeapon = defender->getPrimaryWeapon();

	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();
	if(defender->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(defender);
		if(player->pollForDefense() == false) {
			m_currentState = eCombatState::Defense;
			return;
		}

	}
	else {
		defender->doDefense(attacker, m_currentTempo == eTempo::Second);		
	}
	
	Defense defend = defender->getQueuedDefense();
	assert(defend.dice <= defenseCombatPool);
	assert(defend.dice >= 0);
	defender->reduceCombatPool(defend.dice);
	if(defend.manuever == eDefensiveManuevers::StealInitiative) {
		//need both sides to attempt to allocate dice
		m_currentState = eCombatState::StealInitiative;
		return;
	}
	if(defend.manuever == eDefensiveManuevers::ParryLinked) {
		writeMessage(defender->getName() + " performs a linked parry with " + defenseWeapon->getName() +
					 " using " + to_string(defend.dice) + " action points");
		m_currentState = eCombatState::ParryLinked;
		return;
	}
	
	writeMessage(defender->getName() + " defends with " + defenseWeapon->getName() +
				 " using " + to_string(defend.dice) + " action points");
	
	m_currentState = eCombatState::Resolution;
}

void CombatInstance::doParryLinked()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	
	const Weapon* defenseWeapon = defender->getPrimaryWeapon();

	int defenseCombatPool = defender->getProficiency(defenseWeapon->getType()) + defender->getReflex();
	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(m_currentReach);
	reachCost = abs(reachCost);
	if(defender->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(defender);
		if(player->pollForOffense() == false) {
			m_currentState = eCombatState::ParryLinked;
			return;
		}

	}
	else {
		defender->doOffense(attacker, reachCost, m_currentTempo == eTempo::Second);		
	}
	Offense offense = defender->getQueuedOffense();

	writeMessage(defender->getName() + " links defense to counter with " + offense.component->getName());

	m_currentState = eCombatState::Resolution;
}

void CombatInstance::doStealInitiative()
{
	//defender inputs offense and dice to steal initiative
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	//then input manuever
	Defense defend = defender->getQueuedDefense();
	const Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	const Weapon* defenseWeapon = defender->getPrimaryWeapon();
	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(m_currentReach);
	reachCost = abs(reachCost);
	//do dice to steal initiative first
	//this polls for offense since the initiative steal is stored inside the defense struct
	if(defender->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(defender);
		if(player->pollForOffense() == false) {
			m_currentState = eCombatState::StealInitiative;
			return;
		}

	}
	else {
		defender->doOffense(attacker, reachCost, true);
	}
	
	writeMessage(defender->getName() + " attempts to steal intiative using " + to_string(defend.dice) +
		" action points!");

	m_currentState = eCombatState::StolenOffense;
}

void CombatInstance::doStolenOffense()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	if(attacker->isPlayer() == true) {
		//wait until player inputs
		Player* player = static_cast<Player*>(attacker);
		if(player->pollForDefense() == false) {
			m_currentState = eCombatState::StolenOffense;
			return;
		}
	}
	else {
 		attacker->doStolenInitiative(defender);
	}
	
	assert(attacker->getQueuedDefense().dice <= attacker->getCombatPool());
	attacker->reduceCombatPool(attacker->getQueuedDefense().dice);
	
	writeMessage(attacker->getName() + " allocates " + to_string(attacker->getQueuedDefense().dice) +
				 " action points to contest initiative steal");
	const Weapon* offenseWeapon = attacker->getPrimaryWeapon();
	const Weapon* defenseWeapon = defender->getPrimaryWeapon();	
	int reachCost = static_cast<int>(defenseWeapon->getLength()) - static_cast<int>(m_currentReach);
	reachCost = abs(reachCost);
	if(reachCost != 0) {
		
		writeMessage("Weapon length difference causes reach cost of " + to_string(reachCost) +
					 " action points", Log::eMessageTypes::Announcement);
		defender->reduceOffenseDie(reachCost);
		defender->reduceCombatPool(reachCost);
		defender->reduceCombatPool(defender->getQueuedOffense().dice);
	}
	
	writeMessage(defender->getName() + " " + offensiveManueverToString(defender->getQueuedOffense().manuever) + "s with " +
				 defender->getPrimaryWeapon()->getName() + " at " + hitLocationToString(defender->getQueuedOffense().target) + " using " +
				 defender->getQueuedOffense().component->getName() + " with " +
				 to_string(defender->getQueuedOffense().dice) + " action points");	
	m_currentState = eCombatState::Resolution;
}

void CombatInstance::doResolution2()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	//resolve all of the effects
	
	m_dualRedThrow = false;
	switchTempo();
	m_currentState = eCombatState::Offense;
}

void CombatInstance::doResolution()
{
	cout << "Resolving combat" << endl;
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	Offense attack = attacker->getQueuedOffense();
	Defense defend = defender->getQueuedDefense();

	//determine who was originally attacking
	if(defend.manuever == eDefensiveManuevers::StealInitiative) {
		cout << "Dual resolution" << endl;
		//original attacker gets advantage
		int side1BTN = (m_side1 == attacker && m_dualRedThrow == false) ? m_side1->getAdvantagedBTN() : m_side1->getBTN();

		int side2BTN = (m_side2 == attacker && m_dualRedThrow == false) ? m_side2->getAdvantagedBTN() : m_side2->getBTN();

		//special case for thrust manuever, get an extra die
		int side1Dice = (m_side1->getQueuedOffense().manuever == eOffensiveManuevers::Thrust) ?
			m_side1->getQueuedDefense().dice + 1 : m_side1->getQueuedDefense().dice;
		int side2Dice = (m_side2->getQueuedOffense().manuever == eOffensiveManuevers::Thrust) ?
			m_side2->getQueuedDefense().dice + 1 : m_side2->getQueuedDefense().dice;

		int side1InitiativeSuccesses =
			DiceRoller::rollGetSuccess(side1BTN, side1Dice + m_side1->getSpeed());
		int side2InitiativeSuccesses =
			DiceRoller::rollGetSuccess(side2BTN, side2Dice + m_side2->getSpeed());
		
		Creature* originalAttackerPtr = attacker;
		if(side1InitiativeSuccesses > side2InitiativeSuccesses) {
			m_initiative = eInitiative::Side1;
		} else {
			m_initiative = eInitiative::Side2;
		}
		setSides(attacker, defender);
		writeMessage(attacker->getName() + " has taken the initiative!");

		int attackerSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(),
													   attacker->getQueuedOffense().dice);
		if(attackerSuccesses > 0) {
			if(inflictWound(attackerSuccesses, attacker->getQueuedOffense(), defender, true) == true) {
				m_currentState = eCombatState::FinishedCombat;
				return;
			}
		} else {
			writeMessage(attacker->getName() + " had no successes");			
		}

		if(defender->getQueuedOffense().dice == 0) {
			//if the attack wiped out their combat pool, do nothing
			writeMessage(defender->getName() + " had their action points eliminated by impact, their attack is dropped.");
			m_currentState = eCombatState::Offense;
		}
		else {
			int defendSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(),
															 defender->getQueuedOffense().dice);
			if(defendSuccesses > 0) {

				if(inflictWound(defendSuccesses, defender->getQueuedOffense(), attacker) == true) {
					m_currentState = eCombatState::FinishedCombat;
					return;
				}
			} else {
				writeMessage(defender->getName() + " had no successes");
			}
			m_currentReach = attacker->getPrimaryWeapon()->getLength();
			if(defendSuccesses > attackerSuccesses) {
				m_currentReach = defender->getPrimaryWeapon()->getLength();
				writeMessage(defender->getName() + " had more successes, taking initiative");
				switchInitiative();
			}
		}
	} else {
		cout << "Normal resolution" << endl;
		//roll dice
		int offenseSuccesses = DiceRoller::rollGetSuccess(attacker->getBTN(), attack.dice);
		int defenseSuccesses = DiceRoller::rollGetSuccess(defender->getBTN(), defend.dice);

		int MoS = offenseSuccesses - defenseSuccesses;

		if(MoS > 0) {
			if(attack.manuever == eOffensiveManuevers::FeintThrust || attack.manuever == eOffensiveManuevers::FeintSwing) {
				attacker->setBonusDice(defenseSuccesses);
				writeMessage(attacker->getName() + " feints their attack and receives " +
							 to_string(defenseSuccesses) + " action points on their next attack");
			}
			else {
				if(inflictWound(MoS, attack, defender) == true) {
					m_currentState = eCombatState::FinishedCombat;
					return;
				}
			}
			m_currentReach = attacker->getPrimaryWeapon()->getLength();
		}
		else if (MoS == 0) {
			//nothing happens
			writeMessage("no net successes");
		}
		else {
			if(defend.manuever == eDefensiveManuevers::Dodge) {
				writeMessage("attack dodged with " + to_string(-MoS) + " successes");				
			}
			else {
				writeMessage("attack deflected with " + to_string(-MoS) + " successes");
			}
			if(defend.manuever == eDefensiveManuevers::ParryLinked) {
				//resolve offense
				Offense offense = defender->getQueuedOffense();
				int linkedOffenseMoS = DiceRoller::rollGetSuccess(defender->getDisadvantagedBTN(), -MoS);
				cout << "Linked hits: " << linkedOffenseMoS << endl;
				if(linkedOffenseMoS > 0 && inflictWound(linkedOffenseMoS, offense, attacker) == true) {
					m_currentState = eCombatState::FinishedCombat;
					return;
				}
			}
			if(defend.manuever == eDefensiveManuevers::Counter) {
				cout << "bonus: " << offenseSuccesses << endl;
				defender->setBonusDice(offenseSuccesses);
				writeMessage(defender->getName() + " receives " + to_string(offenseSuccesses) + " action points in their next attack");
			}
			if(defend.manuever != eDefensiveManuevers::Dodge) {
				writeMessage(defender->getName() + " now has initative, becoming attacker");
				switchInitiative();
			}
		}
		
	}
	
	m_currentState = eCombatState::PostCombat;
	
}

void CombatInstance::doDualOffenseResolve()
{
	//dual aggression
	Offense attack = m_side1->getQueuedOffense();
	Offense attack2 = m_side2->getQueuedOffense();

	int MoS = DiceRoller::rollGetSuccess(m_side1->getBTN(), attack.dice);
	int MoS2 = DiceRoller::rollGetSuccess(m_side2->getBTN(), attack2.dice);

	//resolve both
	bool death = false;
	if(MoS > 0) {
		if(inflictWound(MoS, attack, m_side2) == true) {
			death = true;
		}
	}
	if(MoS2 > 0) {
		if(inflictWound(MoS2, attack2, m_side1) == true) {
			death = true;
		}
	}
	
	//intiative goes to whoever got more hits
	m_currentState = eCombatState::PostCombat;
	if(MoS > MoS2) {
		m_currentReach = m_side1->getPrimaryWeapon()->getLength();
		m_initiative = eInitiative::Side1;
	} else if (MoS < MoS2) {
		m_currentReach = m_side2->getPrimaryWeapon()->getLength();
		m_initiative = eInitiative::Side2;
	} else {
		//reroll if no one died
		m_currentState = eCombatState::RollInitiative;
	}

	m_currentState = death == true ? eCombatState::FinishedCombat : m_currentState;
}

void CombatInstance::doPostCombat()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);

	Defense defend = defender->getQueuedDefense();
	if(defend.manuever == eDefensiveManuevers::Dodge) {
		//allow using 2 dice to take initiative
	}
	
	m_side1->clearCreatureManuevers();
	m_side2->clearCreatureManuevers();
	m_dualRedThrow = false;
	switchTempo();

	if(m_numTempos > cMinFatigueTempos) {
		//roll for fatigue
		
	}
	m_numTempos++;
	
	m_currentState = eCombatState::Offense;
}


void CombatInstance::doEndCombat()
{
	writeMessage("Combat has ended", Log::eMessageTypes::Announcement);
	m_side1->clearCreatureManuevers();
	m_side2->clearCreatureManuevers();
	if(m_side1->isConscious() == true) {
		m_side1->setIdle();
	}
	if(m_side2->isConscious() == true) {
		m_side2->setIdle();
	}
	m_side1 = nullptr;
	m_side2 = nullptr;
	m_currentState = eCombatState::Uninitialized;
}

bool CombatInstance::inflictWound(int MoS, Offense attack, Creature* target, bool manueverFirst)
{
	
	eBodyParts bodyPart = WoundTable::getSingleton()->getSwing(attack.target);
	//any thrust manevuer should trigger this
	if(attack.manuever == eOffensiveManuevers::Thrust) {
		bodyPart = WoundTable::getSingleton()->getThrust(attack.target);
	}

	int finalDamage = MoS + attack.component->getDamage();

	const ArmorSegment armorAtLocation = target->getArmorAtPart(bodyPart);

	if(armorAtLocation.AV > 0) {
		writeMessage(target->getName() + "'s armor reduced wound level by " + to_string(armorAtLocation.AV));
	}
	bool doBlunt = false;
	//complicated armor calcs go here
	finalDamage -= armorAtLocation.AV;
	if(armorAtLocation.isMetal == true) {
		cout << "hit metal armor" << endl;
	}
	if(armorAtLocation.isMetal == true && attack.component->getType() != eDamageTypes::Blunt && finalDamage > 0) {
		if(attack.component->hasProperty(eWeaponProperties::MaillePiercing) == false &&
		   armorAtLocation.type == eArmorTypes::Maille) {
			writeMessage("Maille armor reduces wound level by half");
			//piercing attacks round up, otherwise round down
			if(attack.component->getType() == eDamageTypes::Piercing) {
				finalDamage = (finalDamage+1)/2;	
			} else {
				finalDamage = finalDamage / 2;
			}
			finalDamage = max(finalDamage, 1);
			doBlunt = true;
		} 
		else if(attack.component->hasProperty(eWeaponProperties::PlatePiercing) == false &&
				  armorAtLocation.type == eArmorTypes::Plate) {
			writeMessage("Plate armor reduces wound level by half");
			//piercing attacks round up, otherwise round down
			if(attack.component->getType() == eDamageTypes::Piercing) {
				finalDamage = (finalDamage+1)/2;	
			} else {
				finalDamage = finalDamage / 2;
			}
			finalDamage = max(finalDamage, 1);
			doBlunt = true;
		} 
		else if (armorAtLocation.type != eArmorTypes::Plate && armorAtLocation.type != eArmorTypes::Maille) {
			//weird case
			writeMessage("Metal armor reduces wound level by half");
			//piercing attacks round up, otherwise round down
			if(attack.component->getType() == eDamageTypes::Piercing) {
				finalDamage = (finalDamage+1)/2;	
			} else {
				finalDamage = finalDamage / 2;
			}
			finalDamage = max(finalDamage, 1);
			doBlunt = true;
		}
	}

	if(finalDamage <= 0) {
		writeMessage(target->getName() + "'s armor prevented any damage!", Log::eMessageTypes::Announcement);
		return false;
	}

	writeMessage(target->getName() + " received a level " + to_string(finalDamage) + " wound to " + bodyPartToString(bodyPart));
	eDamageTypes finalType = doBlunt == true ? eDamageTypes::Blunt : attack.component->getType();
	if(finalType == eDamageTypes::Blunt && armorAtLocation.isRigid == true) {
		finalDamage = min(3, finalDamage);
	}
	Wound *wound = WoundTable::getSingleton()->getWound(finalType, bodyPart, finalDamage);
	writeMessage(wound->getText(), Log::eMessageTypes::Damage);
	if(wound->getBTN() > target->getBTN())
	{
		writeMessage(target->getName() + " begins to struggle from the pain", Log::eMessageTypes::Alert);
	}
	target->inflictWound(wound, manueverFirst);

	if(target->getCreatureState() == eCreatureState::Dead) {
		//end combat
		writeMessage(target->getName() + " has been killed", Log::eMessageTypes::Announcement);
		m_currentState = eCombatState::FinishedCombat;
		return true;
	}
	if(target->getCreatureState() == eCreatureState::Unconscious) {
		//end combat
		writeMessage(target->getName() + " has been knocked unconcious", Log::eMessageTypes::Announcement);
		m_currentState = eCombatState::FinishedCombat;
		return true;
	}
	writeMessage("Wound impact causes " + target->getName() + " to lose " +
				 to_string(wound->getImpact()) + " action points!", Log::eMessageTypes::Alert);
	return false;
}

void CombatInstance::switchTempo()
{
	if(m_currentTempo == eTempo::First) {
		m_currentTempo = eTempo::Second;
	} else {
		// reset combat pools
		writeMessage("Exchange has ended, combat pools have reset");
		m_currentTempo = eTempo::First;
		m_side1->resetCombatPool();
		m_side2->resetCombatPool();
	}
}

bool CombatInstance::isAttackerPlayer()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	return attacker->isPlayer();
}

bool CombatInstance::isDefenderPlayer()
{
	Creature* attacker = nullptr;
	Creature* defender = nullptr;
	setSides(attacker, defender);
	return defender->isPlayer();
}

void CombatInstance::run()
{
	cout << "CombatState:" << (int)(m_currentState) << endl;
	switch(m_currentState)
	{
	case eCombatState::Uninitialized:
		return;
		break;
	case eCombatState::Initialized:
		doInitialization();
		break;
	case eCombatState::RollInitiative:
		doRollInitiative();
		break;
	case eCombatState::PreexchangeActions:
		doPreexchangeActions();
		break;
	case eCombatState::ResetState:
		doResetState();
		break;
	case eCombatState::Offense:
		doOffense();
		break;
	case eCombatState::StealInitiative:
		doStealInitiative();
		break;
	case eCombatState::StolenOffense:
		doStolenOffense();
		break;
	case eCombatState::DualOffense1:
		doDualOffense1();
		break;
	case eCombatState::DualOffenseStealInitiative:
		doDualOffenseStealInitiative();
		break;
	case eCombatState::DualOffense2:
		doDualOffense2();
		break;
	case eCombatState::DualOffenseSecondInitiative:
		doDualOffenseSecondInitiative();
		break;
	case eCombatState::Defense:
		doDefense();
		break;
	case eCombatState::ParryLinked:
		doParryLinked();
		break;
	case eCombatState::Resolution:
		doResolution();
		break;
	case eCombatState::DualOffenseResolve:
		doDualOffenseResolve();
		break;
	case eCombatState::PostCombat:
		doPostCombat();
		break;
	case eCombatState::FinishedCombat:
		doEndCombat();
		break;
	}
}

void CombatInstance::runUI()
{
	Game::getWindow();
}

void CombatInstance::writeMessage(const std::string& str, Log::eMessageTypes type)
{
	//combat manager is not a singleton, so we can have multiple.
	//we can choose not to display combatmanager messages if we want to.
	Log::push(str, type);
}

