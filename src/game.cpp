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
#include "gfxobjects/utils.h"
#include "object/relationmanager.h"

using namespace std;

Game::eGameState Game::m_currentState;
sf::RenderWindow Game::m_window;
sf::Font Game::m_defaultFont;
CombatManager Game::m_combatManager;

void Game::initialize()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	m_window.create(sf::VideoMode(1600, 900), "window", sf::Style::Default, settings);
	m_window.setFramerateLimit(165);
	m_defaultFont.loadFromFile("data/fonts/MorePerfectDOSVGA.ttf");
	m_currentState = eGameState::Playing;
}

void Game::run()
{
	//clean me up
	PlayerObject* playerObject = new PlayerObject;
	HumanObject* humanObject = new HumanObject;
	HumanObject* humanObject2 = new HumanObject;
	//instance.initCombat(c1, c2);

	sf::Clock clock;

	//main game loop
	float tick = 0;
	float aiTick = 0;

	GameUI ui;

	Level level(40, 40);
	for(int i = 0; i < 40; i++) {
		level(i, 0).m_type = eTileType::Wall;
	}

	for(int i = 2; i < 40; i++) {
		level(0, i).m_type = eTileType::Wall;
	}
	level.generate();
	GFXLevel gfxlevel;
	level.addObject(playerObject);
	level.addObject(humanObject);
	level.addObject(humanObject2);
	playerObject->setPosition(1, 1);
	humanObject->setPosition(6, 6);
	humanObject2->setPosition(20, 20);
	ui.initializeCombatUI(&playerObject->getCombatInstance());
	SelectorObject selector;
	GFXSelector gfxSelector;
	
	Object* pickup = nullptr;

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
		tick += elapsedTime.asSeconds();
		aiTick += elapsedTime.asSeconds();
		
		sf::View v = getWindow().getDefaultView();
		v.setSize(v.getSize().x, v.getSize().y*2);
		//v.setCenter(v.getSize() *.5f);
		sf::Vector2f center(playerObject->getPosition().x, playerObject->getPosition().y);
		center = coordsToScreen(center);
		v.setCenter(center.x, center.y + 200);
		getWindow().setView(v);	
		gfxlevel.run(&level, playerObject->getPosition());
		//temporary until we get graphics queue up and running
		if(m_currentState == eGameState::AttackMode || m_currentState == eGameState::SelectionMode) {
			gfxSelector.run(&selector);
		}
		getWindow().setView(getWindow().getDefaultView());
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
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P) {
				Object *object = level.getObjectMutable(playerObject->getPosition(), playerObject);
				if(object != nullptr) {
					switch(object->getObjectType()) {
					case eObjectTypes::Corpse:
						Log::push("Searching corpse..");
						pickup = object;
						m_currentState = eGameState::Pickup;
						break;
					case eObjectTypes::Creature:
						Log::push("There is a creature here. You need to kill them if you want to loot them.");
						break;
					}
				} else {
					Log::push("There is nothing here.");
				}
			}
			if(aiTick > 0.3) {
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
						//don't do anything more for player
						if(creatureObj->isPlayer() == false) {
							if(creatureObj->isConscious() == false) {
								Log::push("They are unconscious", Log::eMessageTypes::Announcement);
							}
							int relation = RelationManager::getSingleton()->getRelationship(eCreatureFaction::Player,
																							creatureObj->getFaction());
						
							if(relation <= RelationManager::cHostile) {
								Log::push(creatureObj->getName() + " is hostile to you", Log::eMessageTypes::Damage);
							}
						}
					}
				}
			}

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
						if(creatureObject->isConscious() == true) {
							playerObject->startCombatWith(creatureObject->getCreatureComponent());
							m_currentState = eGameState::InCombat;
						} else {
							creatureObject->kill();
							Log::push("You finish off the unconscious creature");
						}
						
					}
				}
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
				m_currentState = eGameState::Playing;
			}
			gfxSelector.run(&selector);
		} else if (m_currentState == eGameState::Inventory) {
			ui.runInventory(event, playerObject);
			if(event.type == sf::Event::KeyReleased && event.key.code ==sf::Keyboard::I) {
				m_currentState = eGameState::Playing;
			}
		} else if(m_currentState == eGameState::Pickup) {
			assert(pickup != nullptr);
			ui.runTrade(event, playerObject->getInventoryMutable(), pickup->getInventoryMutable());
			if(event.type == sf::Event::KeyReleased && event.key.code ==sf::Keyboard::P) {
				m_currentState = eGameState::Playing;
			}

		} else if(m_currentState == eGameState::InCombat) {
			ui.runCombat(event);
		}

		Log::run();
		if(tick > 0.7) {
			if (m_currentState == eGameState::InCombat)
			{
				//pause rest of game if player is in combat. combat between two NPCS can happen anytime
				if(playerObject->runCombat() == false) {
					m_currentState = eGameState::Playing;
				}
				
			}
	
			//m_combatManager.run();
			tick = 0;
		}
		
		float currentTime = clock.restart().asSeconds();
		float fps = 1.f / currentTime;
		//cout << "FPS"  << fps << endl;
		m_window.setTitle(std::to_string(fps));
		m_window.display();
		level.cleanup();
	}

	
}
