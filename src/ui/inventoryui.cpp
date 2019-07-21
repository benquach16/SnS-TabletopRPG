#include "inventoryui.h"
#include "types.h"
#include "../game.h"
#include "../object/creatureobject.h"

InventoryUI::InventoryUI()
{
}

void InventoryUI::run(sf::Event event, PlayerObject* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, windowSize.y/2));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	
	sf::RectangleShape bkg2(sf::Vector2f(windowSize.x, windowSize.y/3));
	bkg2.setPosition(sf::Vector2f(0, windowSize.y - windowSize.y/3));
	bkg2.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg2);
}
