#include "positionui.h"
#include "../game.h"
#include "common.h"
#include "types.h"

using namespace std;

void PositionUI::run(bool hasKeyEvents, sf::Event event, Player* player)
{
    switch (m_currentState) {
    case eUiState::ChooseDice:
        doPositionRoll(hasKeyEvents, event, player);
        break;
    default:
        break;
    }
}

void PositionUI::doPositionRoll(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Allocate action points for positioning roll ("
        + std::to_string(player->getCombatPool()) + " action points left):");

    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Enter) {
        player->setPositionDice(m_numberInput.getNumber());
        m_currentState = eUiState::Finished;
        player->reduceCombatPool(m_numberInput.getNumber());
        // last one so set flag
        player->setPositionReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}
