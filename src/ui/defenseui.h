#pragma once

#include "../combatinstance.h"
#include "../creatures/player.h"
#include "numberinput.h"

class DefenseUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player, bool lastTempo);
    void resetState() { m_currentState = eUiState::ChooseManuever; }

private:
    void doManuever(bool hasKeyEvents, sf::Event event, Player* player, bool lastTempo);
    void doChooseDice(bool hasKeyEvents, sf::Event event, Player* player);

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
