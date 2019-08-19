#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "../combatmanager.h"
#include "defenseui.h"
#include "numberinput.h"
#include "offenseui.h"
#include "positionui.h"
#include "precombatui.h"

class CombatInstance;

class CombatUI {
public:
    CombatUI();

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
};
