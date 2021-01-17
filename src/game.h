#pragma once

#include <SFML/Graphics.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Window.hpp>

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

    void changeToScene();
    void run();
    void destroy();
    Scene getCurrentScene();
    void setState(eApplicationState state) { m_appState = state; }

    static sf::Font& getDefaultFont() { return m_defaultFont; }
    static sf::RenderWindow& getWindow() { return m_window; }
	sfg::Desktop& getDesktop() { return m_desktop; }
	void deleteWidget(sfg::Widget::Ptr ptr) { m_toDelete.push(ptr); }
    static Game* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new Game;
        }
        return singleton;
    }

	static void cleanup()
	{
		if (singleton != nullptr) {
			delete singleton;
		}
		singleton = nullptr;
	}

private:
    Game();
    ~Game();

    void charCreation(bool hasKeyEvents, sf::Event event);
	sfg::SFGUI m_gui;
	sfg::Desktop m_desktop;
	std::queue<sfg::Widget::Ptr> m_toDelete;
    PlayerObject* m_playerObject;
    CreateCharUI m_createUI;
    MainMenuUI m_mainmenu;
    sf::Clock clock;

    float zoom = 1.0f;
    Scene m_scene;
    std::vector<Scene> m_scenes;
    int m_currentScene;
    Overworld m_overworld;
    eApplicationState m_appState;

    static Game* singleton;

    static sf::RenderWindow m_window;
    static sf::Font m_defaultFont;
};
