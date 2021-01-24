#pragma once

#include "creatures/player.h"
#include "numberinput.h"

class CombatInstance;

class PreresolveUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player, const Creature* target, const CombatInstance* instance);
    void resetState() { m_currentState = eUiState::ChooseFeint; }

private:
    void doFeint(bool hasKeyEvents, sf::Event event, Player* player);
    void doDice(bool hasKeyEvents, sf::Event event, Player* player);
    void doManuever(bool hasKeyEvents, sf::Event event, Player* player, const CombatInstance* instance);
    void doTarget(bool hasKeyEvents, sf::Event event, Player* player, const Creature* target);
    void doComponent(bool hasKeyEvents, sf::Event event, Player* player);
    enum class eUiState : unsigned { ChooseFeint, ChooseDice, ChooseManuever, ChooseTarget, ChooseComponent, Finished };

    eUiState m_currentState;
    NumberInput m_numberInput;
};
