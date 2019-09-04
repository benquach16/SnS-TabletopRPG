#include "defenseui.h"
#include "../game.h"
#include "common.h"
#include "types.h"

void DefenseUI::run(bool hasKeyEvents, sf::Event event, Player* player)
{
    switch (m_currentState) {
    case eUiState::ChooseManuever:
        doManuever(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseDice:
        doChooseDice(hasKeyEvents, event, player);
        break;
    case eUiState::Finished:
        break;
    default:
        assert(true);
        break;
    }
}

void DefenseUI::doManuever(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString(
        "Choose defense:\na - Parry\nb - Dodge\nc - Linked Parry "
        "(1AP)\nd - Steal Initiative\ne - Counter (2AP)\nf - Expulsion (1AP)\ng - Disengage");
    Game::getWindow().draw(text);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            player->setDefenseManuever(eDefensiveManuevers::Parry);
            m_currentState = eUiState::ChooseDice;
            break;
        case 'b':
            player->setDefenseManuever(eDefensiveManuevers::Dodge);
            m_currentState = eUiState::ChooseDice;
            break;
        case 'c': {
            // costs 1 die
            int cost = defenseManueverCost(eDefensiveManuevers::ParryLinked);
            if (player->getCombatPool() < cost) {
                Log::push("Requires 1 action point");
            } else {
                player->setDefenseManuever(eDefensiveManuevers::ParryLinked);
                player->reduceCombatPool(cost);
                m_currentState = eUiState::ChooseDice;
            }
            break;
        }
        case 'd':
            player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
            m_currentState = eUiState::ChooseDice;
            break;
        case 'f': {
            if (player->getCombatPool() >= 1) {
                player->setDefenseManuever(eDefensiveManuevers::Expulsion);
                player->reduceCombatPool(1);
                m_currentState = eUiState::ChooseDice;
            } else {
                Log::push("Requires 1 AP");
            }
            break;
        }
        case 'e': {
            int cost = defenseManueverCost(eDefensiveManuevers::Counter);
            if (player->getCombatPool() < cost) {
                Log::push("Requires 2 action points");
            } else {
                player->setDefenseManuever(eDefensiveManuevers::Counter);
                player->reduceCombatPool(cost);
                m_currentState = eUiState::ChooseDice;
            }
            break;
        }
        }
    }
}

void DefenseUI::doChooseDice(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Allocate action points (" + std::to_string(player->getCombatPool())
        + " action points left):");
    Game::getWindow().draw(text);

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {

        player->setDefenseDice(m_numberInput.getNumber());
        player->reduceCombatPool(m_numberInput.getNumber());
        m_currentState = eUiState::Finished;
        // last one so set flag
        player->setDefenseReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}
