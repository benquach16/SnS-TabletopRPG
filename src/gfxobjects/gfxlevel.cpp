#include "gfxlevel.h"
#include "../game.h"
#include "../level/level.h"
#include "utils.h"

GFXLevel::GFXLevel()
{
	m_texture.loadFromFile("data/textures/test.png");
}

void GFXLevel::run(const Level* level, vector2d center)
{
	if(level == nullptr) {
		return;
	}

	int width = level->getWidth();
	int height = level->getHeight();

	auto windowSize = Game::getWindow().getSize();

	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			Tile tile = (*level)(x, y);
			sf::RectangleShape rect(sf::Vector2f(cWidth,cWidth));
			rect.setFillColor(sf::Color(44, 44, 44));
			sf::Vector2f pos(x * cWidth, y * cWidth);
			rect.setRotation(45.f);
			pos = coordsToScreen(pos);
			rect.setPosition(pos);
			Game::getWindow().draw(rect);
		}
	}

	const std::vector<Object*> rLevelObjs = level->getObjects();

	for(int i = 0; i < rLevelObjs.size(); ++i) {
		vector2d position = rLevelObjs[i]->getPosition();
		sf::RectangleShape rect(sf::Vector2f(cWidth,cWidth));
		rect.setFillColor(sf::Color(99, 99, 99));
		sf::Sprite sprite;
		sprite.setTexture(m_texture);
		sf::Vector2f pos(position.x * cWidth, position.y * cWidth);
		pos = coordsToScreen(pos);
		rect.setPosition(pos);
		rect.setRotation(45.f);
		pos.y-=32;
		pos.x-=16;
		sprite.setPosition(pos);
		Game::getWindow().draw(rect);	
		Game::getWindow().draw(sprite);

	}

}
