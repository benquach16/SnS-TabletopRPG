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
	sf::RectangleShape m_combatBkg;
	
	enum class eInitiativeSubState : unsigned {
		ChooseInitiative,
		InspectTarget,
		Finished,
	};

	enum class eStolenOffenseSubState : unsigned {
		ChooseDice,
		Finished,
	};

	enum class eDualRedStealSubState : unsigned {
		ChooseInitiative,
		ChooseDice,
		Finished
	};


	void resetState();
	void doInitiative();
	
	void doStolenOffense(sf::Event event);
	void doDualRedSteal(sf::Event event);

	void showSide1Stats();
	void showSide2Stats();

	CombatInstance *m_instance;

	NumberInput m_numberInput;

	eInitiativeSubState m_initiativeState;
	eStolenOffenseSubState m_stolenOffenseState;
	eDualRedStealSubState m_dualRedState;

	DefenseUI m_defenseUI;
	OffenseUI m_offenseUI;
};
