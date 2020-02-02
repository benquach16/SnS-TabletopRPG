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

using namespace std;

sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;

constexpr float cMaxZoom = 0.6f;
constexpr float cMinZoom = 1.4f;

Game::Game()
    : m_pickup(nullptr)
    , m_talking(nullptr)
    , m_playerObject(nullptr)
    , m_currentLevel(nullptr)
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
    delete m_currentLevel;
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
    m_currentState = eGameState::Playing;
    m_appState = eApplicationState::CharCreation;
    setupLevel();
}

void Game::run()
{
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

        m_window.clear();
        switch (m_appState) {
        case eApplicationState::CharCreation:
            charCreation(hasKeyEvents, event);
            break;
        case eApplicationState::MainMenu:
            break;
        case eApplicationState::Gameplay:
            gameloop(hasKeyEvents, event);
            break;
        }
        m_window.display();
    }
}

void Game::charCreation(bool hasKeyEvents, sf::Event event)
{
    m_ui.runCreate(hasKeyEvents, event, m_playerObject);
    if (m_ui.charCreated() == true) {
        m_appState = eApplicationState::Gameplay;
    }
}

void Game::setupLevel()
{
    m_playerObject = new PlayerObject;
    m_currentLevel = new Level(60, 60);
    m_currentLevel->generate();
    for (unsigned i = 0; i < 60; i++) {
        (*m_currentLevel)(i, 0).m_type = eTileType::Wall;
    }

    for (unsigned i = 2; i < 60; i++) {
        (*m_currentLevel)(0, i).m_type = eTileType::Wall;
    }
    m_currentLevel->addObject(m_playerObject);

    HumanObject* human1 = new HumanObject;
    human1->setFaction(eCreatureFaction::Confederacy);
    human1->setLoadout(eCreatureFaction::Confederacy, eRank::Soldier);
    human1->setPosition(2, 2);
    human1->setAIRole(eAIRoles::Standing);
    human1->setStartingDialogueLabel("greeting_intro");
    human1->getCreatureComponent()->setAgility(9);
    human1->getCreatureComponent()->setCunning(9);
    human1->getCreatureComponent()->setPerception(9);
    human1->setName("Sir Wilhelm");
    m_talking = human1;
    m_ui.initDialog(human1);
    m_currentState = eGameState::DialogueMode;
    m_currentLevel->addObject(human1);

    m_playerObject->setPosition(1, 1);
}

