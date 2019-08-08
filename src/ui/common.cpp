#include "common.h"
#include "../game.h"
#include "types.h"

constexpr int cHeight = 14;

namespace UiCommon {

void drawTopPanel()
{
    auto windowSize = Game::getWindow().getSize();

    static sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cHeight));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);
}

} //namespace UiCommon
