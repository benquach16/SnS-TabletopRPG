#include "game.h"
#include "log.h"
#include "creatures/wound.h"
#include "items/weapon.h"
#include "ui/gameui.h"
#include "level/level.h"
#include "gfxobjects/gfxlevel.h"
#include "object/playerobject.h"
#include "object/humanobject.h"

Game::eGameState Game::m_currentState;
sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;
CombatManager Game::m_combatManager;

void Game::initialize()
{
	m_window.create(sf::VideoMode(1600, 900), "window");
	m_defaultFont.loadFromFile("data/fonts/Consolas.ttf");
	m_currentState = eGameState::Playing;
}

void Game::run()
{
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
	ui.initializeCombatUI(&playerObject->getCombatInstance());
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

		if(m_currentState == eGameState::Playing) {
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
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
				playerObject->startCombatWith(humanObject->getCreatureComponent());
				m_currentState = eGameState::InCombat;
			}			
		}
		level.run();
		gfxlevel.run();
		sf::Time elapsedTime = clock.getElapsedTime();
		tick += elapsedTime.asSeconds();
		ui.run(event);
		Log::run();

		if(tick > 1.0) {
			if(m_currentState == eGameState::InCombat) {
				//pause rest of game if player is in combat. combat between two NPCS can happen anytime
				playerObject->runCombat();
			}
			m_combatManager.run();
			tick = 0;
		}



		
		clock.restart();

		m_window.display();
	}
}
