#include <iostream>

#include "combatinstance.h"
#include "combatui.h"
#include "common.h"
#include "creatures/player.h"
#include "creatures/utils.h"
#include "game.h"
#include "items/utils.h"
#include "items/weapon.h"
#include "types.h"

using namespace std;

constexpr unsigned logHeight = cCharSize * (cLinesDisplayed + 1);
constexpr unsigned rectHeight = cCharSize * 6;

CombatUI::CombatUI()
    : m_currentPlayer(nullptr)
{
    resetState();
}

void CombatUI::resetState()
{
    m_defenseUI.resetState();
    m_offenseUI.resetState();
    m_positionUI.resetState();
    m_precombatUI.resetState();
    m_preresolveUI.resetState();
    m_initiativeState = eInitiativeSubState::ChooseInitiative;
    m_stolenOffenseState = eStolenOffenseSubState::ChooseDice;
    m_dualRedState = eDualRedStealSubState::ChooseDice;
}

void CombatUI::initialize()
{
    m_precombatUI.initialize();

    sf::Vector2u windowSize = Game::getWindow().getSize();
    auto attack = sfg::Button::Create("Attack");
    auto def = sfg::Button::Create("Defend");
    auto inspect = sfg::Button::Create("Inspect");
    m_info = sfg::Label::Create("");
    m_info->SetLineWrap(true);
    m_info->SetRequisition(sf::Vector2f(windowSize.x, 0));
    auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10);
    m_initWindow = sfg::Window::Create();
    m_initWindow->SetStyle(m_initWindow->GetStyle() ^ sfg::Window::RESIZE);
    m_initWindow->SetStyle(m_initWindow->GetStyle() ^ sfg::Window::TITLEBAR);
    m_initWindow->Add(box);
    box->Pack(sfg::Label::Create("Choose Initiative"), false, false);
    box->Pack(attack, false, false);
    box->Pack(def, false, false);

    // shared ptr memory leak
    attack->GetSignal(sfg::Button::OnMouseEnter).Connect([this] {
        m_info->SetText("Choosing Attack means you will attempt to take initiative on the first "
                      "exchange. If the enemy also chooses attack, then both of you will attempt "
                      "to land an attack without defending, which can result in a double-kill.");
    });
    attack->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        m_currentPlayer->setInitiative(eInitiativeRoll::Attack);
        m_initiativeState = eInitiativeSubState::Finished;
        m_info->SetText("");
        hide();
    });
    def->GetSignal(sfg::Button::OnMouseEnter).Connect([this] {
        m_info->SetText(
            "Choosing Defend means you will attempt to defend on the first exchange. If both of "
            "you defend, then no one takes initiative and you can choose initiative again.");
    });
    def->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        m_currentPlayer->setInitiative(eInitiativeRoll::Defend);
        m_initiativeState = eInitiativeSubState::Finished;
        m_info->SetText("");
        hide();
    });
    inspect->GetSignal(sfg::Button::OnLeftClick).Connect([this] {

    });
    box->Pack(sfg::Button::Create("Inspect Target"), false, false);
    box->Pack(m_info);
    m_initWindow->SetRequisition(sf::Vector2f(100, 40));
    Game::getSingleton()->getDesktop().Add(m_initWindow);
    hide();
}

void CombatUI::hide() { m_initWindow->Show(false); }

