#include "mainmenuui.h"
#include "../game.h"
#include "common.h"
#include "types.h"

MainMenuUI::MainMenuUI() {}

void MainMenuUI::run(bool hasKeyEvents, sf::Event event, Game* game)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    UiCommon::initializeText(text);
    text.setString("<Insert Game Title Here>\n\n\na - New Game\nb - Load Game\nc - Exit");
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            game->setupNewgame();
            game->setState(Game::eApplicationState::CharCreation);
            break;
        case 'b':
            game->load("save.dat");
            game->setState(Game::eApplicationState::Gameplay);
            break;

        case 'c':

            break;
        }
    }
}
