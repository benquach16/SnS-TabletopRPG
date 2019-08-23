#include <iostream>

#include "creatures/wound.h"
#include "game.h"
#include "gfxobjects/gfxlevel.h"
#include "gfxobjects/gfxselector.h"
#include "gfxobjects/utils.h"
#include "items/weapon.h"
#include "level/level.h"
#include "log.h"
#include "object/humanobject.h"
#include "object/playerobject.h"
#include "object/relationmanager.h"
#include "object/selectorobject.h"
#include "ui/gameui.h"

using namespace std;

sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;

constexpr float cMaxZoom = 0.6;
constexpr float cMinZoom = 1.4;

Game::Game()
    : m_pickup(nullptr)
    , m_talking(nullptr)
    , m_playerObject(nullptr)
{
    m_playerObject = new PlayerObject;
}

Game::~Game() { delete m_playerObject; }

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
}

void Game::run()
{
    sf::Clock clock;

    // main game loop
    float tick = 0;
    float aiTick = 0;

    GameUI ui;

    Level level(60, 60);
    for (unsigned i = 0; i < 60; i++) {
        level(i, 0).m_type = eTileType::Wall;
    }

    for (unsigned i = 2; i < 60; i++) {
        level(0, i).m_type = eTileType::Wall;
    }
    level.generate();
    GFXLevel gfxlevel;
    level.addObject(m_playerObject);

    HumanObject* human1 = new HumanObject;
    human1->setFaction(eCreatureFaction::EidgenConfederacy);
    human1->setLoadout(eCreatureFaction::EidgenConfederacy);
    human1->setPosition(2, 2);
    human1->setAIRole(eAIRoles::Standing);
    human1->setStartingDialogueLabel("greeting_intro");
    human1->setName("Sir Wilhelm");
    m_talking = human1;
    ui.initDialog(human1);
    m_currentState = eGameState::DialogueMode;
    level.addObject(human1);

    m_playerObject->setPosition(1, 1);

    GFXSelector gfxSelector;

    float zoom = 1.0f;

    while (m_window.isOpen()) {
        m_window.clear();

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
        sf::Time elapsedTime = clock.getElapsedTime();
        // as milliseconds returns 0, so we have to go more granular
        tick += elapsedTime.asSeconds();
        aiTick += elapsedTime.asSeconds();

        sf::View v = getWindow().getDefaultView();
        v.setSize(v.getSize().x, v.getSize().y * 2);
        // v.setCenter(v.getSize() *.5f);
        sf::Vector2f center(m_playerObject->getPosition().x, m_playerObject->getPosition().y);
        center = coordsToScreen(center);
        v.setCenter(center.x, center.y + 200);
        v.zoom(zoom);
        getWindow().setView(v);
        gfxlevel.run(&level, m_playerObject->getPosition());
        // temporary until we get graphics queue up and running
        if (m_currentState == eGameState::AttackMode || m_currentState == eGameState::SelectionMode
            || m_currentState == eGameState::DialogueSelect) {
            gfxSelector.run(&m_selector);
        }
        gfxlevel.renderText();
        getWindow().setView(getWindow().getDefaultView());

        ui.run();

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

        if (m_currentState == eGameState::Playing) {
            vector2d pos = m_playerObject->getPosition();
            if (hasKeyEvents && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Down) {
                    if (level.isFreeSpace(pos.x, pos.y + 1) == true) {
                        m_playerObject->moveDown();
                    }
                }
                if (event.key.code == sf::Keyboard::Up) {
                    if (level.isFreeSpace(pos.x, pos.y - 1) == true) {
                        m_playerObject->moveUp();
                    }
                }
                if (event.key.code == sf::Keyboard::Left) {
                    if (level.isFreeSpace(pos.x - 1, pos.y) == true) {
                        m_playerObject->moveLeft();
                    }
                }
                if (event.key.code == sf::Keyboard::Right) {
                    if (level.isFreeSpace(pos.x + 1, pos.y) == true) {
                        m_playerObject->moveRight();
                    }
                }
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
                    Object* object
                        = level.getObjectMutable(m_playerObject->getPosition(), m_playerObject);
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

            if (aiTick > 0.3) {
                level.run();
                aiTick = 0;
                level.cleanup();
            }
            if (m_playerObject->isInCombat() == true) {
                m_currentState = eGameState::InCombat;
            }

        } else if (m_currentState == eGameState::DialogueSelect) {
            if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
                doMoveSelector(event, true);
                if (event.key.code == sf::Keyboard::Enter) {
                    Object* object
                        = level.getObjectMutable(m_selector.getPosition(), m_playerObject);
                    if (object != nullptr) {
                        if (object->getObjectType() == eObjectTypes::Creature) {
                            CreatureObject* creatureObject = static_cast<CreatureObject*>(object);
                            if (creatureObject->isConscious() == true) {
                                m_talking = creatureObject;
                                ui.initDialog(m_talking);
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
            if (event.type == sf::Event::KeyReleased) {
                doMoveSelector(event, false);
                if (event.key.code == sf::Keyboard::Enter) {
                    const Object* object = level.getObject(m_selector.getPosition());
                    if (object != nullptr) {
                        Log::push("You see " + object->getDescription());
                        if (object->getObjectType() == eObjectTypes::Creature) {
                            const CreatureObject* creatureObj
                                = static_cast<const CreatureObject*>(object);
                            // don't do anything more for player
                            if (creatureObj->isPlayer() == false) {
                                if (creatureObj->isConscious() == false) {
                                    Log::push(
                                        "They are unconscious", Log::eMessageTypes::Announcement);
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
                    const Object* object = level.getObject(m_selector.getPosition());
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
            ui.runInventory(hasKeyEvents, event, m_playerObject);
            if (hasKeyEvents && event.type == sf::Event::KeyReleased
                && event.key.code == sf::Keyboard::I) {
                m_currentState = eGameState::Playing;
            }
        } else if (m_currentState == eGameState::Pickup) {
            assert(m_pickup != nullptr);
            ui.runTrade(hasKeyEvents, event, m_playerObject->getInventoryMutable(),
                m_pickup->getInventoryMutable());
            if (hasKeyEvents && event.type == sf::Event::KeyReleased
                && event.key.code == sf::Keyboard::P) {
                m_currentState = eGameState::Playing;
            }

        } else if (m_currentState == eGameState::InCombat) {
            ui.runCombat(hasKeyEvents, event, m_playerObject->getCombatManager());

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
                level.run();
                tick = 0;
            }
        } else if (m_currentState == eGameState::DialogueMode) {
            assert(m_talking != nullptr);
            if (ui.runDialog(hasKeyEvents, event, m_playerObject, m_talking) == false) {
                m_currentState = eGameState::Playing;
            }
        }

        Log::run();

        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / currentTime;
        // cout << "FPS"  << fps << endl;
        m_window.setTitle(std::to_string(fps));
        m_window.display();
    }
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
