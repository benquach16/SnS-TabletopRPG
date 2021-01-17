#pragma once

#include <vector>

#include <SFGUI/Box.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Table.hpp>
#include <SFGUI/Window.hpp>
#include <SFML/Graphics.hpp>

#include "numberinput.h"
#include "textinput.h"
#include "items/types.h"

class PlayerObject;

class CreateCharUI {
public:
    CreateCharUI();
    void initialize(PlayerObject* player);
    void show();
    void hide();
    void run(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void resetState() { m_currentState = eUiState::Name; }
    bool isDone();

private:
    struct StartingLoadouts {
        std::string name;
        std::string description;
        std::vector<int> armor;
        std::vector<int> inventory;
        std::vector<int> quickdraw;
        int weapon;
    };

    enum eUiState { Name, Loadout, Description, Attributes, Proficiencies, Finished };
    void clearTmpWindows();
    TextInput m_text;

    eUiState m_currentState;

    unsigned m_loadoutIdx;
    unsigned m_pointsLeft;
    unsigned m_profLeft;

    std::vector<StartingLoadouts> m_loadouts;

    sfg::Window::Ptr m_nameWindow;
    sfg::Window::Ptr m_loadoutWindow;
    sfg::Window::Ptr m_statsWindow;
    std::vector<sfg::Window::Ptr> m_tempWindows;

    sfg::Entry::Ptr m_entry;
    // stat allocation labels
    sfg::Label::Ptr m_attrLeftLabel;
    sfg::Label::Ptr m_profLeftLabel;
    sfg::Label::Ptr m_strLabel;
    sfg::Label::Ptr m_agiLabel;
    sfg::Label::Ptr m_intLabel;
    sfg::Label::Ptr m_perLabel;
    sfg::Label::Ptr m_wilLabel;
    sfg::Label::Ptr m_gritLabel;
    sfg::Label::Ptr m_shrewdLabel;
    sfg::Label::Ptr m_reflexLabel;
    sfg::Label::Ptr m_mobLabel;

    sfg::Label::Ptr m_profLabels[static_cast<unsigned>(eWeaponTypes::Count)];
};
