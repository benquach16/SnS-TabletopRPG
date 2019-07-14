#include "gfxlevel.h"
#include "../game.h"
#include "../level/level.h"

void GFXLevel::run()
{
	if(m_level == nullptr) {
		return;
	}

	int width = m_level->getWidth();
	int height = m_level->getHeight();

	auto windowSize = Game::getWindow().getSize();

	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			Tile tile = (*m_level)(x, y);
			sf::RectangleShape rect(sf::Vector2f(32,32));
			rect.setFillColor(sf::Color(44, 44, 44));
			rect.setPosition(sf::Vector2f(x * 32, y * 32));
			Game::getWindow().draw(rect);
		}
	}

	std::vector<Object*> rLevelObjs = m_level->getObjects();

	for(int i = 0; i < rLevelObjs.size(); ++i) {
		vector2d position = rLevelObjs[i]->getPosition();
		sf::RectangleShape rect(sf::Vector2f(32,32));
		rect.setFillColor(sf::Color(99, 99, 99));
		rect.setPosition(sf::Vector2f(position.x * 32, position.y * 32));
		Game::getWindow().draw(rect);	
	}
}
