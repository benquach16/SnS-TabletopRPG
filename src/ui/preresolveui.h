#pragma once

#include "creatures/player.h"
#include "numberinput.h"

class PreresolveUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player);
    void resetState() { m_currentState = eUiState::ChooseFeint; }

private:
    void doFeint(bool hasKeyEvents, sf::Event event, Player* player);
    void doDice(bool hasKeyEvents, sf::Event event, Player* player);
    enum class eUiState : unsigned { ChooseFeint, ChooseDice, Finished };

    eUiState m_currentState;
    NumberInput m_numberInput;
};
