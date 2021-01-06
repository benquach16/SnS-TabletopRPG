#pragma once

#include <SFML/Graphics.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

#include "combatinstance.h"
#include "combatmanager.h"
#include "gfxobjects/gfxlevel.h"
#include "gfxobjects/gfxselector.h"
#include "object/playerobject.h"
#include "object/selectorobject.h"
#include "ui/gameui.h"

class PlayerObject;

class Scene {
public:
    friend class boost::serialization::access;

    Scene();
    ~Scene();
    void setupLevel(PlayerObject* playerObject);
    void setupArena(PlayerObject* playerObject);
    void run(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void changeToLevel(int idx, Object* object, int x, int y);

    PlayerObject* getPlayer();

private:
    static constexpr float cPlayingTick = 0.3;
    void playing(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void selection(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void dialogSelect(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void dialog(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void inventory(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void combat(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void wait(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void use(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void pickup(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);
    void attack(bool hasKeyEvents, sf::Event event, PlayerObject* playerObject);

    void doMoveSelector(sf::Event, PlayerObject* playerObject, bool limit);
    enum class eSceneState : unsigned {
        Uninitialized,
        Playing,
        Waiting,
        SelectionMode,
        DialogueSelect,
        DialogueMode,
        AttackMode,
        UseMode,
        Inventory,
        Pickup,
        InCombat,
        PauseMenu,
        Dead,
        Exiting
    };
    eSceneState m_currentState;
    SelectorObject m_selector;
    Object* m_pickup;

    std::vector<Level*> m_levels;
    int m_currentIdx;
    // who i am talking to
    CreatureObject* m_talking;
    GameUI m_ui;
    sf::Clock clock;
    float currentWaitTick;
    float tick = 0;
    float aiTick = 0;
    int sleepTick = 0;
    GFXLevel m_gfxlevel;

    GFXSelector m_gfxSelector;

    float zoom = 1.0f;

    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_levels;
        ar& m_pickup;
        ar& m_currentIdx;
        // make sure we are out of any weird states when saving/loading
        m_currentState = eSceneState::Playing;
    }
};
