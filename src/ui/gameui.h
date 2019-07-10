#pragma once

#include "combatui.h"

class CombatManager;

class GameUI
{
public:
	GameUI();
	void initializeCombatUI(CombatInstance* instance) { m_combatUI.initialize(instance); }

	void run(sf::Event event);
private:
	CombatUI m_combatUI;
};
