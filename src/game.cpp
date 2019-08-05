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

void Game::initialize()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    m_window.create(sf::VideoMode(1600, 900), "window", sf::Style::Default, settings);
    m_window.setFramerateLimit(165);
    m_defaultFont.loadFromFile("data/fonts/MorePerfectDOSVGA.ttf");
    m_currentState = eGameState::Playing;
}

void Game::run()
{
    //clean me up
    PlayerObject* playerObject = new PlayerObject;

    sf::Clock clock;

    //main game loop
    float tick = 0;
    float aiTick = 0;

    GameUI ui;

    Level level(40, 40);
    for (int i = 0; i < 40; i++) {
        level(i, 0).m_type = eTileType::Wall;
    }

    for (int i = 2; i < 40; i++) {
        level(0, i).m_type = eTileType::Wall;
    }
    level.generate();
    GFXLevel gfxlevel;
    level.addObject(playerObject);

    playerObject->setPosition(1, 1);

    HumanObject target1;
    HumanObject target2;

    //playerObject->startCombatWith(target1.getCreatureComponent());
    //playerObject->startCombatWith(target2.getCreatureComponent());
    SelectorObject selector;
    GFXSelector gfxSelector;

    Object* pickup = nullptr;

    float zoom = 1.0f;

    while (m_window.isOpen()) {
        m_window.clear();

        sf::Event event;
        while (m_window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
        }
        sf::Time elapsedTime = clock.getElapsedTime();
        //as milliseconds returns 0, so we have to go more granular
        tick += elapsedTime.asSeconds();
        aiTick += elapsedTime.asSeconds();

        sf::View v = getWindow().getDefaultView();
        v.setSize(v.getSize().x, v.getSize().y * 2);
        //v.setCenter(v.getSize() *.5f);
        sf::Vector2f center(playerObject->getPosition().x, playerObject->getPosition().y);
        center = coordsToScreen(center);
        v.setCenter(center.x, center.y + 200);
        v.zoom(zoom);
        getWindow().setView(v);
        gfxlevel.run(&level, playerObject->getPosition());
        //temporary until we get graphics queue up and running
        if (m_currentState == eGameState::AttackMode || m_currentState == eGameState::SelectionMode || m_currentState == eGameState::DialogueMode) {
            gfxSelector.run(&selector);
        }
        getWindow().setView(getWindow().getDefaultView());
        ui.run(event);
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Equal) {
            zoom = max(0.6f, zoom - .1f);
        }
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Hyphen) {
            zoom = min(1.4f, zoom + 0.1f);
        }
        if (m_currentState == eGameState::Playing) {
            vector2d pos = playerObject->getPosition();
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
                if (level.isFreeSpace(pos.x, pos.y + 1) == true) {
                    playerObject->moveDown();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
                if (level.isFreeSpace(pos.x, pos.y - 1) == true) {
                    playerObject->moveUp();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
                if (level.isFreeSpace(pos.x - 1, pos.y) == true) {
                    playerObject->moveLeft();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
                if (level.isFreeSpace(pos.x + 1, pos.y) == true) {
                    playerObject->moveRight();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
                selector.setPosition(playerObject->getPosition());
                m_currentState = eGameState::AttackMode;
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::I) {
                m_currentState = eGameState::Inventory;
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D) {
                selector.setPosition(playerObject->getPosition());
                m_currentState = eGameState::SelectionMode;
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P) {
                Object* object = level.getObjectMutable(playerObject->getPosition(), playerObject);
                if (object != nullptr) {
                    switch (object->getObjectType()) {
                    case eObjectTypes::Corpse:
                        Log::push("Searching corpse...");
                        pickup = object;
                        m_currentState = eGameState::Pickup;
                        break;
                    case eObjectTypes::Chest:
                        Log::push("Opening chest...");
                        pickup = object;
                        m_currentState = eGameState::Pickup;
                        break;
                    case eObjectTypes::Creature:
                        Log::push("There is a creature here. You need to kill them if you want to loot them.");
                        break;
                    }
                } else {
                    Log::push("There is nothing here.");
                }
            }
            if (aiTick > 0.3) {
                level.run();
                aiTick = 0;
                level.cleanup();
            }
            if (playerObject->isInCombat() == true) {
                m_currentState = eGameState::InCombat;
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::K) {
                m_currentState = eGameState::DialogueMode;
            }

        } else if (m_currentState == eGameState::SelectionMode) {
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
                selector.moveDown();
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
                selector.moveUp();
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
                selector.moveLeft();
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
                selector.moveRight();
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
                const Object* object = level.getObject(selector.getPosition());
                if (object == nullptr) {
                    std::cout << "Nothing here" << std::endl;
                } else {
                    std::cout << "Something here" << std::endl;
                    Log::push("You see " + object->getDescription());
                    if (object->getObjectType() == eObjectTypes::Creature) {
                        const CreatureObject* creatureObj = static_cast<const CreatureObject*>(object);
                        //don't do anything more for player
                        if (creatureObj->isPlayer() == false) {
                            if (creatureObj->isConscious() == false) {
                                Log::push("They are unconscious", Log::eMessageTypes::Announcement);
                            }
                            int relation = RelationManager::getSingleton()->getRelationship(eCreatureFaction::Player,
                                creatureObj->getFaction());

                            if (relation <= RelationManager::cHostile) {
                                Log::push(creatureObj->getName() + " is hostile to you", Log::eMessageTypes::Damage);
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D) {
                m_currentState = eGameState::Playing;
            }
        } else if (m_currentState == eGameState::AttackMode) {
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
                if (selector.getPosition().y < playerObject->getPosition().y + 2) {
                    selector.moveDown();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
                if (selector.getPosition().y > playerObject->getPosition().y - 2) {
                    selector.moveUp();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
                if (selector.getPosition().x > playerObject->getPosition().x - 2) {
                    selector.moveLeft();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
                if (selector.getPosition().x < playerObject->getPosition().x + 2) {
                    selector.moveRight();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
                const Object* object = level.getObject(selector.getPosition());
                if (object != nullptr) {
                    if (object->getObjectType() == eObjectTypes::Creature) {
                        const CreatureObject* creatureObject = static_cast<const CreatureObject*>(object);
                        if (creatureObject->isConscious() == true) {
                            playerObject->startCombatWith(creatureObject->getCreatureComponent());
                            m_currentState = eGameState::InCombat;
                        } else {
                            creatureObject->kill();
                            Log::push("You finish off the unconscious creature");
                        }
                    }
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
                m_currentState = eGameState::Playing;
            }
        } else if (m_currentState == eGameState::Inventory) {
            ui.runInventory(event, playerObject);
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::I) {
                m_currentState = eGameState::Playing;
            }
        } else if (m_currentState == eGameState::Pickup) {
            assert(pickup != nullptr);
            ui.runTrade(event, playerObject->getInventoryMutable(), pickup->getInventoryMutable());
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P) {
                m_currentState = eGameState::Playing;
            }

        } else if (m_currentState == eGameState::InCombat) {
            ui.runCombat(event, playerObject->getCombatManager());
            if (playerObject->runCombat(tick) == false) {
                m_currentState = eGameState::Playing;
            }
            if (tick > CombatManager::cTick) {
                //pause rest of game if player is in combat. combat between two NPCS can happen anytime

                level.run();
                tick = 0;
            }
        } else if (m_currentState == eGameState::DialogueMode) {
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
                if (selector.getPosition().y < playerObject->getPosition().y + 2) {
                    selector.moveDown();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
                if (selector.getPosition().y > playerObject->getPosition().y - 2) {
                    selector.moveUp();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
                if (selector.getPosition().x > playerObject->getPosition().x - 2) {
                    selector.moveLeft();
                }
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
                if (selector.getPosition().x < playerObject->getPosition().x + 2) {
                    selector.moveRight();
                }
            }
        }

        Log::run();

        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / currentTime;
        //cout << "FPS"  << fps << endl;
        m_window.setTitle(std::to_string(fps));
        m_window.display();
    }
}
