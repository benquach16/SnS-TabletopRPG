#pragma once

#include <vector>

#include "numberinput.h"
#include "textinput.h"

class PlayerObject;

class CreateCharUI {
    struct Loadout {
        std::string name;
        std::string description;
        std::vector<int> armor;
        int weapon;
    };

public:
    CreateCharUI();
    void run(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void resetState() { m_currentState = eUiState::Name; }

private:
    enum eUiState { Name, Loadout, Attributes, Proficiencies, Finished };
    void doName(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    TextInput m_text;

    eUiState m_currentState;
};
