#include "common.h"
#include "../game.h"
#include "types.h"

namespace UiCommon {

	void drawTopPanel()
	{
		auto windowSize = Game::getWindow().getSize();
	
		sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
		bkg.setFillColor(sf::Color(12, 12, 23));
		Game::getWindow().draw(bkg);	
	}

};
