#include "game.h"
#include "log.h"
#include "creatures/wound.h"
#include "items/weapon.h"
#include "ui/gameui.h"
#include "level/level.h"
#include "gfxobjects/gfxlevel.h"
#include "object/playerobject.h"
#include "object/humanobject.h"

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
	CombatInstance instance;
	PlayerObject* playerObject = new PlayerObject;
	HumanObject* humanObject = new HumanObject;

	//instance.initCombat(c1, c2);

	sf::Clock clock;
	//main game loop
	float tick = 0;

	GameUI ui;

	Level level(20, 20);
	
	GFXLevel gfxlevel;
	gfxlevel.setLevel(&level);
	level.addObject(playerObject);
	level.addObject(humanObject);
	humanObject->setPosition(5, 5);
	//ui.initializeCombatUI(&instance);
	while(m_window.isOpen())
	{
		m_window.clear();
		
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed) {
				m_window.close();
			}

		}

		vector2d pos = playerObject->getPosition();
		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
			if(level.isFreeSpace(pos.x, pos.y + 1) == true) {
				playerObject->moveDown();
			}
		}

		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
			if(level.isFreeSpace(pos.x, pos.y - 1) == true) {
				playerObject->moveUp();
			}
		}
		
		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
			if(level.isFreeSpace(pos.x - 1, pos.y) == true) {
				playerObject->moveLeft();
			}
		}

		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
			if(level.isFreeSpace(pos.x + 1, pos.y) == true) {
				playerObject->moveRight();
			}
		}		
		gfxlevel.run();
		sf::Time elapsedTime = clock.getElapsedTime();
		tick += elapsedTime.asSeconds();
		ui.run(event);
		Log::run();

		if(tick > 1.0) {
			instance.run();
			tick = 0;
		}
		
		clock.restart();

		m_window.display();
	}
}
