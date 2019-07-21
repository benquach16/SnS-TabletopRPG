#include "inventoryui.h"
#include "types.h"
#include "../game.h"
#include "../object/creatureobject.h"
#include "../object/playerobject.h"
#include "../creatures/player.h"

using namespace std;

InventoryUI::InventoryUI() : m_uiState(eUiState::Backpack)
{
}
constexpr int cDisplayLines = 24;

void InventoryUI::run(sf::Event event, PlayerObject* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	Player* playerComponent = static_cast<Player*>(player->getCreatureComponent());

	sf::Text txt;
	txt.setFont(Game::getDefaultFont());
	txt.setCharacterSize(cCharSize);
	txt.setString("Backpack");
	Game::getWindow().draw(txt);
}

void InventoryUI::doEquipped(sf::Event event, PlayerObject* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x/2, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	sf::Text txt;
	txt.setFont(Game::getDefaultFont());
	txt.setCharacterSize(cCharSize);
	txt.setString("Equipped Armor");
	Game::getWindow().draw(txt);	
}

void InventoryUI::displayDetail(sf::Event event, int id)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x/2, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);	
}
