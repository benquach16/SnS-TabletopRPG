#pragma once

#include <SFML/Graphics.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Window.hpp>
#include <memory>

#include "../combatmanager.h"
#include "defenseui.h"
#include "numberinput.h"
#include "offenseui.h"
#include "positionui.h"
#include "precombatui.h"
#include "preresolveui.h"

class CombatInstance;

class CombatUI {
public:
    CombatUI();
    void initialize();
    void show();
    void hide();
    void run(bool hasKeyEvents, sf::Event event, const CombatManager* manager);

private:
    sf::RectangleShape m_combatBkg;

    enum class eInitiativeSubState : unsigned {
        ChooseInitiative,
        InspectTarget,
        Finished,
    };

    enum class eStolenOffenseSubState : unsigned {
        ChooseDice,
        Finished,
    };

    enum class eDualRedStealSubState : unsigned { ChooseInitiative, ChooseDice, Finished };

    enum class ePositionRollSubState : unsigned { ChooseDice, Finished };

    void resetState();
    void doInitiative(bool hasKeyEvents, sf::Event event, Player* player, Creature* target);

    void doStolenOffense(bool hasKeyEvents, sf::Event event, Player* player);
    void doDualRedSteal(bool hasKeyEvents, sf::Event event, Player* player);

    void allowFeint(bool hasKeyEvents, sf::Event event, Player* player);

    std::string constructStatBoxText(const Creature* creature);
    void showSide1Stats(const CombatInstance* instance);
    void showSide2Stats(const CombatInstance* instance);

    NumberInput m_numberInput;

    eInitiativeSubState m_initiativeState;
    eStolenOffenseSubState m_stolenOffenseState;
    eDualRedStealSubState m_dualRedState;
    ePositionRollSubState m_positionState;

    DefenseUI m_defenseUI;
    OffenseUI m_offenseUI;
    PositionUI m_positionUI;
    PrecombatUI m_precombatUI;
    PreresolveUI m_preresolveUI;
};
