#include "scene.h"
#include "../3rdparty/random.hpp"
#include "creatures/wound.h"
#include "game.h"
#include "gfxobjects/utils.h"
#include "items/weapon.h"
#include "level/changeleveltrigger.h"
#include "level/factionclearedtrigger.h"
#include "level/level.h"
#include "log.h"
#include "object/campfireobject.h"
#include "object/humanobject.h"
#include "object/playerobject.h"
#include "object/relationmanager.h"
#include "object/selectorobject.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include <sstream>

using namespace effolkronium;
using namespace std;

constexpr float cMaxZoom = 0.6f;
constexpr float cMinZoom = 1.4f;
constexpr float cWaitTick = 0.1;
constexpr int cRestTicks = 20;

Scene::Scene()
    : m_currentState(eSceneState::Uninitialized)
    , m_pickup(nullptr)
    , m_talking(nullptr)
    , m_currentIdx(-1)
{
}

Scene::~Scene()
{
    for (auto i : m_levels) {
        delete i;
    }
    m_levels.clear();
}

void Scene::setupLevel(PlayerObject* playerObject)
{
    m_currentIdx = 0;
    Level* level = new Level(60, 60);
    Level* level2 = new Level(60, 60);
    level2->generateTown();
    level->generate();
    for (unsigned i = 0; i < 60; i++) {
        (*level)(i, 0).m_type = eTileType::Wall;
    }

    for (unsigned i = 0; i < 60; i++) {
        (*level)(0, i).m_type = eTileType::Wall;
    }
    // level->generateTown();
    playerObject->setStartingDialogueLabel("wakeup");

    m_talking = playerObject;
    m_ui.initDialog(playerObject);
    m_currentState = eSceneState::DialogueMode;
    CampfireObject* temp = new CampfireObject;
    temp->setPosition(2, 1);
    level->addObject(temp);

    playerObject->setPosition(1, 1);
    // has some management of player here but cannot delete
    // violates RAII
    level->addObject(playerObject);

    m_levels.push_back(level);
    m_levels.push_back(level2);
}

void Scene::setupArena(PlayerObject* playerObject)
{
    m_currentIdx = 0;
    Level* level = new Level(10, 10);

    playerObject->setPosition(1, 1);
    // has some management of player here but cannot delete
    // violates RAII
    level->addObject(playerObject);
    FactionClearedTrigger* trigger = new FactionClearedTrigger(Trigger::cPersistentTrigger,
        eCreatureFaction::ArenaFighter, GameEffectManager::eGameEffect::Arena);

    level->addGlobalTrigger(trigger);
    m_currentState = eSceneState::Playing;
    m_levels.push_back(level);
}

void Scene::changeToLevel(int idx, Object* object, int x, int y)
{
    m_levels[m_currentIdx]->removeObject(object->getId());
    object->setPosition(x, y);
    m_levels[idx]->addObject(object);
    if (object->getObjectType() == eObjectTypes::Creature) {
        CreatureObject* creature = static_cast<CreatureObject*>(object);
        if (creature->isPlayer()) {
            m_currentIdx = idx;
        }
    }
}

