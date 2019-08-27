#pragma once

#include "../creatures/player.h"
#include "numberinput.h"

class PositionUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player);
    void resetState() { m_currentState = eUiState::ChooseDice; }

private:
    void doChooseManuever(bool hasKeyEvents, sf::Event event, Player* player);
    void doPositionRoll(bool hasKeyEvents, sf::Event event, Player* player);
    enum class eUiState : unsigned { ChooseDice, Finished };

    NumberInput m_numberInput;
    eUiState m_currentState;
};
