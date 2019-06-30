#pragma once

#include <SFML/Graphics.hpp>

#include "combatmanager.h"

class Game
{
public:
	Game();

	static void initialize();
	static void run();
	static void destroy();

	static sf::Font& getDefaultFont() { return m_defaultFont; }
	static sf::RenderWindow& getWindow() { return m_window; }
private:
	enum eGameState {
		Uninitialized,
		Playing,
		Exiting
	};
	static sf::RenderWindow m_window;
	static sf::Font m_defaultFont;
};
