#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"
#include "gfxobjects/gfxlevel.h"
#include "gfxobjects/gfxselector.h"
#include "object/playerobject.h"
#include "object/selectorobject.h"
#include "overworld.h"
#include "scene.h"
#include "ui/gameui.h"
#include "ui/mainmenuui.h"

class Game {
public:
    enum class eApplicationState : unsigned { CharCreation, MainMenu, Gameplay };
    const static std::string cSaveString;
    void load(const std::string& filepath);
    void save(const std::string& filepath);
    void initialize();
    void setupNewgame();
	void setupArena();
    void run();
    void destroy();
    void setState(eApplicationState state) { m_appState = state; }

    static sf::Font& getDefaultFont() { return m_defaultFont; }
    static sf::RenderWindow& getWindow() { return m_window; }

    static Game* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new Game;
        }
        return singleton;
    }

private:
    Game();
    ~Game();

    void charCreation(bool hasKeyEvents, sf::Event event);

    PlayerObject* m_playerObject;
    GameUI m_ui;
    MainMenuUI m_mainmenu;
    sf::Clock clock;

    float zoom = 1.0f;
    Scene m_scene;
    Overworld m_overworld;
    eApplicationState m_appState;

    static Game* singleton;

    static sf::RenderWindow m_window;
    static sf::Font m_defaultFont;
};
