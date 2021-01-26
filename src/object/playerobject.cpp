#include <algorithm>
#include <iostream>

#include "playerobject.h"

using namespace std;

BOOST_CLASS_EXPORT(PlayerObject)

constexpr unsigned cBleedTicks = 50;

PlayerObject::PlayerObject()
    : CreatureObject(new Player)
    , m_toMove(0.f, 0.f)
    , m_bleedTick(0)
{
    m_inventory[5] = 3;
    m_inventory[1] = 3;
    m_inventory[6] = 3;
}

PlayerObject::~PlayerObject() {}

void PlayerObject::run(const Level* level)
{
    /*
    if (m_manager->isParent() == true) {
        m_manager->run(0.9);
    }
    */

    m_toMove.x = 0;
    m_toMove.y = 0;

    if (m_creature->getBleeding()) {
        // tick bleeding until death
        m_bleedTick++;
        if (m_bleedTick > cBleedTicks) {
            m_creature->bleed();
            Log::push("You stuggle as you keep bleeding from an open wound...", Log::Damage);
            m_bleedTick = 0;
        }
    }
}

// player has special combat manager instance code due to player state in scene
bool PlayerObject::runCombat(float tick)
{
    // we are in a duel but not parent
    if (isInCombat() == true && m_manager->isParent() == false) {
        return true;
    }
    if (isInCombat() == false) {
        m_creature->clearCreatureManuevers();
    }
    bool ret = m_manager->run(tick);
    if (ret == false) {
        setOutofCombat();
    }
    return ret;
}

void PlayerObject::moveDown() { m_position.y++; }

void PlayerObject::moveUp() { m_position.y--; }

void PlayerObject::moveLeft() { m_position.x--; }
void PlayerObject::moveRight() { m_position.x++; }
