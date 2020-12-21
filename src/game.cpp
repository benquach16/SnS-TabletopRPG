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

sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;

constexpr float cMaxZoom = 0.6f;
constexpr float cMinZoom = 1.4f;
constexpr int cRestTicks = 20;

Game::Game()
    : m_playerObject(nullptr)
{
    // force initialization
    WeaponTable::getSingleton();
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

void Game::initialize()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    m_window.create(sf::VideoMode(1600, 900), "window", sf::Style::Default, settings);
    m_window.setFramerateLimit(165);
    m_defaultFont.loadFromFile("data/fonts/MorePerfectDOSVGA.ttf");
    // quite possibly the worst way of doing this, but cannot disable AA on sfml text without this.
    const_cast<sf::Texture&>(m_defaultFont.getTexture(11)).setSmooth(false);
    m_appState = eApplicationState::CharCreation;
    setupLevel();
}

void Game::run()
{

    Scene scene;
    scene.setupLevel(m_playerObject);

    while (m_window.isOpen()) {
        sf::Event event;
        bool hasKeyEvents = false;
        while (m_window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;
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
        if (event.text.unicode == 'm' && hasKeyEvents) {
            std::ofstream f("save.dat", std::ofstream::binary);
            boost::archive::text_oarchive oa(f);
            oa << scene;
        }
        if (event.text.unicode == 'n' && hasKeyEvents) {
        }
        m_window.clear();
        switch (m_appState) {
        case eApplicationState::CharCreation:
            charCreation(hasKeyEvents, event);
            break;
        case eApplicationState::MainMenu:
            break;
        case eApplicationState::Gameplay:
            scene.run(hasKeyEvents, event, m_playerObject);
            // gameloop(hasKeyEvents, event);
            break;
        }
        m_window.display();
        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / currentTime;
        // cout << "FPS"  << fps << endl;
        m_window.setTitle(std::to_string(fps));
    }
}

void Game::charCreation(bool hasKeyEvents, sf::Event event)
{
    m_ui.runCreate(hasKeyEvents, event, m_playerObject);
    if (m_ui.charCreated() == true) {
        m_appState = eApplicationState::Gameplay;
    }
}

void Game::setupLevel() { m_playerObject = new PlayerObject; }
