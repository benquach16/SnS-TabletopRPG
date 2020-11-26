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
            player->setPreResolutionReady();
            m_currentState = eUiState::Finished;
        }
    }
}
