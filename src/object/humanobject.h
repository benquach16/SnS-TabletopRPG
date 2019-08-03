#pragma once

#include "../creatures/human.h"
#include "creatureobject.h"

class HumanObject : public CreatureObject {
public:
    HumanObject();
    ~HumanObject() override;
    eCreatureRace getRace() const override { return eCreatureRace::Human; }
    std::string getDescription() const override;

private:
};
