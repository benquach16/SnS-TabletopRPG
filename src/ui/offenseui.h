#pragma once

#include "../creatures/player.h"
#include "numberinput.h"

class OffenseUI {
public:
    void run(sf::Event event, Player* player, Creature* target, bool allowStealInitiative = false,
        bool linkedParry = false);
    void resetState() { m_currentState = eUiState::ChooseManuever; }

private:
    void doManuever(sf::Event event, Player* player, bool linkedParry = false);
    void doFeint(sf::Event event, Player* player);
    void doComponent(sf::Event event, Player* player);
    void doDice(sf::Event event, Player* player);
    void doTarget(sf::Event event, Player* player, bool linkedParry, Creature* target);
    void doInspect(sf::Event event, Creature* target);
    void doPinpointThrust(sf::Event event, Player* player);

    enum class eUiState : unsigned {
        ChooseManuever,
        ChooseFeint,
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
