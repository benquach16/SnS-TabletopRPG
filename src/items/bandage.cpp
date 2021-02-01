#include "bandage.h"
#include "creatures/creature.h"

Bandage::Bandage(const std::string& name, const std::string& description, int cost)
    : Item(name, description, cost, eItemType::Bandage)
{
}

void Bandage::apply(Creature* creature, eBodyParts part) 
{
    creature->healBleed(part);
}
