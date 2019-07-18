#include "inspectui.h"
#include "../creatures/creature.h"
#include "../game.h"

void InspectUI::run(Creature* creature)
{
	auto windowSize = Game::getWindow().getSize();
	sf::RectangleShape rect(sf::Vector2f(windowSize.x, windowSize.y));

	Game::getWindow().draw(rect);
}
