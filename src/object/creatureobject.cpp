#include "creatureobject.h"
#include "../items/consumable.h"

CreatureObject::CreatureObject(Creature* creature)
    : m_creature(creature)
    , m_creatureFaction(eCreatureFaction::None)
    , m_combatManagerId(-1)
{
}

CreatureObject::~CreatureObject()
{
    if (m_creature != nullptr) {
        delete m_creature;
        m_creature = nullptr;
    }
}

void CreatureObject::run(const Level* level)
{
    m_controller.run(level, this);
}

void CreatureObject::applyItem(int id)
{
    const Item* item = ItemTable::getSingleton()->get(id);
    assert(item->getItemType() != eItemType::Weapon);
    assert(item->getItemType() != eItemType::Armor);
    const Consumable* consumable = static_cast<const Consumable*>(item);

    for (auto it : consumable->getEffects()) {
    }
}
