#pragma once

#include <SFML/Graphics.hpp>

#include "numberinput.h"

class CombatManager;

class CombatUI
{
public:
	CombatUI();
	void initialize(CombatManager* manager);
	
	void run(sf::Event event);

private:
	enum class eInitiativeSubState : unsigned {
		ChooseInitiative,
		Finished,
	};

	enum class eOffenseSubState : unsigned {
		ChooseManuever,
		ChooseComponent,
		ChooseDice,
		ChooseTarget,
		Finished,
	};

	enum class eStolenOffenseSubState : unsigned {
		ChooseDice,
		Finished,
	};

	enum class eDefenseSubState : unsigned {
		ChooseManuever,
		ChooseDice,
		Finished,
	};
	void resetState();
	void doInitiative();
	void doOffense(sf::Event event);
	void doDefense(sf::Event event);
	void doStolenOffense(sf::Event event);
	
	void showSide1Stats();
	void showSide2Stats();

	CombatManager *m_manager;

	NumberInput m_numberInput;

	eInitiativeSubState m_initiativeState;
	eOffenseSubState m_offenseState;
	eStolenOffenseSubState m_stolenOffenseState;
	eDefenseSubState m_defenseState;
};
