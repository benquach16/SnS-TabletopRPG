#pragma once

#include "numberinput.h"
#include "../creatures/player.h"

class OffenseUI
{
public:
	void run(sf::Event event, Player* player, Creature* target, bool allowStealInitiative = false, bool linkedParry = false);
	void resetState() { m_currentState = eUiState::ChooseManuever; }
private:
	void doManuever(sf::Event event, Player* player);
	void doComponent(sf::Event event, Player* player);
	void doDice(sf::Event event, Player* player);
	void doTarget(sf::Event event, Player* player, bool linkedParry);
	void doInspect(sf::Event event, Creature* target);
	void doPinpointThrust(sf::Event event, Player* player);
	
	enum class eUiState : unsigned {
		ChooseManuever,
		InspectTarget,
		ChooseComponent,
		ChooseDice,
		ChooseTarget,
		PinpointThrust,
		Finished,
	};

	NumberInput m_numberInput;
	eUiState m_currentState;
};