void Scene::run(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    m_gfxlevel.renderBkg(m_levels[m_currentIdx]);
    // todo: remove all this sfml specific stuff and move to a gfx specific class
    //------------------
    sf::View original = Game::getWindow().getView();
    sf::View v = Game::getWindow().getView();
    v.setSize(v.getSize().x, v.getSize().y * 2);
    // v.setCenter(v.getSize() *.5f);
    sf::Vector2f center(playerObject->getPosition().x, playerObject->getPosition().y);
    center = coordsToScreen(center);
    v.setCenter(center.x, center.y + cViewDiff);
    v.zoom(zoom);
    Game::getWindow().setView(v);
    //------------------
    m_gfxlevel.run(m_levels[m_currentIdx], playerObject->getPosition());
    // temporary until we get graphics queue up and running
    if (m_currentState == eSceneState::AttackMode || m_currentState == eSceneState::SelectionMode
        || m_currentState == eSceneState::DialogueSelect
        || m_currentState == eSceneState::UseMode) {
        m_gfxSelector.run(&m_selector);
    }
    m_gfxlevel.renderText();
    Game::getWindow().setView(original);

    m_ui.run();

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Equal) {
        zoom = max(cMaxZoom, zoom - 0.1f);
    }
    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Hyphen) {
        zoom = min(cMinZoom, zoom + 0.1f);
    }
    switch (m_currentState) {
    case eSceneState::Playing:
        playing(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::SelectionMode:
        selection(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::DialogueSelect:
        dialogSelect(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::DialogueMode:
        dialog(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::AttackMode:
        attack(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::UseMode:
        use(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::Inventory:
        inventory(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::Pickup:
        pickup(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::InCombat:
        combat(hasKeyEvents, event, playerObject);
        break;
    case eSceneState::Waiting:
        wait(hasKeyEvents, event, playerObject);
        break;
    default:
        break;
    }
    if (playerObject->isConscious() == false && m_currentState != eSceneState::Dead) {
        m_currentState = eSceneState::Dead;
        Log::push("You have died!", Log::eMessageTypes::Damage);
    }

    Log::run();
    float currentTime = clock.restart().asSeconds();
    aiTick += currentTime;
    tick += currentTime;
}

void Scene::selection(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    doMoveSelector(hasKeyEvents, event, playerObject, false);
    if (hasKeyEvents
        && (event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonReleased)) {
        if (event.key.code == sf::Keyboard::Enter || event.mouseButton.button == sf::Mouse::Left) {
            const Object* object = m_levels[m_currentIdx]->getObject(m_selector.getPosition());
            if (object != nullptr) {
                Log::push("You see " + object->getDescription());
                if (object->getObjectType() == eObjectTypes::Creature) {
                    const CreatureObject* creatureObj = static_cast<const CreatureObject*>(object);
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
            } else {
                Tile tile = m_levels[m_currentIdx]->get(m_selector.getPosition());
                string str = "Just a ";
                switch (tile.m_material) {
                case eTileMaterial::Stone:
                    str += "stone ";
                    break;
                }
                switch (tile.m_type) {
                case eTileType::Ground:
                    str += "floor.";
                    break;
                case eTileType::Wall:
                    str += "wall. ";
                    break;
                }
                Log::push(str);
            }
        }

        if (event.key.code == sf::Keyboard::Escape) {
            m_currentState = eSceneState::Playing;
        }
    }
}

void Scene::playing(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    vector2d pos = playerObject->getPosition();
    if (hasKeyEvents && event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
            tick = cPlayingTick + 1;
            aiTick = cPlayingTick + 1;
            if (m_levels[m_currentIdx]->isFreeSpace(pos.x, pos.y + 1) == true) {
                playerObject->moveDown();
            }
        }
        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
            tick = cPlayingTick + 1;
            aiTick = cPlayingTick + 1;
            if (m_levels[m_currentIdx]->isFreeSpace(pos.x, pos.y - 1) == true) {
                playerObject->moveUp();
            }
        }
        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
            tick = cPlayingTick + 1;
            aiTick = cPlayingTick + 1;
            if (m_levels[m_currentIdx]->isFreeSpace(pos.x - 1, pos.y) == true) {
                playerObject->moveLeft();
            }
        }
        if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
            tick = cPlayingTick + 1;
            aiTick = cPlayingTick + 1;
            if (m_levels[m_currentIdx]->isFreeSpace(pos.x + 1, pos.y) == true) {
                playerObject->moveRight();
            }
        }
    } else if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Num1) {
            m_selector.setPosition(playerObject->getPosition());
            m_currentState = eSceneState::AttackMode;
        }
        if (event.key.code == sf::Keyboard::I) {
            m_currentState = eSceneState::Inventory;
        }
        if (event.key.code == sf::Keyboard::Num3) {
            m_selector.setPosition(playerObject->getPosition());
            m_currentState = eSceneState::UseMode;
        }
        if (event.key.code == sf::Keyboard::Num2) {
            m_selector.setPosition(playerObject->getPosition());
            m_currentState = eSceneState::SelectionMode;
        }
        if (event.key.code == sf::Keyboard::Num4) {
            Object* object = m_levels[m_currentIdx]->getObjectMutable(
                playerObject->getPosition(), playerObject);
            if (object != nullptr) {
                switch (object->getObjectType()) {
                case eObjectTypes::Corpse:
                    Log::push("Searching corpse...");
                    m_pickup = object;
                    m_currentState = eSceneState::Pickup;
                    break;
                case eObjectTypes::Chest:
                    Log::push("Opening chest...");
                    m_pickup = object;
                    m_currentState = eSceneState::Pickup;
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
        if (event.key.code == sf::Keyboard::Num5) {
            m_selector.setPosition(playerObject->getPosition());
            m_currentState = eSceneState::DialogueSelect;
        }
        if (event.key.code == sf::Keyboard::Num6) {
            Log::push("Resting..");
            m_currentState = eSceneState::Waiting;
        }
    }

    if (playerObject->isInCombat() == true) {
        m_currentState = eSceneState::InCombat;
    }

    if (aiTick > cPlayingTick) {
        m_levels[m_currentIdx]->run(this);
        aiTick = 0;
        m_levels[m_currentIdx]->cleanup();
    }
}

void Scene::dialogSelect(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    doMoveSelector(hasKeyEvents, event, playerObject, true);
    if (hasKeyEvents
        && (event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonReleased)) {
        if (event.key.code == sf::Keyboard::Enter || event.mouseButton.button == sf::Mouse::Left) {
            Object* object
                = m_levels[m_currentIdx]->getObjectMutable(m_selector.getPosition(), playerObject);
            if (object != nullptr) {
                if (object->getObjectType() == eObjectTypes::Creature) {
                    CreatureObject* creatureObject = static_cast<CreatureObject*>(object);
                    if (creatureObject->isConscious() == true) {
                        m_talking = creatureObject;
                        m_ui.initDialog(m_talking);
                        m_currentState = eSceneState::DialogueMode;
                    } else {
                        Log::push("You can't talk to an unconscious creature");
                    }
                }
            }
        }
        if (event.key.code == sf::Keyboard::Escape) {
            m_currentState = eSceneState::Playing;
        }
    }
}

void Scene::inventory(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    m_ui.runInventory(hasKeyEvents, event, playerObject);
    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Escape) {
        m_currentState = eSceneState::Playing;
    }
}

void Scene::use(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    doMoveSelector(hasKeyEvents, event, playerObject, true);
    if (hasKeyEvents
        && (event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonReleased)) {
        if (event.key.code == sf::Keyboard::Enter || event.mouseButton.button == sf::Mouse::Left) {
            Object* object
                = m_levels[m_currentIdx]->getObjectMutable(m_selector.getPosition(), playerObject);
            if (object != nullptr) {
                if (object->getObjectType() == eObjectTypes::Campfire) {
                    Game::getSingleton()->save(Game::cSaveString);
                    Log::push("Game saved!");
                }
            }
        }
        if (event.key.code == sf::Keyboard::Escape) {
            m_currentState = eSceneState::Playing;
        }
    }
}

void Scene::combat(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    m_ui.runCombat(hasKeyEvents, event, playerObject->getCombatManager());

    if (playerObject->runCombat(tick) == false) {
        m_currentState = eSceneState::Playing;
    }
    /*
      if (playerObject->isInCombat() == false) {
      m_currentState = eSceneState::Playing;
      }
    */
    if (tick > CombatManager::cTick) {
        // issue here is if player is engaged and is not the parent. the ai updates so
        // slowly that its hard for the player to understand whats going on
        m_levels[m_currentIdx]->run(this);
        tick = 0;
    }
}

void Scene::dialog(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    assert(m_talking != nullptr);
    if (m_ui.runDialog(hasKeyEvents, event, playerObject, m_talking) == false) {
        m_currentState = eSceneState::Playing;
    }
}

void Scene::pickup(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    assert(m_pickup != nullptr);
    m_ui.runTrade(
        hasKeyEvents, event, playerObject->getInventoryMutable(), m_pickup->getInventoryMutable());
    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Escape) {
        m_currentState = eSceneState::Playing;
    }
}

void Scene::wait(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    if (sleepTick < cRestTicks) {
        // cout << aiTick << endl;
        if (aiTick > cWaitTick) {
            m_levels[m_currentIdx]->run(this);
            aiTick = 0;
            sleepTick++;
            m_levels[m_currentIdx]->cleanup();
            cout << "sleeping" << endl;
        }
    } else {
        Log::push("You recover all fatigue and your minor wounds heal up.");
        playerObject->getCreatureComponent()->resetFatigue();
        playerObject->getCreatureComponent()->healWounds(1);
        sleepTick = 0;
        m_currentState = eSceneState::Playing;
    }
}

PlayerObject* Scene::getPlayer()
{
    // todo : group objs by type
    auto objects = m_levels[m_currentIdx]->getObjects();
    for (auto it : objects) {
        if (it->getObjectType() == eObjectTypes::Creature) {
            CreatureObject* creature = static_cast<CreatureObject*>(it);
            if (creature->isPlayer()) {
                return static_cast<PlayerObject*>(creature);
            }
        }
    }
    return nullptr;
}

void Scene::attack(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject)
{
    doMoveSelector(hasKeyEvents, event, playerObject, true);
    if (hasKeyEvents
        && (event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonReleased)) {
        if (event.key.code == sf::Keyboard::Enter || event.mouseButton.button == sf::Mouse::Left) {
            const Object* object = m_levels[m_currentIdx]->getObject(m_selector.getPosition());
            if (object != nullptr) {
                if (object->getObjectType() == eObjectTypes::Creature) {
                    const CreatureObject* creatureObject
                        = static_cast<const CreatureObject*>(object);
                    if (creatureObject->isConscious() == true) {
                        playerObject->startCombatWith(creatureObject);
                        m_currentState = eSceneState::InCombat;
                    } else {
                        creatureObject->kill();
                        Log::push("You finish off the unconscious creature");
                    }
                }
            }
        }
        if (event.key.code == sf::Keyboard::Escape) {
            m_currentState = eSceneState::Playing;
        }
    }
}

void Scene::doMoveSelector(
    bool hasKeyEvents, sf::Event event, PlayerObject* playerObject, bool limit)
{
    int maxWidth = m_levels[m_currentIdx]->getWidth();
    int maxHeight = m_levels[m_currentIdx]->getHeight();
    constexpr int cLimit = 2;

    // m_selector.setPosition(pos);

    if (event.type == sf::Event::MouseMoved) {
        auto size = Game::getWindow().getSize();
        float x = event.mouseMove.x - (int)size.x / 2;
        float y = event.mouseMove.y - (int)size.y / 2 + cViewDiff / 2;
        x *= zoom;
        y *= zoom;
        sf::Vector2f coords = screenToCoords(sf::Vector2f(x, y));
        vector2d mousePos(
            coords.x + playerObject->getPosition().x, coords.y + playerObject->getPosition().y);

        // cout << "cx: " << coords.x << " cy: " << coords.y << endl;
        // cout << "x: " << mousePos.x << " y: " << mousePos.y << endl;
        if (limit) {
            mousePos.x = max(mousePos.x, playerObject->getPosition().x - cLimit);
            mousePos.y = max(mousePos.y, playerObject->getPosition().y - cLimit);
            mousePos.x = min(mousePos.x, playerObject->getPosition().x + cLimit);
            mousePos.y = min(mousePos.y, playerObject->getPosition().y + cLimit);
        }

        m_selector.setPosition(mousePos.x, mousePos.y);
    }
    if (event.type == sf::Event::KeyReleased && hasKeyEvents) {
        if (event.key.code == sf::Keyboard::Down) {
            if (limit == false
                || m_selector.getPosition().y < playerObject->getPosition().y + cLimit) {
                m_selector.moveDown();
            }
        }
        if (event.key.code == sf::Keyboard::Up) {
            if (limit == false
                || m_selector.getPosition().y > playerObject->getPosition().y - cLimit) {
                m_selector.moveUp();
            }
        }
        if (event.key.code == sf::Keyboard::Left) {
            if (limit == false
                || m_selector.getPosition().x > playerObject->getPosition().x - cLimit) {
                m_selector.moveLeft();
            }
        }
        if (event.key.code == sf::Keyboard::Right) {
            if (limit == false
                || m_selector.getPosition().x < playerObject->getPosition().x + cLimit) {
                m_selector.moveRight();
            }
        }
    }
    vector2d pos = m_selector.getPosition();
    pos.x = max(0, pos.x);
    pos.y = max(0, pos.y);
    pos.x = min(maxWidth - 1, pos.x);
    pos.y = min(maxHeight - 1, pos.y);
    m_selector.setPosition(pos);
}
