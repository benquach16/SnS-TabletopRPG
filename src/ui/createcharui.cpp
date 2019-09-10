#include <fstream>

#include "../3rdparty/json.hpp"
#include "../game.h"
#include "common.h"
#include "createcharui.h"
#include "types.h"

using namespace std;

const string filepath = "data/starting.json";

CreateCharUI::CreateCharUI()
    : m_loadoutIdx(-1)
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
    case eUiState::Description:
        doDescription(hasKeyEvents, event, player);
        break;
    case eUiState::Attributes:
        doAttributes(hasKeyEvents, event, player);
        break;
    case eUiState::Proficiencies:
        break;
    default:
        break;
    }
}

bool CreateCharUI::isDone() { return (m_currentState == eUiState::Finished); }

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
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, windowSize.y));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    string str;
    for (unsigned i = 0; i < m_loadouts.size(); ++i) {
        char idx = 'a' + i;
        str += idx;
        str += " - " + m_loadouts[i].name + '\n';

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                m_loadoutIdx = i;
                m_currentState = eUiState::Description;
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}

void CreateCharUI::doDescription(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, windowSize.y));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    string str;
    str = m_loadouts[m_loadoutIdx].description;
    str += "\n\nEnter - Choose class ESC - Go back";
    text.setString(str);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) == true) {
        m_currentState = eUiState::Attributes;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) == true) {
        m_currentState = eUiState::Loadout;
    }
    Game::getWindow().draw(text);
}

void CreateCharUI::doAttributes(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, windowSize.y));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("a - Brawn\nb - Agility\nc - Cunning\nd - Perception\ne - "
                   "Will\n\nGrit\nKeen\nReflex\nSpeed\n");

    Game::getWindow().draw(text);
}

void CreateCharUI::doProficiencies(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, windowSize.y));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);
}
