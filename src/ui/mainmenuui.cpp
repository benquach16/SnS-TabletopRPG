#include <filesystem>

#include "../game.h"
#include "common.h"
#include "mainmenuui.h"
#include "types.h"

using namespace std;

MainMenuUI::MainMenuUI() {}

void MainMenuUI::run(bool hasKeyEvents, sf::Event event)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    UiCommon::initializeText(text);
    string str = "<Insert Game Title Here>\n\n\na - New Game\n";
    // this is awful as it is a file system access every frame
    bool existingSave = std::filesystem::exists(Game::cSaveString);
    if (existingSave) {
        str += "b - Load Game\n";
    } else {
        str += "b - No savegame to load!\n";
    }

    str += "c - Arena\nd - Exit\n";
    text.setString(str);
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            Game::getSingleton()->setupNewgame();
			Game::getSingleton()->setState(Game::eApplicationState::CharCreation);
            break;
        case 'b':
            if (existingSave) {
				Game::getSingleton()->load(Game::cSaveString);
            }

            break;

        case 'c':
			Game::getSingleton()->setupArena();
			Game::getSingleton()->setState(Game::eApplicationState::CharCreation);
            break;
        case 'd':
            break;
        }
    }
}
