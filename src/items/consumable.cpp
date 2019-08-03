#include "consumable.h"

Consumable::Consumable(const std::string& name, const std::string& description, int cost, eItemType type, std::set<Effect*> effects)
    : Item(name, description, cost, type)
    , m_effects(effects)
{
}

Consumable::~Consumable()
{
    for (auto it : m_effects) {
        delete it;
    }
    m_effects.clear();
}

void Consumable::apply()
{
}