void CombatUI::run(bool hasKeyEvents, sf::Event event, const CombatManager* manager)
{
    assert(manager != nullptr);

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
    m_currentPlayer = player;
    Creature* target = instance->getSide2();

    sf::RectangleShape reachBkg(sf::Vector2f(windowSize.x - 4, cCharSize));
    reachBkg.setFillColor(sf::Color(12, 12, 23));
    reachBkg.setPosition(2, windowSize.y - logHeight - rectHeight - cCharSize - 2);
    reachBkg.setOutlineThickness(2);
    reachBkg.setOutlineColor(sf::Color(22, 22, 33));
    int reachCostPrimary
        = calculateReachCost(instance->getCurrentReach(), player->getCurrentReach());
    int reachCostSecondary
        = calculateReachCost(instance->getCurrentReach(), player->getSecondaryWeaponReach());
    sf::Text reachTxt;
    reachTxt.setCharacterSize(cCharSize);
    reachTxt.setFont(Game::getDefaultFont());
    reachTxt.setPosition(5, windowSize.y - logHeight - rectHeight - cCharSize - 6);
    if (instance->getInGrapple() == false) {
        string str = "Current reach is " + lengthToString(instance->getCurrentReach())
            + " (Primary: " + to_string(reachCostPrimary) + "AP to attack";
        // might break branch prediction
        if (player->getCurrentReach() > instance->getCurrentReach()) {
            str += " and defend";
        }
        str += " / Secondary: " + to_string(reachCostSecondary) + "AP to attack";
        if (player->getSecondaryWeaponReach() > instance->getCurrentReach()) {
            str += " and defend";
        }
        str += ")";
        reachTxt.setString(str);

    } else {
        reachTxt.setString(
            "Currently in a grapple! Reach is fixed to Hand reach until the grapple is broken.");
        reachTxt.setFillColor(sf::Color::Red);
    }

    Game::getWindow().draw(reachBkg);
    Game::getWindow().draw(reachTxt);

    // See comment in combatmanager.h
    // The player's node is not guaranteed to be active, so this function
    // will handle that case
    if (manager->waitingForPosition()) {
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
        m_precombatUI.run(hasKeyEvents, event, player, instance->getInGrapple(), false);
        return;
    }
    if (instance->getState() == eCombatState::BetweenExchangeActions) {
        m_precombatUI.run(hasKeyEvents, event, player, instance->getInGrapple(), true);
        return;
    }
    if (instance->getState() == eCombatState::ResetState) {
        resetState();
        return;
    }
    if (instance->getState() == eCombatState::Offense && instance->isAttackerPlayer() == true) {
        m_offenseUI.run(hasKeyEvents, event, player, target, instance);
        return;
    }
    if (instance->getState() == eCombatState::Defense && instance->isDefenderPlayer() == true) {
        m_defenseUI.run(hasKeyEvents, event, player, instance);
        return;
    }
    if (instance->getState() == eCombatState::AttackFromDefense) {
        m_offenseUI.run(hasKeyEvents, event, player, target, instance);
        return;
    }
    if (instance->getState() == eCombatState::ParryLinked) {
        m_offenseUI.run(hasKeyEvents, event, player, target, instance, false, true);
        return;
    }

    // TODO: make dual red force initiative steal from both characters, this is hacky and bad
    if (instance->getState() == eCombatState::DualOffense1
        && instance->isAttackerPlayer() == true) {
        if (m_dualRedState == eDualRedStealSubState::Finished) {
            m_offenseUI.run(hasKeyEvents, event, player, target, instance, false);
        } else {
            doDualRedSteal(hasKeyEvents, event, player);
        }
        return;
    }
    if (instance->getState() == eCombatState::DualOffense2
        && instance->isAttackerPlayer() == true) {
        if (m_dualRedState == eDualRedStealSubState::Finished) {
            m_offenseUI.run(hasKeyEvents, event, player, target, instance, false);
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
            m_offenseUI.run(hasKeyEvents, event, player, target, instance);
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
        m_offenseUI.run(hasKeyEvents, event, player, target, instance);
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
        sf::Vector2u windowSize = Game::getWindow().getSize();
        m_info->SetRequisition(sf::Vector2f(windowSize.x, 0));
        m_initWindow->Show();
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

std::string CombatUI::constructStatBoxText(const Creature* creature)
{
    string str = creature->getName() + " - " + creature->getPrimaryWeapon()->getName();
    if (creature->primaryWeaponDisabled()) {
        str += "[D]";
    }
    str += " - " + lengthToString(creature->getCurrentReach()) + " - "
        + gripToString(creature->getGrip()) + " grip\n";
    // left pad, if you turn this into a library you can take down NPM
    for (unsigned i = 0; i < creature->getName().size(); ++i) {
        str += " ";
    }
    str += " - " + creature->getSecondaryWeapon()->getName() + " - "
        + lengthToString(creature->getSecondaryWeaponReach());

    return str;
}

void CombatUI::doDualRedSteal(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
    // TODO :remove this, make everyone steal initiatve on dual red
    if (m_dualRedState == eDualRedStealSubState::ChooseDice) {
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

    side1Info.setString(constructStatBoxText(creature));
    side1Info.setCharacterSize(cCharSize);
    side1Info.setFont(Game::getDefaultFont());
    side1Info.setPosition(6, windowSize.y - logHeight - rectHeight);

    Game::getWindow().draw(side1Info);

    sf::Text ap;
    ap.setCharacterSize(cCharSize);
    ap.setFont(Game::getDefaultFont());
    ap.setPosition(6, windowSize.y - logHeight - rectHeight + (cCharSize * 2));
    ap.setString("Action Points : " + to_string(creature->getCombatPool()) + " / "
        + to_string(creature->getMaxCombatPool()) + " - Pain: " + to_string(creature->getPain())
        + " Fatigue: " + to_string(creature->getFatigue()) + '\n' + "Blood loss: "
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
    side1Info.setString(constructStatBoxText(creature));
    side1Info.setCharacterSize(cCharSize);
    side1Info.setFont(Game::getDefaultFont());
    side1Info.setPosition(windowSize.x / 2 + 5, windowSize.y - logHeight - rectHeight);

    Game::getWindow().draw(side1Info);

    sf::Text ap;
    ap.setCharacterSize(cCharSize);
    ap.setFont(Game::getDefaultFont());
    ap.setPosition(windowSize.x / 2 + 5, windowSize.y - logHeight - rectHeight + (cCharSize * 2));
    ap.setString("Action Points : " + to_string(creature->getCombatPool()) + " / "
        + to_string(creature->getMaxCombatPool()) + " - Pain: " + to_string(creature->getPain())
        + " Fatigue: " + to_string(creature->getFatigue()) + '\n' + "Blood loss: "
        + to_string(creature->getBloodLoss()) + '\n' + stanceToString(creature->getStance()));

    Game::getWindow().draw(ap);
}
