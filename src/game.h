#pragma once

#include <SFML/Graphics.hpp>

#include "combatinstance.h"
#include "combatmanager.h"

class Game
{
public:
	static void initialize();
	static void run();
	static void destroy();

	static sf::Font& getDefaultFont() { return m_defaultFont; }
	static sf::RenderWindow& getWindow() { return m_window; }
	static CombatManager& getCombatManager() { return m_combatManager; }
private:
	enum eGameState {
		Uninitialized,
		MainMenu,
		Playing,
		SelectionMode,
		AttackMode,
		Inventory,
		Pickup,
		InCombat,
		PauseMenu,
		Exiting
	};
	static eGameState m_currentState;
	static sf::RenderWindow m_window;
	static sf::Font m_defaultFont;
	static CombatManager m_combatManager;
};
