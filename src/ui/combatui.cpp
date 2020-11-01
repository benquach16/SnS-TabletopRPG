#include <iostream>

#include "../combatinstance.h"
#include "../creatures/player.h"
#include "../creatures/utils.h"
#include "../game.h"
#include "../items/utils.h"
#include "../items/weapon.h"
#include "combatui.h"
#include "common.h"
#include "types.h"

using namespace std;

constexpr unsigned logHeight = cCharSize * (cLinesDisplayed + 1);
constexpr unsigned rectHeight = cCharSize * 6;

CombatUI::CombatUI() { resetState(); }

void CombatUI::resetState()
{
    m_defenseUI.resetState();
    m_offenseUI.resetState();
    m_positionUI.resetState();
    m_precombatUI.resetState();
    m_preresolveUI.resetState();
    m_initiativeState = eInitiativeSubState::ChooseInitiative;
    m_stolenOffenseState = eStolenOffenseSubState::ChooseDice;
    m_dualRedState = eDualRedStealSubState::ChooseInitiative;
}

void CombatUI::run(bool hasKeyEvents, sf::Event event, const CombatManager* manager)
{
    // bug - players combat manager might not be parent, then we are working with an outdated
    // instance
    if (manager == nullptr) {
        cout << "this shouldnt happen" << endl;
        return;
    }

    CombatEdge* edge = manager->getCurrentEdge();
    if (edge == nullptr) {
        return;
    }
    CombatInstance* instance = edge->getInstance();

    assert(instance != nullptr);

    if (instance->getState() == eCombatState::Uninitialized) {
        return;
    }

    auto windowSize = Game::getWindow().getSize();

    showSide1Stats(instance);
    showSide2Stats(instance);

    assert(instance->getSide1()->isPlayer() == true);
    Player* player = static_cast<Player*>(instance->getSide1());
    Creature* target = instance->getSide2();

    sf::RectangleShape reachBkg(sf::Vector2f(windowSize.x - 4, cCharSize));
    reachBkg.setFillColor(sf::Color(12, 12, 23));
    reachBkg.setPosition(2, windowSize.y - logHeight - rectHeight - cCharSize - 2);
    reachBkg.setOutlineThickness(2);
    reachBkg.setOutlineColor(sf::Color(22, 22, 33));
    int reachCost = calculateReachCost(instance->getCurrentReach(), player->getCurrentReach());

    sf::Text reachTxt;
    reachTxt.setCharacterSize(cCharSize);
    reachTxt.setFont(Game::getDefaultFont());
    reachTxt.setString("Current reach is " + lengthToString(instance->getCurrentReach()) + " ("
        + to_string(reachCost) + "AP to attack)");
    reachTxt.setPosition(5, windowSize.y - logHeight - rectHeight - cCharSize - 6);
    Game::getWindow().draw(reachBkg);
    Game::getWindow().draw(reachTxt);

    if (manager->getState() == eCombatManagerState::PositioningRoll) {
        m_positionUI.run(hasKeyEvents, event, player);
        return;
    }

    if (edge->getActive() == false || edge->getCurrent() == false) {
        UiCommon::drawTopPanel();
        sf::Text text;
        text.setFont(Game::getDefaultFont());
        text.setCharacterSize(cCharSize);
        text.setString("Waiting for turn...");
        Game::getWindow().draw(text);
        return;
    }
    if (instance->getState() == eCombatState::Initialized) {
        resetState();
        return;
    }
    if (instance->getState() == eCombatState::RollInitiative) {
        doInitiative(hasKeyEvents, event, player, target);
        return;
    }
    if (instance->getState() == eCombatState::PreexchangeActions) {
        m_precombatUI.run(hasKeyEvents, event, player);
        return;
    }
    if (instance->getState() == eCombatState::ResetState) {
        resetState();
        return;
    }
    if (instance->getState() == eCombatState::Offense && instance->isAttackerPlayer() == true) {
        m_offenseUI.run(hasKeyEvents, event, player, target);
        return;
    }
    if (instance->getState() == eCombatState::Defense && instance->isDefenderPlayer() == true) {
        m_defenseUI.run(hasKeyEvents, event, player, instance->getLastTempo());
        return;
    }
    if (instance->getState() == eCombatState::AttackFromDefense) {
        m_offenseUI.run(hasKeyEvents, event, player, target);
        return;
    }
    if (instance->getState() == eCombatState::ParryLinked) {
        m_offenseUI.run(hasKeyEvents, event, player, target, false, true);
        return;
    }

    // TODO: make dual red force initiative steal from both characters, this is hacky and bad
    if (instance->getState() == eCombatState::DualOffense1
        && instance->isAttackerPlayer() == true) {
        if (m_dualRedState == eDualRedStealSubState::Finished) {
            m_offenseUI.run(hasKeyEvents, event, player, target, false);
        } else {
            doDualRedSteal(hasKeyEvents, event, player);
        }
        return;
    }
    if (instance->getState() == eCombatState::DualOffense2
        && instance->isAttackerPlayer() == true) {
        if (m_dualRedState == eDualRedStealSubState::Finished) {
            m_offenseUI.run(hasKeyEvents, event, player, target, false);
        } else {
            doDualRedSteal(hasKeyEvents, event, player);
        }
        return;
    }
    if (instance->getState() == eCombatState::StolenOffense
        && instance->isAttackerPlayer() == true) {
        doStolenOffense(hasKeyEvents, event, player);
        return;
    }
    if (instance->getState() == eCombatState::DualOffenseStealInitiative
        && instance->isDefenderPlayer() == true) {
        // absurdly ugly and probably unnecessary
        if (m_stolenOffenseState == eStolenOffenseSubState::Finished) {
            m_offenseUI.run(hasKeyEvents, event, player, target);
        } else {
            doStolenOffense(hasKeyEvents, event, player);
        }
        return;
    }
    if (instance->getState() == eCombatState::DualOffenseSecondInitiative
        && instance->isDefenderPlayer() == true) {
        doStolenOffense(hasKeyEvents, event, player);
        return;
    }
    if (instance->getState() == eCombatState::StealInitiative) {
        m_offenseUI.run(hasKeyEvents, event, player, target);
        return;
    }
    if (instance->getState() == eCombatState::PreResolution
        && instance->isAttackerPlayer() == true) {
        m_preresolveUI.run(hasKeyEvents, event, player);
        return;
    }
    if (instance->getState() == eCombatState::Resolution) {
        resetState();
        m_defenseUI.resetState();
        return;
    }
    if (instance->getState() == eCombatState::DualOffenseResolve) {
        resetState();
        return;
    }

    if (instance->getState() == eCombatState::FinishedCombat) {
        return;
    }
}

