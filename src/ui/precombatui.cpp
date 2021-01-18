#include "precombatui.h"
#include "common.h"
#include "creatures/utils.h"
#include "game.h"
#include "items/utils.h"
#include "types.h"

using namespace std;

PrecombatUI::PrecombatUI()
    : m_currentPlayer(nullptr)
    , m_inGrapple(false)
{
}

void PrecombatUI::run(
    bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple, bool secondExchange)
{
    m_currentPlayer = player;
    m_inGrapple = inGrapple;
    switch (m_currentState) {
    case eUiState::ChooseFavoring:
        m_window->Show();
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

void PrecombatUI::initialize()
{
    auto windowSize = Game::getWindow().getSize();
    m_window = sfg::Window::Create();
    auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10);
    m_window->Add(box);
    m_window->SetStyle(m_window->GetStyle() ^ sfg::Window::RESIZE);
    m_window->SetStyle(m_window->GetStyle() ^ sfg::Window::TITLEBAR);
    box->Pack(sfg::Label::Create("First Tempo Actions"));
    auto quickdrawBtn = sfg::Button::Create("Quickdraw Weapon");
    auto info = sfg::Label::Create("");
    info->SetLineWrap(true);
    info->SetRequisition(sf::Vector2f(windowSize.x, 0));
    quickdrawBtn->GetSignal(sfg::Button::OnMouseEnter).Connect([this, info] {
        info->SetText("Quickly draw a weapon you have equipped on your belt, to use in the same "
                      "tempo. Costs AP");
    });
    quickdrawBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        auto weaponWindow = sfg::Window::Create();
        auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10);
        weaponWindow->Add(box);
        std::vector<int> quickDrawIds = m_currentPlayer->getQuickdrawItems();
        for (int i = 0; i < quickDrawIds.size(); ++i) {
            int cost = 1;
            const Weapon* weapon = WeaponTable::getSingleton()->get(quickDrawIds[i]);
            cost += calculateReachCost(weapon->getLength(), eLength::Hand) / 2;
            string str = weapon->getName() + " (" + to_string(cost) + " AP)\n";
            auto weaponBtn = sfg::Button::Create(str);
            box->Pack(sfg::Label::Create("Weapons on belt:"));
            box->Pack(weaponBtn);
            int id = quickDrawIds[i];
            weaponBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, cost, id] {
                const Weapon* weapon = WeaponTable::getSingleton()->get(id);
                if (m_currentPlayer->getCombatPool() > cost) {
                    if (weapon->getLength() > eLength::Hand && m_inGrapple) {
                        Log::push("Weapon is too long to use in a grapple! You must use a dagger "
                                  "or other Hand length weapon");
                    } else {
                        m_currentPlayer->reduceCombatPool(cost);
                        m_currentPlayer->dropWeapon();
                        m_currentPlayer->setPrimaryWeapon(id);
                        m_currentPlayer->removeQuickdrawItem(id);
                        Log::push("Dropped current weapon and quickdrew " + weapon->getName());
                        hide();
                    }
                } else {
                    Log::push("You do not have enough AP for that!");
                }
            });
        }
        auto closeButton = sfg::Button::Create("Close");
        box->Pack(closeButton);
        closeButton->GetSignal(sfg::Button::OnLeftClick).Connect([this] { hide(); });
        Game::getSingleton()->getDesktop().Add(weaponWindow);
        m_tmpWindows.push_back(weaponWindow);
    });
    box->Pack(quickdrawBtn);
    m_guardBtn = sfg::Button::Create("Guard Location (1 AP)");
    box->Pack(m_guardBtn);
    auto gripBtn = sfg::Button::Create("Switch Grips");
    box->Pack(gripBtn);
    m_positioningBtn = sfg::Button::Create("Attempt to Stand (3 AP)");
    m_positioningBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        if (m_currentPlayer->getCombatPool() >= 3) {
            m_currentPlayer->attemptStand();
        } else {
            Log::push("Requires 3 AP");
        }
    });
    box->Pack(m_positioningBtn);
    auto confirmBtn = sfg::Button::Create("Confirm");
    confirmBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        hide();
        m_currentPlayer->setPrecombatReady();
        m_currentState = eUiState::Finished;
    });
    box->Pack(confirmBtn);

    box->Pack(info);
    Game::getSingleton()->getDesktop().Add(m_window);
    hide();
}

void PrecombatUI::hide()
{
    m_window->Show(false);
    for (auto ptr : m_tmpWindows) {
        Game::getSingleton()->getDesktop().Remove(ptr);
    }
    m_tmpWindows.clear();
}

void PrecombatUI::doFavoring(
    bool hasKeyEvents, sf::Event event, Player* player, bool secondExchange)
{
    UiCommon::drawTopPanel();
    m_currentPlayer = player;
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    string str;
    if (secondExchange) {
        str += "Second Tempo Actions:\na - Confirm\n";
    } else {
        str += "First Tempo Actions:\na - Confirm\n";
    }

    const Weapon* weapon = player->getPrimaryWeapon();
    int cost = getGripChangeCost(secondExchange);
    str += "b - Quickdraw Weapon\n";
    if (player->getFavoredLocations().size() == 0 && secondExchange == false) {
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

void PrecombatUI::doQuickdraw(bool hasKeyEvents, sf::Event event, Player* player, bool inGrapple)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

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
