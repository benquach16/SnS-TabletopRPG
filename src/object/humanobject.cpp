#include <iostream>

#include "../3rdparty/random.hpp"
#include "equipmentmanager.h"
#include "humanobject.h"
#include "utils.h"

using namespace std;

HumanObject::HumanObject()
    : CreatureObject(new Human)
{
    // m_creature->setWeapon(1041); //arming sword
    m_creature->setName(EquipmentManager::getSingleton()->getRandomName(eCreatureRace::Human));
}

HumanObject::~HumanObject() {}

std::string HumanObject::getDescription() const
{
    std::string ret;
    ret += m_creature->getName() + ", human of the " + factionToString(m_creatureFaction)
        + " armed with " + m_creature->getPrimaryWeapon()->getName() + ", wearing ";

    for (auto it : m_creature->getArmor()) {
        ret += it->getName() + ", ";
    }

    return ret;
}
