#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "numberinput.h"
#include "defenseui.h"
#include "offenseui.h"

class CombatInstance;

class CombatUI
{
public:
	CombatUI();
	void initialize(CombatInstance* instance) { m_instance = instance; }
	
	void run(sf::Event event);

private:
	enum class eInitiativeSubState : unsigned {
		ChooseInitiative,
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

	CombatInstance *m_instance;

	NumberInput m_numberInput;

	eInitiativeSubState m_initiativeState;
	eStolenOffenseSubState m_stolenOffenseState;

	DefenseUI m_defenseUI;
	OffenseUI m_offenseUI;
};
