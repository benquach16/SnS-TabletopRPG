#pragma once

#include <set>

#include "effect.h"
#include "item.h"

class Consumable : public Item {
public:
    Consumable(const std::string& name, const std::string& description, int cost, eItemType type, std::set<Effect*> effects);
    ~Consumable();
    void apply();

    const std::set<Effect*>& getEffects() const { return m_effects; }

private:
    std::set<Effect*> m_effects;
};
