#include "gameui.h"

GameUI::GameUI()
{
	
}

void GameUI::run(sf::Event event)
{
	m_combatUI.run(event);

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
		
	}
}


