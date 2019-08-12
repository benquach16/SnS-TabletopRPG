#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"
#include "object/playerobject.h"
#include "object/selectorobject.h"

class Game {
public:
    Game();
    ~Game();
    void initialize();
    void run();
    void destroy();

    static sf::Font& getDefaultFont() { return m_defaultFont; }
    static sf::RenderWindow& getWindow() { return m_window; }

private:
    void doMoveSelector(sf::Event event, bool limit);
    enum eGameState {
        Uninitialized,
        MainMenu,
        Playing,
        SelectionMode,
        DialogueSelect,
        DialogueMode,
        AttackMode,
        Inventory,
        Pickup,
        InCombat,
        PauseMenu,
        Exiting
    };
    SelectorObject m_selector;
    Object* m_pickup;
    CreatureObject* m_talking;
    PlayerObject* m_playerObject;

    eGameState m_currentState;
    static sf::RenderWindow m_window;
    static sf::Font m_defaultFont;
};
