#pragma once

#include "human.h"

#include <iostream>

class Player : public Human {
public:
    Player();
    ~Player() {}
    bool isPlayer() override { return true; }

    bool pollForInitiative();
    void setInitiative(eInitiativeRoll initiative);
    eInitiativeRoll getInitiative() { return m_initiative; }

    void clearCreatureManuevers(bool skipDisable = false) override;

private:
    eInitiativeRoll m_initiative;

    // wrap in fence object?
    bool m_flagInitiative;
};
