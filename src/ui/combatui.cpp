#include <iostream>

#include "combatui.h"
#include "../game.h"
#include "../combatmanager.h"
#include "../items/weapon.h"
#include "../creatures/player.h"
#include "types.h"

using namespace std;

constexpr unsigned logHeight = cCharSize * (cLinesDisplayed+1);
constexpr unsigned rectHeight = cCharSize * 5;

CombatUI::CombatUI() : m_manager(nullptr)
{
	resetState();
}

void CombatUI::initialize(CombatManager* manager)
{	
	m_manager = manager;
}

void CombatUI::resetState()
{
	m_initiativeState = eInitiativeSubState::ChooseInitiative;
	m_offenseState = eOffenseSubState::ChooseManuever;
	m_stolenOffenseState = eStolenOffenseSubState::ChooseDice;
	m_defenseState = eDefenseSubState::ChooseManuever;
}

void CombatUI::run(sf::Event event)
{
	if(m_manager == nullptr) {
		return;
	}

	if(m_manager->getState() == eCombatState::Uninitialized) {
		return;
	}

	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape combatBkg(sf::Vector2f(windowSize.x/2 - 6, rectHeight));
	combatBkg.setPosition(3, windowSize.y - logHeight - rectHeight - 3);
	combatBkg.setFillColor(sf::Color(12, 12, 23));
	combatBkg.setOutlineThickness(3);
	combatBkg.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(combatBkg);
	sf::RectangleShape combatBkg2(sf::Vector2f(windowSize.x/2 - 6, rectHeight));
	combatBkg2.setPosition(windowSize.x/2 + 3, windowSize.y - logHeight - rectHeight - 3);
	combatBkg2.setFillColor(sf::Color(12, 12, 23));
	combatBkg2.setOutlineThickness(3);
	combatBkg2.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(combatBkg2);

	showSide1Stats();
	showSide2Stats();
	
	if(m_manager->getState() == eCombatState::Initialized) {
		resetState();
	}
	if(m_manager->getState() == eCombatState::RollInitiative) {
		doInitiative();
	}
	if(m_manager->getState() == eCombatState::ResetState) {
		resetState();
	}
	if(m_manager->getState() == eCombatState::Offense && m_manager->isAttackerPlayer() == true) {
		doOffense(event);
	}
	if(m_manager->getState() == eCombatState::Defense && m_manager->isDefenderPlayer() == true) {
		doDefense(event);
	}
	if(m_manager->getState() == eCombatState::DualOffense1 && m_manager->isAttackerPlayer() == true) {
		doOffense(event);
	}
	if(m_manager->getState() == eCombatState::DualOffense2 && m_manager->isAttackerPlayer() == true) {
		doOffense(event);
	}
	if(m_manager->getState() == eCombatState::StolenOffense) {
		doStolenOffense(event);
	}
	if(m_manager->getState() == eCombatState::Resolution) {
		resetState();
	}
}

void CombatUI::doInitiative()
{
	auto windowSize = Game::getWindow().getSize();
	if(m_initiativeState == eInitiativeSubState::ChooseInitiative) {
		sf::RectangleShape chooseInitiative(sf::Vector2f(windowSize.x, cCharSize*12));
		chooseInitiative.setFillColor(sf::Color(12, 12, 23));
		Game::getWindow().draw(chooseInitiative);

		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Choose initiative:\na - attack \nb - defend");
		Game::getWindow().draw(text);

		assert(m_manager->getSide1()->isPlayer() == true);
		Player* player = static_cast<Player*>(m_manager->getSide1());
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) == true) {
			player->setInitiative(eInitiativeRoll::Attack);
			m_initiativeState = eInitiativeSubState::Finished;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::B) == true) {
			player->setInitiative(eInitiativeRoll::Defend);
			m_initiativeState = eInitiativeSubState::Finished;
		}
	}
}

