#pragma once

#include "../combatinstance.h"
#include "../creatures/player.h"
#include "numberinput.h"

class DefenseUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player, bool lastTempo);
    void resetState() { m_currentState = eUiState::ChooseWeapon; }

private:
    void doChooseWeapon(bool hasKeyEvents, sf::Event event, Player* player);
    void doManuever(bool hasKeyEvents, sf::Event event, Player* player, bool lastTempo);
    void doChooseDice(bool hasKeyEvents, sf::Event event, Player* player);

    enum class eUiState : unsigned {
        ChooseWeapon,
        ChooseManuever,
        ChooseDice,
        ChooseLinkedManuever,
        ChooseLinkedTarget,
        Finished,
    };

    eUiState m_currentState;

    NumberInput m_numberInput;
};
