#include <iostream>

#include "playerobject.h"

PlayerObject::PlayerObject()
    : CreatureObject(new Player)
{
    /*
    m_creature->setWeapon(1040);
    // m_creature->equipArmor(2055);
    // m_creature->equipArmor(2052);
    m_creature->equipArmor(2044);
    m_creature->equipArmor(2040);
    m_creature->equipArmor(2046);
    // m_creature->equipArmor(2043);
    // m_creature->equipArmor(2056);
    // m_creature->equipArmor(2057);
    m_creature->setName("John");

    m_inventory[0] = 50;
    m_inventory[1] = 5;
    m_inventory[2] = 1;
    m_inventory[3] = 1;
    m_inventory[4] = 1;
    m_inventory[5] = 3;
    m_inventory[6] = 1;
    */
}

PlayerObject::~PlayerObject() {}

void PlayerObject::run(const Level* level)
{
    /*
    if (m_manager->isParent() == true) {
        m_manager->run(0.9);
    }
    */
}

bool PlayerObject::runCombat(float tick)
{
    // we are in a duel but not parent
    if (isInCombat() == true && m_manager->isParent() == false) {
        return true;
    }
    bool ret = m_manager->run(tick);
    if (ret == false) {
        setOutofCombat();
    }
    return ret;
}
