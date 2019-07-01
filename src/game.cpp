#include "game.h"
#include "log.h"
#include "creatures/wound.h"
#include "combatmanager.h"
#include "weapons/weapon.h"
#include "creatures/human.h"

sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;

Game::Game()
{
}

void Game::initialize()
{
	m_window.create(sf::VideoMode(1600, 900), "window");
	m_defaultFont.loadFromFile("data/fonts/Consolas.ttf");
}

void Game::run()
{
	CombatManager manager;
	Human* c1 = new Human;
	Human* c2 = new Human;
	c1->setWeapon(40); //pollax
	c2->setWeapon(41); //arming sword
	c1->setName("John");
	c2->setName("Sam");
	manager.initCombat(c1, c2);

	sf::Clock clock;
	//main game loop
	float tick = 0;
	while(m_window.isOpen())
	{
		m_window.clear();

		sf::Time elapsedTime = clock.getElapsedTime();
		tick += elapsedTime.asSeconds();

		if(tick > 2.0) {
			manager.run();
			tick = 0;
		}
		
		clock.restart();
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed) {
				m_window.close();
			}

		}
		Log::run();

		m_window.display();
	}
}
