#include "creatureobject.h"
#include "../items/consumable.h"

CreatureObject::CreatureObject(Creature* creature)
    : m_creature(creature)
    , m_creatureFaction(eCreatureFaction::None)
    , m_combatManagerId(-1)
    , m_thirst(0)
    , m_hunger(0)
    , m_exhaustion(0)
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
        switch (it->getType()) {
        case eEffect::Thirst:
            break;
        case eEffect::Hunger:
            break;
        }
    }
}

int CreatureObject::getFatigue() const
{
    return m_creature->getFatigue();
}
