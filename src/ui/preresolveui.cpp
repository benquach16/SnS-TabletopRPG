#include "preresolveui.h"
#include "../game.h"
#include "common.h"
#include "types.h"

using namespace std;

void PreresolveUI::run(bool hasKeyEvents, sf::Event event, Player* player)
{
    switch (m_currentState) {
    case eUiState::ChooseFeint:
        doFeint(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseDice:
        doDice(hasKeyEvents, event, player);
        break;
    case eUiState::Finished:
        break;
    }
}

void PreresolveUI::doFeint(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Feint attack? (2AP)\na - No\nb - Yes");
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c == 'a') {
            player->setPreResolutionReady();
            m_currentState = eUiState::Finished;
        }
        if (c == 'b') {
            // TODO : REPLACE ME WITH CODE IN CREATURE TO REDUCE COMBAT POOL
            player->setOffenseFeint();
            player->reduceCombatPool(2);

            m_currentState = eUiState::ChooseDice;
        }
    }
}

void PreresolveUI::doDice(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Choose amount of AP to invest in the feint:\n");
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Enter) {
        int dice = m_numberInput.getNumber();
        player->setOffenseFeintDice(dice);
        player->reduceCombatPool(dice);
        m_currentState = eUiState::Finished;
        // last one so set flag
        player->setPreResolutionReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}
