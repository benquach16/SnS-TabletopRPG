#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"
#include "gfxobjects/gfxlevel.h"
#include "gfxobjects/gfxselector.h"
#include "object/playerobject.h"
#include "object/selectorobject.h"
#include "ui/gameui.h"

class Scene {
public:
    Scene();
    ~Scene();
    void run();

private:
    void doMoveSelector(sf::Event, bool limit);
    enum class eGameState : unsigned {
        Uninitialized,
        Playing,
        Waiting,
        SelectionMode,
        DialogueSelect,
        DialogueMode,
        AttackMode,
        Inventory,
        Pickup,
        InCombat,
        PauseMenu,
        Dead,
        Exiting
    };

    SelectorObject m_selector;
    Object* m_pickup;
    CreatureObject* m_talking;
    PlayerObject* m_playerObject;
    Level* m_currentLevel;

    sf::Clock clock;
    float tick = 0;
    float aiTick = 0;
    int sleepTick = 0;
    GFXLevel m_gfxlevel;

    GFXSelector m_gfxSelector;

    float zoom = 1.0f;
};