void CombatUI::doOffense(sf::Event event)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	Player* player = static_cast<Player*>(m_manager->getSide1());
	if(m_offenseState == eOffenseSubState::ChooseManuever) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Choose attack:\na - Swing\nb - Thrust");
		Game::getWindow().draw(text);

		if(event.type == sf::Event::TextEntered) {
			char c = event.text.unicode;
			if(c == 'a') {
				player->setOffenseManuever(eOffensiveManuevers::Swing);
				m_offenseState = eOffenseSubState::ChooseComponent;
			}
			if(c == 'b') {
				player->setOffenseManuever(eOffensiveManuevers::Thrust);
				m_offenseState = eOffenseSubState::ChooseComponent;				
			}
		}
	}
	else if(m_offenseState == eOffenseSubState::ChooseComponent) {
		Weapon* weapon = player->getPrimaryWeapon();
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		std::string str("Choose weapon component:\n");
		
		if(player->getQueuedOffense().manuever == eOffensiveManuevers::Swing) {
			for(int i = 0; i < weapon->getSwingComponents().size(); ++i) {
				char idx = ('a' + i);

				str += idx;
				str += " - " + weapon->getSwingComponents()[i]->getName() + '\n';

				if(event.type == sf::Event::TextEntered) {
					char c = event.text.unicode;
					if(c == idx) {
						player->setOffenseComponent(weapon->getSwingComponents()[i]);
						m_offenseState = eOffenseSubState::ChooseTarget;
					}
				}
			}
		} else {
			for(int i = 0; i < weapon->getThrustComponents().size(); ++i) {
				char idx = ('a' + i);

				str += idx;
				str += " - " + weapon->getThrustComponents()[i]->getName() + '\n';

				if(event.type == sf::Event::TextEntered) {
					char c = event.text.unicode;
					if(c == idx) {
						player->setOffenseComponent(weapon->getThrustComponents()[i]);
						m_offenseState = eOffenseSubState::ChooseTarget;
					}
				}
			}
		}

		text.setString(str);
		Game::getWindow().draw(text);
	}
	else if(m_offenseState == eOffenseSubState::ChooseTarget) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Choose target location:\na - Head\nb - Chest\nc - Arms\nd - Belly\ne - Thigh\nf - Shin\n");
		if(event.type == sf::Event::TextEntered) {
			char c = event.text.unicode;
			if(c == 'a') {
				player->setOffenseTarget(eHitLocations::Head);
				m_offenseState = eOffenseSubState::ChooseDice;
			} else if(c == 'b') {
				player->setOffenseTarget(eHitLocations::Chest);
				m_offenseState = eOffenseSubState::ChooseDice;
			} else if(c == 'c') {
				player->setOffenseTarget(eHitLocations::Arm);
				m_offenseState = eOffenseSubState::ChooseDice;
			} else if(c == 'd') {
				player->setOffenseTarget(eHitLocations::Head);
				m_offenseState = eOffenseSubState::ChooseDice;
			} else if(c == 'e') {
				player->setOffenseTarget(eHitLocations::Head);
				m_offenseState = eOffenseSubState::ChooseDice;
			} else if(c == 'f') {
				player->setOffenseTarget(eHitLocations::Head);
				m_offenseState = eOffenseSubState::ChooseDice;
			}
		}
		Game::getWindow().draw(text);
	}
	else if(m_offenseState == eOffenseSubState::ChooseDice) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Allocate action points (" + std::to_string(player->getCombatPool()) + " action points left):");

		Game::getWindow().draw(text);

		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
			player->setOffenseDice(m_numberInput.getNumber());
			m_offenseState = eOffenseSubState::Finished;
			//last one so set flag
			player->setOffenseReady();
			m_numberInput.reset();
		}

		m_numberInput.setMax(player->getCombatPool());
		m_numberInput.run(event);
		m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
	}
	else if(m_offenseState == eOffenseSubState::Finished) {
		
	}
}