void Game::gameloop(bool hasKeyEvents, sf::Event event)
{
    sf::View v = getWindow().getDefaultView();
    v.setSize(v.getSize().x, v.getSize().y * 2);
    // v.setCenter(v.getSize() *.5f);
    sf::Vector2f center(m_playerObject->getPosition().x, m_playerObject->getPosition().y);
    center = coordsToScreen(center);
    v.setCenter(center.x, center.y + 200);
    v.zoom(zoom);
    getWindow().setView(v);
    m_gfxlevel.run(m_currentLevel, m_playerObject->getPosition());
    // temporary until we get graphics queue up and running
    if (m_currentState == eGameState::AttackMode || m_currentState == eGameState::SelectionMode
        || m_currentState == eGameState::DialogueSelect) {
        m_gfxSelector.run(&m_selector);
    }
    m_gfxlevel.renderText();
    getWindow().setView(getWindow().getDefaultView());

    m_ui.run();

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Equal) {
        zoom = max(cMaxZoom, zoom - .1f);
    }
    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Hyphen) {
        zoom = min(cMinZoom, zoom + 0.1f);
    }

    switch (m_currentState) {
    case eGameState::Playing:
    case eGameState::SelectionMode:
    case eGameState::DialogueMode:
    case eGameState::AttackMode:
    case eGameState::Inventory:
    case eGameState::Pickup:
    case eGameState::InCombat:
    default:
        break;
    }

    if (m_playerObject->isConscious() == false && m_currentState != eGameState::Dead) {
        m_currentState = eGameState::Dead;
        Log::push("You have died!", Log::eMessageTypes::Damage);
    }

    if (m_currentState == eGameState::Playing) {
        vector2d pos = m_playerObject->getPosition();
        if (hasKeyEvents && event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Down) {
                if (m_currentLevel->isFreeSpace(pos.x, pos.y + 1) == true) {
                    m_playerObject->moveDown();
                }
            }
            if (event.key.code == sf::Keyboard::Up) {
                if (m_currentLevel->isFreeSpace(pos.x, pos.y - 1) == true) {
                    m_playerObject->moveUp();
                }
            }
            if (event.key.code == sf::Keyboard::Left) {
                if (m_currentLevel->isFreeSpace(pos.x - 1, pos.y) == true) {
                    m_playerObject->moveLeft();
                }
            }
            if (event.key.code == sf::Keyboard::Right) {
                if (m_currentLevel->isFreeSpace(pos.x + 1, pos.y) == true) {
                    m_playerObject->moveRight();
                }
            }
        } else if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::A) {
                m_selector.setPosition(m_playerObject->getPosition());
                m_currentState = eGameState::AttackMode;
            }
            if (event.key.code == sf::Keyboard::I) {
                m_currentState = eGameState::Inventory;
            }
            if (event.key.code == sf::Keyboard::D) {
                m_selector.setPosition(m_playerObject->getPosition());
                m_currentState = eGameState::SelectionMode;
            }
            if (event.key.code == sf::Keyboard::P) {
                Object* object = m_currentLevel->getObjectMutable(
                    m_playerObject->getPosition(), m_playerObject);
                if (object != nullptr) {
                    switch (object->getObjectType()) {
                    case eObjectTypes::Corpse:
                        Log::push("Searching corpse...");
                        m_pickup = object;
                        m_currentState = eGameState::Pickup;
                        break;
                    case eObjectTypes::Chest:
                        Log::push("Opening chest...");
                        m_pickup = object;
                        m_currentState = eGameState::Pickup;
                        break;
                    case eObjectTypes::Creature:
                        Log::push("There is a creature here. You need to kill "
                                  "them if you want to loot them.",
                            Log::eMessageTypes::Alert);
                        break;
                    default:
                        Log::push("Nothing to loot");
                        break;
                    }
                } else {
                    Log::push("There is nothing here.");
                }
            }
            if (event.key.code == sf::Keyboard::K) {
                m_selector.setPosition(m_playerObject->getPosition());
                m_currentState = eGameState::DialogueSelect;
            }
        }

        if (aiTick > 0.4) {
            m_currentLevel->run();
            aiTick = 0;
            m_currentLevel->cleanup();
        }
        if (m_playerObject->isInCombat() == true) {
            m_currentState = eGameState::InCombat;
        }

    } else if (m_currentState == eGameState::DialogueSelect) {
        if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
            doMoveSelector(event, true);
            if (event.key.code == sf::Keyboard::Enter) {
                Object* object
                    = m_currentLevel->getObjectMutable(m_selector.getPosition(), m_playerObject);
                if (object != nullptr) {
                    if (object->getObjectType() == eObjectTypes::Creature) {
                        CreatureObject* creatureObject = static_cast<CreatureObject*>(object);
                        if (creatureObject->isConscious() == true) {
                            m_talking = creatureObject;
                            m_ui.initDialog(m_talking);
                            m_currentState = eGameState::DialogueMode;
                        } else {
                            Log::push("You can't talk to an unconscious creature");
                        }
                    }
                }
            }
            if (event.key.code == sf::Keyboard::K) {
                m_currentState = eGameState::Playing;
            }
        }

    } else if (m_currentState == eGameState::SelectionMode) {
        if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
            doMoveSelector(event, false);
            if (event.key.code == sf::Keyboard::Enter) {
                const Object* object = m_currentLevel->getObject(m_selector.getPosition());
                if (object != nullptr) {
                    Log::push("You see " + object->getDescription());
                    if (object->getObjectType() == eObjectTypes::Creature) {
                        const CreatureObject* creatureObj
                            = static_cast<const CreatureObject*>(object);
                        // don't do anything more for player
                        if (creatureObj->isPlayer() == false) {
                            if (creatureObj->isConscious() == false) {
                                Log::push("They are unconscious", Log::eMessageTypes::Announcement);
                            }
                            int relation = RelationManager::getSingleton()->getRelationship(
                                eCreatureFaction::Player, creatureObj->getFaction());

                            if (relation <= RelationManager::cHostile) {
                                Log::push(creatureObj->getName() + " is hostile to you",
                                    Log::eMessageTypes::Damage);
                            }
                        }
                    }
                }
            }

            if (event.key.code == sf::Keyboard::D) {
                m_currentState = eGameState::Playing;
            }
        }
    } else if (m_currentState == eGameState::AttackMode) {
        if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
            doMoveSelector(event, true);
            if (event.key.code == sf::Keyboard::Enter) {
                const Object* object = m_currentLevel->getObject(m_selector.getPosition());
                if (object != nullptr) {
                    if (object->getObjectType() == eObjectTypes::Creature) {
                        const CreatureObject* creatureObject
                            = static_cast<const CreatureObject*>(object);
                        if (creatureObject->isConscious() == true) {
                            m_playerObject->startCombatWith(creatureObject);
                            m_currentState = eGameState::InCombat;
                        } else {
                            creatureObject->kill();
                            Log::push("You finish off the unconscious creature");
                        }
                    }
                }
            }
            if (event.key.code == sf::Keyboard::A) {
                m_currentState = eGameState::Playing;
            }
        }
    } else if (m_currentState == eGameState::Inventory) {
        m_ui.runInventory(hasKeyEvents, event, m_playerObject);
        if (hasKeyEvents && event.type == sf::Event::KeyReleased
            && event.key.code == sf::Keyboard::I) {
            m_currentState = eGameState::Playing;
        }
    } else if (m_currentState == eGameState::Pickup) {
        assert(m_pickup != nullptr);
        m_ui.runTrade(hasKeyEvents, event, m_playerObject->getInventoryMutable(),
            m_pickup->getInventoryMutable());
        if (hasKeyEvents && event.type == sf::Event::KeyReleased
            && event.key.code == sf::Keyboard::P) {
            m_currentState = eGameState::Playing;
        }

    } else if (m_currentState == eGameState::InCombat) {
        m_ui.runCombat(hasKeyEvents, event, m_playerObject->getCombatManager());

        if (m_playerObject->runCombat(tick) == false) {
            m_currentState = eGameState::Playing;
        }
        /*
          if (m_playerObject->isInCombat() == false) {
          m_currentState = eGameState::Playing;
          }
        */
        if (tick > CombatManager::cTick) {
            // issue here is if player is engaged and is not the parent. the ai updates so
            // slowly that its hard for the player to understand whats going on
            m_currentLevel->run();
            tick = 0;
        }
    } else if (m_currentState == eGameState::DialogueMode) {
        assert(m_talking != nullptr);
        if (m_ui.runDialog(hasKeyEvents, event, m_playerObject, m_talking) == false) {
            m_currentState = eGameState::Playing;
        }
    } else if (m_currentState == eGameState::Dead) {
    }

    Log::run();

    float currentTime = clock.restart().asSeconds();
    aiTick += currentTime;
    tick += currentTime;
    float fps = 1.f / currentTime;
    // cout << "FPS"  << fps << endl;
    m_window.setTitle(std::to_string(fps));
}

void Game::doMoveSelector(sf::Event event, bool limit)
{
    constexpr int cLimit = 2;
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Down) {
            if (limit == false
                || m_selector.getPosition().y < m_playerObject->getPosition().y + cLimit) {
                m_selector.moveDown();
            }
        }
        if (event.key.code == sf::Keyboard::Up) {
            if (limit == false
                || m_selector.getPosition().y > m_playerObject->getPosition().y - cLimit) {
                m_selector.moveUp();
            }
        }
        if (event.key.code == sf::Keyboard::Left) {
            if (limit == false
                || m_selector.getPosition().x > m_playerObject->getPosition().x - cLimit) {
                m_selector.moveLeft();
            }
        }
        if (event.key.code == sf::Keyboard::Right) {
            if (limit == false
                || m_selector.getPosition().x < m_playerObject->getPosition().x + cLimit) {
                m_selector.moveRight();
            }
        }
    }
}
