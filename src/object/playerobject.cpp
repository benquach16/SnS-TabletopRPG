#include <iostream>

#include "playerobject.h"

PlayerObject::PlayerObject()
    : CreatureObject(new Player)
{
    m_creature->setWeapon(1040);
    m_creature->equipArmor(2055);
    m_creature->equipArmor(2052);
    m_creature->equipArmor(2044);
    m_creature->equipArmor(2040);
    m_creature->equipArmor(2046);
    m_creature->equipArmor(2043);
    m_creature->equipArmor(2056);
    // m_creature->equipArmor(2057);
    m_creature->setName("John");

    m_inventory[0] = 50;
    m_inventory[1] = 5;
    m_inventory[2] = 1;
    m_inventory[3] = 1;
    m_inventory[4] = 1;
    m_inventory[5] = 3;
}

PlayerObject::~PlayerObject()
{
    // temp
    delete m_manager;
}

bool PlayerObject::runCombat(float tick)
{
    bool ret = m_manager->run(tick);
    if (ret == false) {
        setOutofCombat();
    }
    return ret;
}
