#pragma once

#include "numberinput.h"
#include "../creatures/player.h"

class PositionUI
{
public:
	void run(sf::Event event, Player* player);
	void resetState() { m_currentState = eUiState::ChooseDice; }
private:
	void doPositionRoll(sf::Event event, Player* player);
	enum class eUiState : unsigned {
		ChooseDice,
		Finished
	};

	NumberInput m_numberInput;
	eUiState m_currentState;
};