void CombatUI::doInitiative(bool hasKeyEvents, sf::Event event, Player* player, Creature* target)
{
    if (m_initiativeState == eInitiativeSubState::ChooseInitiative) {
        UiCommon::drawTopPanel();

        sf::Text text;
        text.setCharacterSize(cCharSize);
        text.setFont(Game::getDefaultFont());
        text.setString("Choose initiative:\na - Attack \nb - Defend\nc - Inspect target");
        Game::getWindow().draw(text);

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            switch (c) {
            case 'a':
                player->setInitiative(eInitiativeRoll::Attack);
                m_initiativeState = eInitiativeSubState::Finished;
                break;
            case 'b':
                player->setInitiative(eInitiativeRoll::Defend);
                m_initiativeState = eInitiativeSubState::Finished;
                break;
            case 'c':
                m_initiativeState = eInitiativeSubState::InspectTarget;
                break;
            }
        }
    } else if (m_initiativeState == eInitiativeSubState::InspectTarget) {
        UiCommon::drawTopPanel();

        sf::Text text;
        text.setCharacterSize(cCharSize);
        text.setFont(Game::getDefaultFont());
        string str = "They ";
        if (target->getStrength() < player->getStrength()) {
            str += "have less strength than you. ";
        } else {
            str += "have more strength than you. ";
        }
        str += "They ";
        if (target->getShrewdness() < player->getShrewdness()) {
            str += "have less shrewdness than you.";
        } else {
            str += "have more shrewdness than you.";
        }
        str += "They ";
        if (target->getMobility() < player->getMobility()) {
            str += "are slower than you.";
        } else {
            str += "are faster than you.";
        }
        text.setString(str);
        Game::getWindow().draw(text);
        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            m_initiativeState = eInitiativeSubState::ChooseInitiative;
        }
    }
}

void CombatUI::doStolenOffense(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    if (m_stolenOffenseState == eStolenOffenseSubState::ChooseDice) {

        sf::Text text;
        text.setCharacterSize(cCharSize);
        text.setFont(Game::getDefaultFont());
        text.setString("Initiative roll, allocate action points ("
            + std::to_string(player->getCombatPool()) + " action points left):");

        Game::getWindow().draw(text);

        if (hasKeyEvents && event.type == sf::Event::KeyReleased
            && event.key.code == sf::Keyboard::Enter) {
            player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
            player->setDefenseDice(m_numberInput.getNumber());
            // last one so set flag
            player->setDefenseReady();
            player->reduceCombatPool(m_numberInput.getNumber());
            m_numberInput.reset();
            m_stolenOffenseState = eStolenOffenseSubState::Finished;
        }

        m_numberInput.setMax(player->getCombatPool());
        m_numberInput.run(hasKeyEvents, event);
        m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
    }
}

