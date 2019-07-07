#pragma once

#include "combatui.h"

class CombatManager;

class GameUI
{
public:
	GameUI();
	void initializeCombatUI(CombatManager* manager) { m_combatUI.initialize(manager); }

	void run(sf::Event event);
private:
	CombatUI m_combatUI;
};
