#include <iostream>

#include "game.h"
#include "log.h"
#include "creatures/wound.h"
#include "items/weapon.h"
#include "ui/gameui.h"
#include "level/level.h"
#include "gfxobjects/gfxlevel.h"
#include "object/playerobject.h"
#include "object/humanobject.h"
#include "object/selectorobject.h"
#include "gfxobjects/gfxselector.h"
#include "object/relationmanager.h"

using namespace std;

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
	int tick = 0;
	int aiTick = 0;

	GameUI ui;

	Level level(20, 20);

	GFXLevel gfxlevel;
	level.addObject(playerObject);
	level.addObject(humanObject);
	humanObject->setPosition(5, 5);
	ui.initializeCombatUI(&playerObject->getCombatInstance());
	SelectorObject selector;
	GFXSelector gfxSelector;
	while (m_window.isOpen())
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
		sf::Time elapsedTime = clock.getElapsedTime();
		//as milliseconds returns 0, so we have to go more granular
		tick += elapsedTime.asMicroseconds();
		aiTick += elapsedTime.asMicroseconds();
		
		gfxlevel.run(&level);
		ui.run(event);
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
				selector.setPosition(playerObject->getPosition());
				m_currentState = eGameState::AttackMode;
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::I) {
				m_currentState = eGameState::Inventory;
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D) {
				selector.setPosition(playerObject->getPosition());
				m_currentState = eGameState::SelectionMode;
			}
			if(aiTick > 60000) {
				level.run();
				aiTick = 0;
			}
			if(playerObject->isInCombat() == true) {
				m_currentState = eGameState::InCombat;
			}
			
		}
		else if(m_currentState == eGameState::SelectionMode) {
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
				selector.moveDown();
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
				selector.moveUp();
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
				selector.moveLeft();
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
				selector.moveRight();
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
				const Object *object = level.getObject(selector.getPosition());
				if(object == nullptr) {
					std::cout << "Nothing here" << std::endl;
				}
				else
				{
					std::cout << "Something here" << std::endl;
					Log::push("You see " + object->getDescription());
					if(object->getObjectType() == eObjectTypes::Creature) {
						const CreatureObject* creatureObj = static_cast<const CreatureObject*>(object);
						int relation = RelationManager::getSingleton()->getRelationship(eCreatureFaction::Player,
																						creatureObj->getFaction());
						
						if(relation <= RelationManager::cHostile) {
							Log::push(creatureObj->getName() + " is hostile to you", Log::eMessageTypes::Damage);
						}
					}
				}
			}
			gfxSelector.run(&selector);
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D) {
				m_currentState = eGameState::Playing;
			}
		}
		else if(m_currentState == eGameState::AttackMode) {
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down) {
				if(selector.getPosition().y < playerObject->getPosition().y+2) {
					selector.moveDown();
				}
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up) {
				if(selector.getPosition().y > playerObject->getPosition().y-2) {
					selector.moveUp();
				}
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Left) {
				if(selector.getPosition().x > playerObject->getPosition().x-2) {
					selector.moveLeft();
				}
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Right) {
				if(selector.getPosition().x < playerObject->getPosition().x+2) {
					selector.moveRight();
				}
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
				const Object *object = level.getObject(selector.getPosition());
				if(object != nullptr) {
					if(object->getObjectType() == eObjectTypes::Creature) {
						const CreatureObject* creatureObject = static_cast<const CreatureObject*>(object);
						playerObject->startCombatWith(creatureObject->getCreatureComponent());
						m_currentState = eGameState::InCombat;
					}
				}
			
			}
			gfxSelector.run(&selector);
		} else if (m_currentState == eGameState::Inventory) {
			ui.runInventory(event, playerObject);
		}
		

		Log::run();

		if(tick > 105000) {
			if (m_currentState == eGameState::InCombat)
			{
				//pause rest of game if player is in combat. combat between two NPCS can happen anytime
				if(playerObject->runCombat() == false) {
					m_currentState = eGameState::Playing;
				}
				
			}
	
			m_combatManager.run();
			tick = 0;
		}
		
		clock.restart();

		m_window.display();
		level.cleanup();
	}
}
