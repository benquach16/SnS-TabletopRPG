#pragma once

#include "../combatinstance.h"
#include "../creatures/player.h"
#include "numberinput.h"

class DefenseUI {
public:
    void run(sf::Event event, Player* player);
    void resetState() { m_currentState = eUiState::ChooseManuever; }

private:
    void doManuever(sf::Event event, Player* player);
    void doChooseDice(sf::Event event, Player* player);

    enum class eUiState : unsigned {
        ChooseManuever,
        ChooseDice,
        ChooseLinkedManuever,
        ChooseLinkedTarget,
        Finished,
    };

    eUiState m_currentState;

    NumberInput m_numberInput;
};
