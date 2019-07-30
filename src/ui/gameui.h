#pragma once

#include "combatui.h"
#include "inventoryui.h"
#include "tradeui.h"

class CombatManager;
class PlayerObject;

class GameUI
{
public:
	GameUI();

	void run(sf::Event event);
	void runCombat(sf::Event event, const CombatManager* manager);
	void runInventory(sf::Event event, PlayerObject* player);
	void runTrade(sf::Event event, std::map<int, int>& inventory, std::map<int, int>& container);
private:
	enum class eGameUIState {
		
		
	};
	CombatUI m_combatUI;
	InventoryUI m_inventoryUI;
	TradeUI m_tradeUI;
};
