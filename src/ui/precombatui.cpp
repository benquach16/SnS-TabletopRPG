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
    string str = "First Tempo Actions:\na - Confirm\n";

    const Weapon* weapon = player->getPrimaryWeapon();
    if (player->getFavoredLocations().size() == 0) {
        str += "b - Guard Location (1 AP)\n";
    }
    if (weapon->getType() == eWeaponTypes::Polearms) {
        if (player->getGrip() == eGrips::Standard) {
            str += "c - Switch to Staff Grip\n";
        } else {
            str += "c - Switch to Standard Grip\n";
        }
    } else if (weapon->getType() == eWeaponTypes::Longswords) {
        if (player->getGrip() == eGrips::Standard) {
            str += "c - Switch to Halfswording\n";
        } else {
            str += "c - Switch to Standard Grip\n";
        }
    }
    if (player->getHasPosition() == false) {
        str += "d - Attempt to stand (3AP)\n";
        str += "e - Attempt to pick up weapon (3AP)\n";
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
            if (player->getFavoredLocations().size() == 0) {
                m_currentState = eUiState::ChooseFavorLocations;
            }
            break;
        case 'c':
            if (weapon->getType() == eWeaponTypes::Polearms) {
                if (player->getGrip() == eGrips::Standard) {
                    player->setGrip(eGrips::Staff);
                } else {
                    player->setGrip(eGrips::Standard);
                }
            } else if (weapon->getType() == eWeaponTypes::Longswords) {
                if (player->getGrip() == eGrips::Standard) {
                    player->setGrip(eGrips::HalfSword);
                } else {
                    player->setGrip(eGrips::Standard);
                }
            }
            break;
        case 'd':
            if (player->getHasPosition() == false) {
                if (player->getCombatPool() >= 3) {
                    player->attemptStand();
                } else {
                    Log::push("Requires 3 AP");
                }
            }
            break;
        case 'e':
            if (player->getHasPosition() == false) {
                if (player->getCombatPool() >= 3) {
                    player->attemptPickup();
                } else {
                    Log::push("Requires 3 AP");
                }
            }
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
                m_currentState = eUiState::ChooseFavoring;
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}
