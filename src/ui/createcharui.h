#pragma once

#include <vector>

#include "numberinput.h"
#include "textinput.h"

class PlayerObject;

class CreateCharUI {
public:
    CreateCharUI();
    void run(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void resetState() { m_currentState = eUiState::Name; }
    bool isDone();

private:
    struct StartingLoadouts {
        std::string name;
        std::string description;
        std::vector<int> armor;
        int weapon;
    };

    enum eUiState { Name, Loadout, Description, Attributes, Proficiencies, Finished };
    void doName(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doLoadout(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    TextInput m_text;

    eUiState m_currentState;

    std::vector<StartingLoadouts> m_loadouts;
};
