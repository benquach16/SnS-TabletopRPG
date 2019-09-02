#pragma once

#include <vector>

#include "numberinput.h"
#include "textinput.h"

class CreateCharUI {
    struct Loadout {
        std::string name;
        std::string description;
        std::vector<int> armor;
        int weapon;
    };

public:
    CreateCharUI();
    void run();

private:
    void doName();
};
