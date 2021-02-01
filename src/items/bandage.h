#pragma once

#include "item.h"
#include "creatures/types.h"

class Creature;

class Bandage : public Item {
public:
    Bandage(const std::string& name, const std::string& description, int cost);

    void apply(Creature* creature, eBodyParts part);
};
