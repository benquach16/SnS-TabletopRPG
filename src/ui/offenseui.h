#pragma once

#include <SFGUI/Button.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/ComboBox.hpp>

#include "creatures/player.h"
#include "numberinput.h"

class CombatInstance;

class OffenseUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player, Creature* target,
        const CombatInstance* instance, bool allowStealInitiative = false,
        bool linkedParry = false);
    void resetState() { m_currentState = eUiState::ChooseWeapon; }
    void initialize();
private:
    void doChooseWeapon(bool hasKeyEvents, sf::Event event, Player* player);
    void doManuever(
        bool hasKeyEvents, sf::Event event, Player* player, const CombatInstance* instance);
    void doFeint(bool hasKeyEvents, sf::Event event, Player* player);
    void doHeavyBlow(bool hasKeyEvents, sf::Event event, Player* player);
    void doComponent(bool hasKeyEvents, sf::Event event, Player* player);
    void doDice(bool hasKeyEvents, sf::Event event, Player* player);
    void doTarget(bool hasKeyEvents, sf::Event event, Player* player, Creature* target);
    void doInspect(bool hasKeyEvents, sf::Event event, Creature* target);
    void doPinpointThrust(bool hasKeyEvents, sf::Event event, Player* player);

    enum class eUiState : unsigned {
        ChooseWeapon,
        ChooseManuever,
        ChooseFeint,
        ChooseHeavyBlow,
        InspectTarget,
        ChooseComponent,
        ChooseDice,
        ChooseTarget,
        PinpointThrust,
        Finished,
    };

    NumberInput m_numberInput;
    eUiState m_currentState;
};
