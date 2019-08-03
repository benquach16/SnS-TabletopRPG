#pragma once

#include <iostream>
#include <map>
#include <string>

#include "object.h"
#include "types.h"

class CorpseObject : public Object {
public:
    CorpseObject(const std::string& name);

    std::string getName() const;
    std::string getDescription() const override;
    eObjectTypes getObjectType() const override { return eObjectTypes::Corpse; }
    void setInventory(const std::map<int, int>& inventory) { m_inventory = inventory; }

protected:
    std::string m_name;
    eCreatureRace m_race;
};
