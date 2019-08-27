#include "precombatui.h"
#include "../creatures/utils.h"
#include "../game.h"
#include "common.h"
#include "types.h"

using namespace std;

void PrecombatUI::run(bool hasKeyEvents, sf::Event event, Player* player)
{
    switch (m_currentState) {
    case eUiState::ChooseFavoring:
        doFavoring(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseFavorLocations:
        doFavorLocation(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseGrip:
        doChooseGrip(hasKeyEvents, event, player);
        break;
    case eUiState::Finished:
        break;
    }
}

void PrecombatUI::doFavoring(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    string str = "Favor location (1AP)?\na - Skip all pre-exchange actions\nb - Yes\nc - No";

    if (player->getHasPosition() == false) {
        str += "\nd - Attempt to stand";
        str += "\ne - Attempt to pick up weapon";
    }
    text.setString(str);

    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            player->setPrecombatReady();
            m_currentState = eUiState::Finished;
            break;
        case 'b':
            m_currentState = eUiState::ChooseFavorLocations;
            break;
        case 'c':
            m_currentState = eUiState::ChooseGrip;
            break;
        case 'd':
            player->setPositionReady();
            break;
        case 'e':
            player->setPositionReady();
            break;
        }
    }
}

void PrecombatUI::doFavorLocation(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    std::string str = "Choose location:\n";

    const std::vector<eHitLocations> locations = player->getHitLocations();
    for (int i = 0; i < locations.size(); ++i) {
        char idx = ('a' + i);

        str += idx;
        str += " - " + hitLocationToString(locations[i]) + '\n';

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                player->reduceCombatPool(1);
                player->addFavored(locations[i]);
                m_currentState = eUiState::ChooseGrip;
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}

void PrecombatUI::doChooseGrip(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    const Weapon* weapon = player->getPrimaryWeapon();
    if (weapon->getType() == eWeaponTypes::Polearms) {
        text.setString(
            "Switch Grip?\na - Standard\nb - Staff Grip\nc - Reverse (Tail-end forward)");
        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            switch (c) {
            case 'a':
                player->setGrip(eGrips::Standard);
                player->setPrecombatReady();
                m_currentState = eUiState::Finished;
                break;
            case 'b':
                player->setGrip(eGrips::Staff);
                player->setPrecombatReady();
                m_currentState = eUiState::Finished;
                break;
            case 'c':
                player->setGrip(eGrips::Overhand);
                player->setPrecombatReady();
                m_currentState = eUiState::Finished;
                break;
            }
        }
    } else if (weapon->getType() == eWeaponTypes::Longswords
        || weapon->getType() == eWeaponTypes::Swords) {
        text.setString("Switch Grip?\na - Standard\nb - Half Sword");
        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            switch (c) {
            case 'a':
                player->setGrip(eGrips::Standard);
                player->setPrecombatReady();
                m_currentState = eUiState::Finished;
                break;
            case 'b':
                player->setGrip(eGrips::HalfSword);
                player->setPrecombatReady();
                m_currentState = eUiState::Finished;
                break;
            }
        }
    } else {
        player->setPrecombatReady();
        m_currentState = eUiState::Finished;
    }
    Game::getWindow().draw(text);
}
