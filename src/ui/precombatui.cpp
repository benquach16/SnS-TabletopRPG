#include "precombatui.h"
#include "creatures/utils.h"
#include "game.h"
#include "items/utils.h"
#include "common.h"
#include "types.h"

using namespace std;

void PrecombatUI::run(
    bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple, bool secondExchange)
{
    switch (m_currentState) {
    case eUiState::ChooseFavoring:
        doFavoring(hasKeyEvents, event, player, secondExchange);
        break;
    case eUiState::ChooseFavorLocations:
        doFavorLocation(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseQuickdraw:
        doQuickdraw(hasKeyEvents, event, player, inGrapple);
        break;
    case eUiState::Finished:
        break;
    }
}

void PrecombatUI::doFavoring(
    bool hasKeyEvents, sf::Event event, Player* player, bool secondExchange)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    UiCommon::initializeText(text);
    string str;
    if (secondExchange) {
        str += "Second Tempo Actions:\na - Confirm\n";
    } else {
        str += "First Tempo Actions:\na - Confirm\n";
    }

    const Weapon* weapon = player->getPrimaryWeapon();
    int cost = getGripChangeCost(secondExchange);
    str += "b - Quickdraw Weapon\n";
    if (player->canFavor() && secondExchange == false) {
        str += "c - Guard Location (1 AP)\n";
    }
    if (weapon->getType() == eWeaponTypes::Polearms) {
        if (player->getGrip() == eGrips::Standard) {
            str += "d - Switch to Staff Grip";
        } else {
            str += "d - Switch to Standard Grip";
        }
        if (cost > 0) {
            str += " (" + to_string(cost) + " AP)";
        }
        str += "\n";
    } else if (weapon->getType() == eWeaponTypes::Longswords) {
        if (player->getGrip() == eGrips::Standard) {
            str += "d - Switch to Halfswording";
        } else {
            str += "d - Switch to Standard Grip";
        }
        if (cost > 0) {
            str += " (" + to_string(cost) + " AP)";
        }
        str += "\n";
    }
    if (player->getHasPosition() == false) {
        str += "e - Attempt to stand (3AP)\n";
        str += "f - Attempt to pick up weapon (3AP)\n";
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
            m_currentState = eUiState::ChooseQuickdraw;
            break;
        case 'c':
            if (player->getFavoredLocations().size() == 0) {
                m_currentState = eUiState::ChooseFavorLocations;
            }
            break;
        case 'd':
            if (player->getCombatPool() > cost) {
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
                player->reduceCombatPool(cost);
            } else {
                Log::push("Requires " + to_string(cost) + " AP");
            }

            break;
        case 'e':
            if (player->getHasPosition() == false) {
                if (player->getCombatPool() >= 3) {
                    player->attemptStand();
                } else {
                    Log::push("Requires 3 AP");
                }
            }
            break;
        case 'f':
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
    UiCommon::initializeText(text);

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

void PrecombatUI::doQuickdraw(bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    UiCommon::initializeText(text);

    std::string str = "Quickdraw Weapon (Automatically drops current weapon)\n";
    str += "a - Cancel\n";

    std::vector<int> quickDrawIds = player->getQuickdrawItems();
    for (int i = 0; i < quickDrawIds.size(); ++i) {
        char idx = ('b' + i);

        str += idx;
        int cost = 1;
        const Weapon* weapon = WeaponTable::getSingleton()->get(quickDrawIds[i]);
        cost += calculateReachCost(weapon->getLength(), eLength::Hand) / 2;
        str += " - " + weapon->getName() + " (" + to_string(cost) + " AP)\n";

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == 'a') {
                m_currentState = eUiState::ChooseFavoring;
            } else if (c == idx) {
                if (player->getCombatPool() > cost) {
                    if (weapon->getLength() > eLength::Hand && inGrapple) {
                        Log::push("Weapon is too long to use in a grapple! You must use a dagger "
                                  "or other Hand length weapon");
                    } else {
                        player->reduceCombatPool(cost);
                        player->dropWeapon();
                        player->setPrimaryWeapon(quickDrawIds[i]);
                        player->removeQuickdrawItem(quickDrawIds[i]);
                        m_currentState = eUiState::ChooseFavoring;
                    }
                } else {
                    Log::push("You do not have enough AP for that!");
                }
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}
