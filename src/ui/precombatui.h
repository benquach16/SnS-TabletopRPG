#pragma once

#include <SFGUI/Box.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Table.hpp>
#include <SFGUI/Window.hpp>

#include "creatures/player.h"
#include "numberinput.h"

class PrecombatUI {
public:
    PrecombatUI();
    void run(
        bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple, bool secondExchange);
    void resetState() { m_currentState = eUiState::ChooseFavoring; }
    void initialize();
    void hide();
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

    sfg::Window::Ptr m_window;
    sfg::Button::Ptr m_positioningBtn;
    sfg::Button::Ptr m_guardBtn;
    std::vector<sfg::Window::Ptr> m_tmpWindows;

    Player* m_currentPlayer;
    bool m_inGrapple;
};
