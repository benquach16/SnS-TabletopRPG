#include "tradeui.h"
#include "../game.h"
#include "types.h"

using namespace std;

constexpr int cDisplayLines = 28;

void TradeUI::run(std::map<int, int>& inventory, std::map<int, int>& container)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x/2, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	bkg.setOutlineThickness(1);
	bkg.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(bkg);
	sf::RectangleShape bkg2(sf::Vector2f(windowSize.x/2, cCharSize * cDisplayLines));
	bkg2.setPosition(sf::Vector2f(windowSize.x/2, 0));
	bkg2.setFillColor(sf::Color(12, 12, 23));
	bkg2.setOutlineThickness(1);
	bkg2.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(bkg2);

	sf::Text inventoryTxt;
	inventoryTxt.setFont(Game::getDefaultFont());
	inventoryTxt.setCharacterSize(cCharSize);

	string inventoryStr;

	int count = 0;
	for(auto it : inventory) {
		char idx = ('a' + count);
		const Item* item = ItemTable::getSingleton()->get(it.first);
		inventoryStr+= idx;
		inventoryStr+= " - " + item->getName() + " x" + to_string(it.second) + '\n';
		count++;
	}

	inventoryTxt.setString(inventoryStr);

	Game::getWindow().draw(inventoryTxt);

	sf::Text containerTxt;
	containerTxt.setFont(Game::getDefaultFont());
	containerTxt.setCharacterSize(cCharSize);
	containerTxt.setPosition(sf::Vector2f(windowSize.x, 0));

	string containerStr;

	for(auto it : container) {
		char idx = ('a' + count);
		const Item* item = ItemTable::getSingleton()->get(it.first);
		containerStr+= idx;
		containerStr+= " - " + item->getName() + " x" + to_string(it.second) + '\n';
		count++;
	}

	containerTxt.setString(containerStr);

	Game::getWindow().draw(containerTxt);
}

