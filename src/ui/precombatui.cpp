#include "precombatui.h"
#include "../game.h"
#include "common.h"
#include "types.h"

using namespace std;

void PrecombatUI::run(sf::Event event, Player* player)
{
    switch (m_currentState) {
    case eUiState::ChooseFavoring:
        doFavoring(event);
        break;
    case eUiState::ChooseGrip:
        doChooseGrip(event);
        break;
    case eUiState::Finished:
        break;
    }
}

void PrecombatUI::doFavoring(sf::Event event)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Favor location (1AP)?\na - Yes\nb - No");

    Game::getWindow().draw(text);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            break;
        case 'b':
            break;
        }
    }
}

void PrecombatUI::doChooseGrip(sf::Event event)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Switch Grip?\na - Standard\nb - Staff Grip");

    Game::getWindow().draw(text);
}
