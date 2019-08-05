#include "precombatui.h"
#include "../game.h"
#include "common.h"
#include "types.h"

using namespace std;

void PrecombatUI::run(sf::Event event, Player* player)
{
    switch (m_currentState) {
    case eUiState::ChooseFavoring:
        doFavoring(event, player);
        break;
    case eUiState::ChooseFavorLocations:
        doFavorLocation(event, player);
        break;
    case eUiState::ChooseGrip:
        doChooseGrip(event, player);
        break;
    case eUiState::Finished:
        break;
    }
}

void PrecombatUI::doFavoring(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Favor location (1AP)?\na - Skip all pre-exchange actions\nb - Yes\nc - No");

    Game::getWindow().draw(text);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            player->setPrecombatReady();
            break;
        case 'b':
            m_currentState = eUiState::ChooseFavorLocations;
            break;
        case 'c':
            m_currentState = eUiState::ChooseGrip;
            break;
        }
    }
}

void PrecombatUI::doFavorLocation(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Choose location");
    Game::getWindow().draw(text);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            player->setPrecombatReady();
            break;
        case 'b':
            m_currentState = eUiState::ChooseFavorLocations;
            break;
        case 'c':
            m_currentState = eUiState::ChooseGrip;
            break;
        }
    }
}

void PrecombatUI::doChooseGrip(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Switch Grip?\na - Standard\nb - Staff Grip\nc - Overhead");
    Game::getWindow().draw(text);

    Game::getWindow().draw(text);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            player->setPrecombatReady();
            break;
        case 'b':
            m_currentState = eUiState::ChooseFavorLocations;
            break;
        case 'c':
            m_currentState = eUiState::ChooseGrip;
            break;
        }
    }
}
