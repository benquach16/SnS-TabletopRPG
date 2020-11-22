#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"
#include "gfxobjects/gfxlevel.h"
#include "gfxobjects/gfxselector.h"
#include "object/playerobject.h"
#include "object/selectorobject.h"
#include "scene.h"
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

    enum class eApplicationState : unsigned { CharCreation, MainMenu, Gameplay };
    PlayerObject* m_playerObject;
    GameUI m_ui;

    sf::Clock clock;

    float zoom = 1.0f;

    eApplicationState m_appState;
    static sf::RenderWindow m_window;
    static sf::Font m_defaultFont;
};