void CombatUI::doDualRedSteal(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    // TODO :remove this, make everyone steal initiatve on dual red
    if (m_dualRedState == eDualRedStealSubState::ChooseInitiative) {
        sf::Text text;
        text.setCharacterSize(cCharSize);
        text.setFont(Game::getDefaultFont());
        text.setString("Steal Initiative?\na - Yes\nb - No");
        if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::A) {
                player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
                m_dualRedState = eDualRedStealSubState::ChooseDice;
            }
            if (event.key.code == sf::Keyboard::B) {
                m_dualRedState = eDualRedStealSubState::Finished;
            }
        }
        Game::getWindow().draw(text);
    } else if (m_dualRedState == eDualRedStealSubState::ChooseDice) {
        sf::Text text;
        text.setCharacterSize(cCharSize);
        text.setFont(Game::getDefaultFont());
        text.setString("Initiative roll, allocate action points ("
            + std::to_string(player->getCombatPool()) + " action points left):");

        Game::getWindow().draw(text);

        if (hasKeyEvents && event.type == sf::Event::KeyReleased
            && event.key.code == sf::Keyboard::Enter) {
            player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
            player->setDefenseDice(m_numberInput.getNumber());
            player->reduceCombatPool(m_numberInput.getNumber());
            // last one so set flag
            player->setDefenseReady();
            m_numberInput.reset();
            m_dualRedState = eDualRedStealSubState::Finished;
        }

        m_numberInput.setMax(player->getCombatPool());
        m_numberInput.run(hasKeyEvents, event);
        m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
    }
}

void CombatUI::showSide1Stats(const CombatInstance* instance)
{
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape combatBkg(sf::Vector2f(windowSize.x / 2 - 4, rectHeight));
    combatBkg.setPosition(2, windowSize.y - logHeight - rectHeight - 3);
    combatBkg.setFillColor(sf::Color(12, 12, 23));
    combatBkg.setOutlineThickness(2);
    combatBkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(combatBkg);

    assert(instance != nullptr);
    Creature* creature = instance->getSide1();
    sf::Text side1Info;
    side1Info.setString(creature->getName() + " \n" + creature->getPrimaryWeapon()->getName()
        + " - " + lengthToString(creature->getCurrentReach()) + " - "
        + gripToString(creature->getGrip())
        + " grip / Off-Hand: " + creature->getSecondaryWeapon()->getName());
    side1Info.setCharacterSize(cCharSize);
    side1Info.setFont(Game::getDefaultFont());
    side1Info.setPosition(6, windowSize.y - logHeight - rectHeight);

    Game::getWindow().draw(side1Info);

    sf::Text ap;
    ap.setCharacterSize(cCharSize);
    ap.setFont(Game::getDefaultFont());
    ap.setPosition(6, windowSize.y - logHeight - rectHeight + (cCharSize * 2));
    ap.setString("Action Points : " + to_string(creature->getCombatPool()) + " / "
        + to_string(creature->getMaxCombatPool())
        + " - Success rate: " + to_string(creature->getSuccessRate()) + "%" + '\n' + "Blood loss: "
        + to_string(creature->getBloodLoss()) + '\n' + stanceToString(creature->getStance()));

    Game::getWindow().draw(ap);
}

void CombatUI::showSide2Stats(const CombatInstance* instance)
{
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape combatBkg(sf::Vector2f(windowSize.x / 2 - 2, rectHeight));
    combatBkg.setPosition(windowSize.x / 2, windowSize.y - logHeight - rectHeight - 3);
    combatBkg.setFillColor(sf::Color(12, 12, 23));
    combatBkg.setOutlineThickness(2);
    combatBkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(combatBkg);

    assert(instance != nullptr);
    Creature* creature = instance->getSide2();
    sf::Text side1Info;
    side1Info.setString(creature->getName() + " \n" + creature->getPrimaryWeapon()->getName()
        + " - " + lengthToString(creature->getCurrentReach()) + " - "
        + gripToString(creature->getGrip())
        + " grip / Off-Hand: " + creature->getSecondaryWeapon()->getName());
    side1Info.setCharacterSize(cCharSize);
    side1Info.setFont(Game::getDefaultFont());
    side1Info.setPosition(windowSize.x / 2 + 5, windowSize.y - logHeight - rectHeight);

    Game::getWindow().draw(side1Info);

    sf::Text ap;
    ap.setCharacterSize(cCharSize);
    ap.setFont(Game::getDefaultFont());
    ap.setPosition(windowSize.x / 2 + 5, windowSize.y - logHeight - rectHeight + (cCharSize * 2));
    ap.setString("Action Points : " + to_string(creature->getCombatPool()) + " / "
        + to_string(creature->getMaxCombatPool())
        + " - Success rate: " + to_string(creature->getSuccessRate()) + "%" + '\n' + "Blood loss: "
        + to_string(creature->getBloodLoss()) + '\n' + stanceToString(creature->getStance()));

    Game::getWindow().draw(ap);
}
