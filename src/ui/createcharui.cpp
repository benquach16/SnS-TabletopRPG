#include <fstream>
#include <iostream>

#include "3rdparty/json.hpp"
#include "common.h"
#include "createcharui.h"
#include "game.h"
#include "items/utils.h"
#include "types.h"

using namespace std;

const string filepath = "data/starting.json";

constexpr unsigned cAttributes = 20;
constexpr unsigned cProficiencies = 20;
constexpr unsigned cMax = 7;
constexpr unsigned cMaxProficiency = 12;

CreateCharUI::CreateCharUI()
    : m_loadoutIdx(-1)
    , m_pointsLeft(cAttributes)
    , m_profLeft(cProficiencies)
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
        vector<int> quickdraw = values["quickdraw"];
        vector<int> inventory = values["inventory"];
        StartingLoadouts loadout;
        loadout.name = values["name"];
        loadout.description = description;
        loadout.armor = armor;
        loadout.weapon = weapon;
        loadout.quickdraw = quickdraw;
        loadout.inventory = inventory;
        m_loadouts.push_back(loadout);
    }

    resetState();
}

void CreateCharUI::initialize(PlayerObject* player)
{
    Creature* creature = player->getCreatureComponent();
    m_nameWindow = sfg::Window::Create();
    m_nameWindow->SetStyle(m_nameWindow->GetStyle() ^ sfg::Window::RESIZE);
    m_nameWindow->SetStyle(m_nameWindow->GetStyle() ^ sfg::Window::TITLEBAR);
    m_entry = sfg::Entry::Create();
    auto nameBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5);
    auto confirmBtn = sfg::Button::Create("Confirm Name");
    confirmBtn->SetRequisition(sf::Vector2f(100, 40));
    m_nameWindow->Add(nameBox);
    nameBox->Pack(sfg::Label::Create("Enter Name (Can't be empty):"));
    nameBox->Pack(m_entry);
    nameBox->Pack(confirmBtn);
    m_nameWindow->SetRequisition(sf::Vector2f(300, 120));

    Game::getSingleton()->getDesktop().Add(m_nameWindow);

    confirmBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, player] {
        std::string name = m_entry->GetText();
        if (name.size() != 0) {
            player->setName(name);
            m_currentState = eUiState::Loadout;
            m_nameWindow->Show(false);
        }
    });

    m_loadoutWindow = sfg::Window::Create();
    m_loadoutWindow->SetStyle(m_loadoutWindow->GetStyle() ^ sfg::Window::RESIZE);
    m_loadoutWindow->SetStyle(m_loadoutWindow->GetStyle() ^ sfg::Window::TITLEBAR);

    auto loadoutBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10);
    m_loadoutWindow->SetAllocation(
        sf::FloatRect(0, 0, Game::getWindow().getSize().x, Game::getWindow().getSize().y));
    m_loadoutWindow->Add(loadoutBox);
    loadoutBox->Pack(sfg::Label::Create("Choose starting background:"));
    for (unsigned i = 0; i < m_loadouts.size(); ++i) {
        auto button = sfg::Button::Create(m_loadouts[i].name);
        loadoutBox->Pack(button);
        button->GetSignal(sfg::Button::OnLeftClick).Connect([this, player, i] {
            auto descWindow = sfg::Window::Create();
            auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5);
            descWindow->Add(box);
            auto label = sfg::Label::Create(m_loadouts[i].description);
            label->SetLineWrap(true);
            label->SetRequisition(sf::Vector2f(400, 40));
            box->Pack(label);
            auto selectBtn = sfg::Button::Create("Select");
            auto retBtn = sfg::Button::Create("Return");
            box->Pack(selectBtn, false, false);
            box->Pack(retBtn, false, false);
            clearTmpWindows();
            retBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] { clearTmpWindows(); });
            selectBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, player, i] {
                player->getCreatureComponent()->setPrimaryWeapon(m_loadouts[i].weapon);
                player->addItem(m_loadouts[i].weapon);
                for (auto item : m_loadouts[i].armor) {
                    player->getCreatureComponent()->equipArmor(item);
                    player->addItem(item);
                }
                for (auto item : m_loadouts[i].quickdraw) {
                    player->getCreatureComponent()->addQuickdrawItem(item);
                    player->addItem(item);
                }
                for (auto item : m_loadouts[i].inventory) {
                    player->addItem(item);
                }
                hide();
                m_currentState = eUiState::Attributes;
                clearTmpWindows();
            });
            descWindow->SetPosition(sf::Vector2f(
                Game::getWindow().getSize().x / 2 - descWindow->GetClientRect().width / 2,
                Game::getWindow().getSize().y / 2 - descWindow->GetClientRect().height / 2));
            Game::getSingleton()->getDesktop().Add(descWindow);
            m_tempWindows.push_back(descWindow);
        });
    }

    Game::getSingleton()->getDesktop().Add(m_loadoutWindow);

    m_statsWindow = sfg::Window::Create();
    m_statsWindow->SetRequisition(sf::Vector2f(600, 500));
    auto statsBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10);

    auto table = sfg::Table::Create();
    statsBox->Pack(table);
    m_statsWindow->Add(statsBox);
    table->Attach(sfg::Label::Create("Attributes"), sf::Rect<sf::Uint32>(0, 0, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    m_attrLeftLabel = sfg::Label::Create("Points Left: " + to_string(m_pointsLeft));
    table->Attach(m_attrLeftLabel, sf::Rect<sf::Uint32>(0, 1, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    m_strLabel = sfg::Label::Create("Strength: ");

    table->Attach(m_strLabel, sf::Rect<sf::Uint32>(0, 2, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    auto strIncrBtn = sfg::Button::Create("+");
    strIncrBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (m_pointsLeft > 0 && creature->getStrength() < cMax) {
            creature->setStrength(creature->getStrength() + 1);
            m_pointsLeft -= 1;
        }
    });
    auto strDecBtn = sfg::Button::Create("-");
    strDecBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (creature->getStrength() > 1) {
            creature->setStrength(creature->getStrength() - 1);
            m_pointsLeft += 1;
        }
    });

    table->Attach(strIncrBtn, sf::Rect<sf::Uint32>(1, 2, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    table->Attach(strDecBtn, sf::Rect<sf::Uint32>(2, 2, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    m_agiLabel = sfg::Label::Create("Agility: ");
    table->Attach(m_agiLabel, sf::Rect<sf::Uint32>(0, 3, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    auto agiIncrBtn = sfg::Button::Create("+");
    agiIncrBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (m_pointsLeft > 0 && creature->getAgility() < cMax) {
            creature->setAgility(creature->getAgility() + 1);
            m_pointsLeft -= 1;
        }
    });
    auto agiDecBtn = sfg::Button::Create("-");
    agiDecBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (creature->getAgility() > 1) {
            creature->setAgility(creature->getAgility() - 1);
            m_pointsLeft += 1;
        }
    });
    table->Attach(agiIncrBtn, sf::Rect<sf::Uint32>(1, 3, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    table->Attach(agiDecBtn, sf::Rect<sf::Uint32>(2, 3, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    m_intLabel = sfg::Label::Create("Intuition: ");
    table->Attach(m_intLabel, sf::Rect<sf::Uint32>(0, 4, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    auto intIncrBtn = sfg::Button::Create("+");
    intIncrBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (m_pointsLeft > 0 && creature->getIntuition() < cMax) {
            creature->setIntuition(creature->getIntuition() + 1);
            m_pointsLeft -= 1;
        }
    });
    auto intDecBtn = sfg::Button::Create("-");
    intDecBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (creature->getIntuition() > 1) {
            creature->setIntuition(creature->getIntuition() - 1);
            m_pointsLeft += 1;
        }
    });
    table->Attach(intIncrBtn, sf::Rect<sf::Uint32>(1, 4, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    table->Attach(intDecBtn, sf::Rect<sf::Uint32>(2, 4, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    m_perLabel = sfg::Label::Create("Perception: ");
    table->Attach(m_perLabel, sf::Rect<sf::Uint32>(0, 5, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    auto perIncrBtn = sfg::Button::Create("+");
    perIncrBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (m_pointsLeft > 0 && creature->getPerception() < cMax) {
            creature->setPerception(creature->getPerception() + 1);
            m_pointsLeft -= 1;
        }
    });
    auto perDecBtn = sfg::Button::Create("-");
    perDecBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (creature->getPerception() > 1) {
            creature->setPerception(creature->getPerception() - 1);
            m_pointsLeft += 1;
        }
    });
    table->Attach(perIncrBtn, sf::Rect<sf::Uint32>(1, 5, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    table->Attach(perDecBtn, sf::Rect<sf::Uint32>(2, 5, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    m_wilLabel = sfg::Label::Create("Willpower: ");
    table->Attach(m_wilLabel, sf::Rect<sf::Uint32>(0, 6, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    auto wilIncrBtn = sfg::Button::Create("+");
    wilIncrBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (m_pointsLeft > 0 && creature->getWillpower() < cMax) {
            creature->setWillpower(creature->getWillpower() + 1);
            m_pointsLeft -= 1;
        }
    });
    auto wilDecBtn = sfg::Button::Create("-");
    wilDecBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature] {
        if (creature->getWillpower() > 1) {
            creature->setWillpower(creature->getWillpower() - 1);
            m_pointsLeft += 1;
        }
    });
    table->Attach(wilIncrBtn, sf::Rect<sf::Uint32>(1, 6, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    table->Attach(wilDecBtn, sf::Rect<sf::Uint32>(2, 6, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    table->Attach(sfg::Label::Create("Proficiencies"), sf::Rect<sf::Uint32>(3, 0, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    m_profLeftLabel = sfg::Label::Create("Points Left: ");
    table->Attach(m_profLeftLabel, sf::Rect<sf::Uint32>(3, 1, 1, 1),
        sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

    for (unsigned i = 0; i < static_cast<unsigned>(eWeaponTypes::Count); ++i) {
        m_profLabels[i] = sfg::Label::Create();
        eWeaponTypes type = static_cast<eWeaponTypes>(i);
        table->Attach(m_profLabels[i], sf::Rect<sf::Uint32>(3, 2 + i, 1, 1),
            sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));

        auto incrBtn = sfg::Button::Create("+");
        incrBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature, type] {
            unsigned lvl = creature->getProficiency(type);
            if (lvl < cMaxProficiency && m_profLeft > 0) {
                creature->setProficiency(type, lvl + 1);
                m_profLeft--;
            }
        });
        auto decBtn = sfg::Button::Create("-");
        decBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this, creature, type] {
            unsigned lvl = creature->getProficiency(type);
            if (lvl > 1) {
                creature->setProficiency(type, lvl - 1);
                m_profLeft++;
            }
        });
        table->Attach(incrBtn, sf::Rect<sf::Uint32>(4, 2 + i, 1, 1),
            sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
        table->Attach(decBtn, sf::Rect<sf::Uint32>(5, 2 + i, 1, 1),
            sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
    }

    m_gritLabel = sfg::Label::Create("Grit (Str + Will)/2: ");
    m_shrewdLabel = sfg::Label::Create("Shrewdness (Int + Per)/2: ");
    m_reflexLabel = sfg::Label::Create("Reflex (Agi + Int)/2: ");
    m_mobLabel = sfg::Label::Create("Mobility (Str + Agi)/2: ");
    statsBox->Pack(m_gritLabel);
    statsBox->Pack(m_shrewdLabel);
    statsBox->Pack(m_reflexLabel);
    statsBox->Pack(m_mobLabel);
    auto finishBtn = sfg::Button::Create("Confirm");
    finishBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        Game::getSingleton()->setState(Game::eApplicationState::Gameplay);
        hide();
    });
    statsBox->Pack(finishBtn, false, false);
    m_statsWindow->SetStyle(m_statsWindow->GetStyle() ^ sfg::Window::TITLEBAR);
    Game::getSingleton()->getDesktop().Add(m_statsWindow);
    hide();
}

void CreateCharUI::clearTmpWindows()
{
    for (int i = 0; i < m_tempWindows.size(); ++i) {
        Game::getSingleton()->getDesktop().Remove(m_tempWindows[i]);
        m_tempWindows.erase(m_tempWindows.begin() + i);
    }
}

void CreateCharUI::hide()
{
    m_nameWindow->Show(false);
    m_loadoutWindow->Show(false);
    m_statsWindow->Show(false);
}

void CreateCharUI::run(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    m_nameWindow->SetPosition(
        sf::Vector2f(Game::getWindow().getSize().x / 2 - m_nameWindow->GetClientRect().width / 2,
            Game::getWindow().getSize().y / 2 - m_nameWindow->GetClientRect().height / 2));
    m_loadoutWindow->SetPosition(
        sf::Vector2f(Game::getWindow().getSize().x / 2 - m_loadoutWindow->GetClientRect().width / 2,
            Game::getWindow().getSize().y / 2 - m_loadoutWindow->GetClientRect().height / 2));
    m_statsWindow->SetPosition(
        sf::Vector2f(Game::getWindow().getSize().x / 2 - m_statsWindow->GetClientRect().width / 2,
            Game::getWindow().getSize().y / 2 - m_statsWindow->GetClientRect().height / 2));
    switch (m_currentState) {
    case eUiState::Name: {
        m_nameWindow->Show();
        if (hasKeyEvents && event.type == sf::Event::KeyReleased
            && event.key.code == sf::Keyboard::Enter) {
            std::string name = m_entry->GetText();
            if (name.size() != 0) {
                player->setName(name);
                m_currentState = eUiState::Loadout;
                m_nameWindow->Show(false);
            }
        }
        break;
    }

    case eUiState::Loadout:
        m_loadoutWindow->Show();
        break;
    case eUiState::Description:
        break;
    case eUiState::Attributes:
        m_attrLeftLabel->SetText("Points Left: " + to_string(m_pointsLeft));
        m_profLeftLabel->SetText("Points Left: " + to_string(m_profLeft));
        m_strLabel->SetText(
            "Strength: " + to_string(player->getCreatureComponent()->getStrength()));
        m_agiLabel->SetText("Agility: " + to_string(player->getCreatureComponent()->getAgility()));
        m_intLabel->SetText(
            "Intuition: " + to_string(player->getCreatureComponent()->getIntuition()));
        m_perLabel->SetText(
            "Perception: " + to_string(player->getCreatureComponent()->getPerception()));
        m_wilLabel->SetText(
            "Willpower: " + to_string(player->getCreatureComponent()->getWillpower()));

        m_gritLabel->SetText("Grit (Average of Strength and Willpower): "
            + to_string(player->getCreatureComponent()->getGrit()));
        m_shrewdLabel->SetText("Shrewdness (Average of Intuition and Perception): "
            + to_string(player->getCreatureComponent()->getShrewdness()));
        m_reflexLabel->SetText("Reflex (Average of Agility and Intuition): "
            + to_string(player->getCreatureComponent()->getReflex()));
        m_mobLabel->SetText("Mobility (Average of Strength and Agility): "
            + to_string(player->getCreatureComponent()->getMobility()));
        for (unsigned i = 0; i < static_cast<unsigned>(eWeaponTypes::Count); ++i) {
            eWeaponTypes type = static_cast<eWeaponTypes>(i);
            m_profLabels[i]->SetText(weaponTypeToString(type) + ": "
                + to_string(player->getCreatureComponent()->getProficiency(type)));
        }
        m_statsWindow->Show();
        break;
    case eUiState::Proficiencies:
        break;
    default:
        break;
    }
}

bool CreateCharUI::isDone() { return (m_currentState == eUiState::Finished); }
