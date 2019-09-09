#include <fstream>

#include "../3rdparty/json.hpp"
#include "../game.h"
#include "common.h"
#include "createcharui.h"
#include "types.h"

using namespace std;

const string filepath = "data/starting.json";

CreateCharUI::CreateCharUI()
{
    ifstream file(filepath);

    nlohmann::json parsedLoadouts;
    file >> parsedLoadouts;

    for (auto& iter : parsedLoadouts.items()) {
        string key = iter.key();
        auto values = iter.value();

        string description = values["description"];
        vector<int> armor = values["armor"];
        int weapon = values["weapon"];

        StartingLoadouts loadout;
        loadout.name = values["name"];
        loadout.description = description;
        loadout.armor = armor;
        loadout.weapon = weapon;

        m_loadouts.push_back(loadout);
    }

    resetState();
}

void CreateCharUI::run(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    switch (m_currentState) {
    case eUiState::Name:
        doName(hasKeyEvents, event, player);
        break;
    case eUiState::Loadout:
        doLoadout(hasKeyEvents, event, player);
        break;
    case eUiState::Attributes:
        break;
    case eUiState::Proficiencies:
        break;
    default:
        break;
    }
}

bool CreateCharUI::isDone()
{
    return false;
}

void CreateCharUI::doName(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Enter your name (Cannot be blank):");
    Game::getWindow().draw(text);

    m_text.run(hasKeyEvents, event);
    m_text.setPosition(sf::Vector2f(0, cCharSize));

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Enter) {
        string name = m_text.getString();
        if (name != "") {
            player->getCreatureComponent()->setName(name);
            m_currentState = eUiState::Loadout;
        }
    }
}

void CreateCharUI::doLoadout(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    string str;
    for(unsigned i = 0; i < m_loadouts.size(); ++i) {
        char idx = 'a' + i;
        str += idx;
        str += " - " + m_loadouts[i].name + '\n';

        if(hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if(c == idx) {
                
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);    
}
