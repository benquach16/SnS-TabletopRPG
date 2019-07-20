#pragma once

#include "combatui.h"
#include "inventoryui.h"

class CombatManager;

class GameUI
{
public:
	GameUI();
	void initializeCombatUI(CombatInstance* instance) { m_combatUI.initialize(instance); }

	void run(sf::Event event);
	void runInventory(sf::Event event);
private:
	enum class eGameUIState {
		
		
	};
	CombatUI m_combatUI;
	InventoryUI m_inventoryUI;
};
