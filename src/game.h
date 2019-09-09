#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"
#include "gfxobjects/gfxlevel.h"
#include "gfxobjects/gfxselector.h"
#include "object/playerobject.h"
#include "object/selectorobject.h"
#include "ui/gameui.h"

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
    void setupLevel();
    void charCreation(bool hasKeyEvents, sf::Event event);
    void gameloop(bool hasKeyEvents, sf::Event event);

    void doMoveSelector(sf::Event event, bool limit);
    enum eApplicationState { CharCreation, MainMenu, Gameplay };
    enum eGameState {
        Uninitialized,
        Playing,
        SelectionMode,
        DialogueSelect,
        DialogueMode,
        AttackMode,
        Inventory,
        Pickup,
        InCombat,
        PauseMenu,
        Dead,
        Exiting
    };
    SelectorObject m_selector;
    Object* m_pickup;
    CreatureObject* m_talking;
    PlayerObject* m_playerObject;
    Level* m_currentLevel;
    GameUI m_ui;

    sf::Clock clock;
    float tick = 0;
    float aiTick = 0;

    GFXLevel m_gfxlevel;

    GFXSelector m_gfxSelector;

    float zoom = 1.0f;

    eApplicationState m_appState;
    eGameState m_currentState;
    static sf::RenderWindow m_window;
    static sf::Font m_defaultFont;
};
