#include "inventoryui.h"
#include "types.h"
#include "../game.h"

InventoryUI::InventoryUI()
{
}

void InventoryUI::run(sf::Event event)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, windowSize.y/2));

	Game::getWindow().draw(bkg);
}
