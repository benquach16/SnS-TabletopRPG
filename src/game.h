#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"

class Game {
public:
    void initialize();
    void run();
    void destroy();

    static sf::Font& getDefaultFont() { return m_defaultFont; }
    static sf::RenderWindow& getWindow() { return m_window; }

private:
    enum eGameState {
        Uninitialized,
        MainMenu,
        Playing,
        SelectionMode,
        DialogueMode,
        AttackMode,
        Inventory,
        Pickup,
        InCombat,
        PauseMenu,
        Exiting
    };
    eGameState m_currentState;
    static sf::RenderWindow m_window;
    static sf::Font m_defaultFont;
};
