#include "gameui.h"

GameUI::GameUI()
{
	
}

void GameUI::run(sf::Event event)
{
	m_combatUI.run(event);
}

void GameUI::runInventory(sf::Event event, PlayerObject* player)
{
	m_inventoryUI.run(event, player);
}


