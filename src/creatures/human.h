#pragma once

#include "creature.h"

class Human : public Creature {
public:
    Human();
    ~Human() {}
    eCreatureType getCreatureType() override { return eCreatureType::Human; }
    void randomizeStats();

    const Weapon* getNaturalWeapon() const override;

private:
};
