#pragma once

#include "../creatures/player.h"
#include "numberinput.h"

class PrecombatUI {
public:
    void run(
        bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple, bool secondExchange);
    void resetState() { m_currentState = eUiState::ChooseFavoring; }

private:
    void doFavoring(bool hasKeyEvents, sf::Event event, Player* player, bool secondExchange);
    void doFavorLocation(bool hasKeyEvents, sf::Event event, Player* player);
    void doQuickdraw(bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple);
    enum class eUiState : unsigned {
        ChooseFavoring,
        ChooseFavorLocations,
        ChooseQuickdraw,
        Finished
    };

    eUiState m_currentState;
};