void CombatUI::doDefense(sf::Event event)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	//player side 1 for now
	Player* player = static_cast<Player*>(m_manager->getSide1());
	if(m_defenseState == eDefenseSubState::ChooseManuever) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Choose defense:\na - Parry\nb - Dodge\nc - Linked Parry\nd - Steal initiative\ne - Attack out of initiative");
		Game::getWindow().draw(text);
		if(event.type == sf::Event::TextEntered) {
			char c = event.text.unicode;
			if(c == 'a') {
				player->setDefenseManuever(eDefensiveManuevers::Parry);
				m_defenseState = eDefenseSubState::ChooseDice;
			}
			if(c == 'b') {
				player->setDefenseManuever(eDefensiveManuevers::Dodge);
				m_defenseState = eDefenseSubState::ChooseDice;
			}
		}		
	}
	else if(m_defenseState == eDefenseSubState::ChooseDice) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Allocate action points (" + std::to_string(player->getCombatPool()) + " action points left):");

		Game::getWindow().draw(text);

		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
			player->setDefenseDice(m_numberInput.getNumber());
			m_defenseState = eDefenseSubState::Finished;
			//last one so set flag
			player->setDefenseReady();
			m_numberInput.reset();
		}

		m_numberInput.setMax(player->getCombatPool());
		m_numberInput.run(event);
		m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
	}
}

void CombatUI::doStolenOffense(sf::Event event)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	Player* player = static_cast<Player*>(m_manager->getSide1());

	if(m_stolenOffenseState == eStolenOffenseSubState::ChooseDice) {
		
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Initiative roll, allocate action points (" + std::to_string(player->getCombatPool()) + " action points left):");

		Game::getWindow().draw(text);

		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
			player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
			player->setDefenseDice(m_numberInput.getNumber());
			//last one so set flag
			player->setDefenseReady();
			m_numberInput.reset();
			m_stolenOffenseState = eStolenOffenseSubState::Finished;
		}

		m_numberInput.setMax(player->getCombatPool());
		m_numberInput.run(event);
		m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
	}
}

void CombatUI::showSide1Stats()
{
	auto windowSize = Game::getWindow().getSize();

	assert(m_manager != nullptr);
	Creature* creature = m_manager->getSide1();
	sf::Text side1Info;
	side1Info.setString(creature->getName() + " - " + creature->getPrimaryWeapon()->getName());
	side1Info.setCharacterSize(cCharSize);
	side1Info.setFont(Game::getDefaultFont());
	side1Info.setPosition(6, windowSize.y - logHeight - rectHeight);
	
	Game::getWindow().draw(side1Info);

	sf::Text ap;
	ap.setCharacterSize(cCharSize);
	ap.setFont(Game::getDefaultFont());
	ap.setPosition(6, windowSize.y - logHeight - rectHeight + cCharSize);
	ap.setString("Action Points : " + to_string(creature->getCombatPool()) +
				 " - Success rate: " + to_string(creature->getSuccessRate()) + "%");
	
	Game::getWindow().draw(ap);
}

void CombatUI::showSide2Stats()
{
	auto windowSize = Game::getWindow().getSize();

	assert(m_manager != nullptr);
	Creature* creature = m_manager->getSide2();
	sf::Text side1Info;
	side1Info.setString(creature->getName() + " - " + creature->getPrimaryWeapon()->getName());
	side1Info.setCharacterSize(cCharSize);
	side1Info.setFont(Game::getDefaultFont());
	side1Info.setPosition(windowSize.x/2, windowSize.y - logHeight - rectHeight);
	
	Game::getWindow().draw(side1Info);

	sf::Text ap;
	ap.setCharacterSize(cCharSize);
	ap.setFont(Game::getDefaultFont());
	ap.setPosition(windowSize.x/2, windowSize.y - logHeight - rectHeight + cCharSize);
	ap.setString("Action Points : " + to_string(creature->getCombatPool()) +
				 " - Success rate: " + to_string(creature->getSuccessRate()) + "%");
	
	Game::getWindow().draw(ap);
}
