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
        std::vector<int> inventory;
        int weapon;
    };

    enum eUiState { Name, Loadout, Description, Attributes, Proficiencies, Finished };
    void doName(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doLoadout(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doDescription(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doAttributes(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doProficiencies(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    TextInput m_text;

    eUiState m_currentState;

    unsigned m_loadoutIdx;
    unsigned m_pointsLeft;

    std::vector<StartingLoadouts> m_loadouts;
};
