#include <fstream>
#include <iostream>

#include "creatures/wound.h"
#include "game.h"
#include "gfxobjects/utils.h"
#include "items/weapon.h"
#include "level/level.h"
#include "log.h"
#include "object/humanobject.h"
#include "object/playerobject.h"
#include "object/relationmanager.h"
#include "object/selectorobject.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include <sstream>
using boost::serialization::make_binary_object;

using namespace std;

const string Game::cSaveString = "save.dat";
Game* Game::singleton = nullptr;
sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;

constexpr float cMaxZoom = 0.6f;
constexpr float cMinZoom = 1.4f;
constexpr int cRestTicks = 20;

Game::Game()
    : m_playerObject(nullptr)
{
}

Game::~Game()
{
    /*
    if (m_playerObject != nullptr) {
        delete m_playerObject;
        m_playerObject = nullptr;
    }
    */
    // m_currentlevel should be cleared by level manager, when it exists
    // temporary, move level management to levelmanager
}

void Game::load(const std::string& filepath)
{
    std::ifstream f(filepath, std::ifstream::binary);

    boost::archive::text_iarchive oa(f);
    oa >> m_scene;
    if (m_playerObject != nullptr) {
        delete m_playerObject;
    }
    m_playerObject = m_scene.getPlayer();
    setState(Game::eApplicationState::Gameplay);
}

void Game::save(const std::string& filepath)
{
    std::ofstream f(filepath, std::ofstream::binary);
    boost::archive::text_oarchive oa(f);
    oa << m_scene;
}

void Game::initialize()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    m_window.create(sf::VideoMode(1600, 900), "window", sf::Style::Default, settings);
    // sm_window.setFramerateLimit(165);
    m_defaultFont.loadFromFile("data/fonts/MorePerfectDOSVGA.ttf");
    // quite possibly the worst way of doing this, but cannot disable AA on sfml text without this.
    const_cast<sf::Texture&>(m_defaultFont.getTexture(11)).setSmooth(false);
    m_desktop.SetProperty<sf::Color>("*", "BackgroundColor", sf::Color(12, 12, 23));
    m_desktop.SetProperty<sf::Color>("Button", "BackgroundColor", sf::Color(12, 12, 23));
    m_desktop.SetProperty<sf::Color>("Button", "BorderColor", sf::Color(22, 22, 33));
    m_desktop.SetProperty<sf::Color>("Button:PRELIGHT", "BackgroundColor", sf::Color(22, 22, 33));
    m_desktop.SetProperty<sf::Color>("Button:ACTIVE", "BackgroundColor", sf::Color(22, 22, 33));
    m_desktop.SetProperty<string>("*", "FontName", "data/fonts/MorePerfectDOSVGA.ttf");
    m_desktop.SetProperty<int>("*", "FontSize", 14);
    m_desktop.SetProperty<sf::Color>("Window", "BorderColor", sf::Color(22, 22, 33));
    m_desktop.SetProperty<sf::Color>("Window", "TitleBackgroundColor", sf::Color(22, 22, 33));
	m_mainmenu.initialize();
    m_appState = eApplicationState::MainMenu;
}

void Game::setupNewgame()
{
    m_playerObject = new PlayerObject;
    m_scene.setupLevel(m_playerObject);
}

void Game::setupArena()
{
    m_playerObject = new PlayerObject;
    m_scene.setupArena(m_playerObject);
}

void Game::run()
{
    while (m_window.isOpen()) {
        sf::Event event;
        bool hasKeyEvents = false;
        while (m_window.pollEvent(event)) {
			m_desktop.HandleEvent(event);

            switch (event.type) {
            case sf::Event::Closed:
                m_window.close();
                // don't render to window after it's been closed
                return;
            case sf::Event::KeyPressed:
            case sf::Event::TextEntered:
            case sf::Event::KeyReleased:
                hasKeyEvents = true;
                break;
            default:
                hasKeyEvents = false;
                break;
            }
        }


        m_window.clear();
        switch (m_appState) {
        case eApplicationState::CharCreation:
            charCreation(hasKeyEvents, event);
            break;
        case eApplicationState::MainMenu:
            m_mainmenu.run(hasKeyEvents, event);
            break;
        case eApplicationState::Gameplay:
            m_scene.run(hasKeyEvents, event, m_playerObject);
            break;
        }
		m_desktop.Update(0.f);
        m_gui.Display(m_window);
        m_window.display();

        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / currentTime;
        // cout << "FPS"  << fps << endl;
        m_window.setTitle(std::to_string(fps));
		while (m_toDelete.empty() == false) {
			auto ptr = m_toDelete.front();
			m_toDelete.pop();
			m_desktop.Remove(ptr);
		}
    }
}

void Game::charCreation(bool hasKeyEvents, sf::Event event)
{
    m_ui.runCreate(hasKeyEvents, event, m_playerObject);
    if (m_ui.charCreated() == true) {
        m_appState = eApplicationState::Gameplay;
    }
}
