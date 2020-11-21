#pragma once

#include "human.h"

#include <iostream>

class Player : public Human {
public:
    Player();
    ~Player() {}
    bool isPlayer() override { return true; }
};
