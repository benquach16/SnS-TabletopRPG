#include "gameui.h"

GameUI::GameUI()
{
	
}

void GameUI::run(sf::Event event)
{

}

void GameUI::runCombat(sf::Event event)
{
	m_combatUI.run(event);
}

void GameUI::runInventory(sf::Event event, PlayerObject* player)
{
	m_inventoryUI.run(event, player);
}

void GameUI::runTrade(sf::Event event, std::map<int, int>& inventory, std::map<int, int>& container)
{
	m_tradeUI.run(event, inventory, container);
}

