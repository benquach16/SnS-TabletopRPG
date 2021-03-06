#pragma once

#include "../creatures/player.h"
#include "numberinput.h"

class CombatInstance;

class OffenseUI {
public:
    void run(bool hasKeyEvents, sf::Event event, Player* player, Creature* target,
        const CombatInstance* instance, bool allowStealInitiative = false, bool payCosts = true);
    void resetState() { m_currentState = eUiState::ChooseWeapon; }

private:
    void doChooseWeapon(bool hasKeyEvents, sf::Event event, Player* player);
    void doManuever(bool hasKeyEvents, sf::Event event, Player* player, const Creature* target,
        const CombatInstance* instance);
    void doFeint(bool hasKeyEvents, sf::Event event, Player* player);
    void doHeavyBlow(bool hasKeyEvents, sf::Event event, Player* player);
    void doComponent(bool hasKeyEvents, sf::Event event, Player* player);
    void doDice(bool hasKeyEvents, sf::Event event, Player* player);
    void doTarget(
        bool hasKeyEvents, sf::Event event, Player* player, Creature* target, bool payCosts);
    void doInspect(bool hasKeyEvents, sf::Event event, Creature* target);
    void doPinpointThrust(bool hasKeyEvents, sf::Event event, Player* player);
    void doHookTarget(bool hasKeyEvents, sf::Event event, Player* player);
    void doBeatTarget(bool hasKeyEvents, sf::Event event, Player* player);
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
        ChooseHookTarget,
        ChooseBeatTarget,
        Finished,
    };

    NumberInput m_numberInput;
    eUiState m_currentState;
};
