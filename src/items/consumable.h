#pragma once

#include <set>

#include "effect.h"
#include "item.h"

class Creature;

class Consumable : public Item {
public:
    Consumable(const std::string& name, const std::string& description, int cost, eItemType type,
        std::set<ItemEffect*> effects);
    ~Consumable();
    void apply(Creature* creature, eBodyParts part = eBodyParts::Face) const;

    const std::set<ItemEffect*>& getEffects() const { return m_effects; }

private:
    std::set<ItemEffect*> m_effects;
};
