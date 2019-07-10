#include <iostream>

#include "combatui.h"
#include "../game.h"
#include "../combatinstance.h"
#include "../items/weapon.h"
#include "../creatures/player.h"
#include "types.h"

using namespace std;

constexpr unsigned logHeight = cCharSize * (cLinesDisplayed+1);
constexpr unsigned rectHeight = cCharSize * 5;

CombatUI::CombatUI() : m_instance(nullptr)
{
	resetState();
}

void CombatUI::resetState()
{
	m_defenseUI.resetState();
	m_offenseUI.resetState();
	m_initiativeState = eInitiativeSubState::ChooseInitiative;
	m_offenseState = eOffenseSubState::ChooseManuever;
	m_stolenOffenseState = eStolenOffenseSubState::ChooseDice;
}

void CombatUI::run(sf::Event event)
{
	if(m_instance == nullptr) {
		return;
	}

	if(m_instance->getState() == eCombatState::Uninitialized) {
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

	Player* player = static_cast<Player*>(m_instance->getSide1());
	
	if(m_instance->getState() == eCombatState::Initialized) {
		resetState();
		return;
	}
	if(m_instance->getState() == eCombatState::RollInitiative) {
		doInitiative();
		return;
	}
	if(m_instance->getState() == eCombatState::ResetState) {
		resetState();
		return;
	}
	if(m_instance->getState() == eCombatState::Offense && m_instance->isAttackerPlayer() == true) {
		m_offenseUI.run(event, player);
		return;
	}
	if(m_instance->getState() == eCombatState::Defense && m_instance->isDefenderPlayer() == true) {
		m_defenseUI.run(event, player);
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffense1 && m_instance->isAttackerPlayer() == true) {
		m_offenseUI.run(event, player);
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffense2 && m_instance->isAttackerPlayer() == true) {
		m_offenseUI.run(event, player);
		return;
	}
	if(m_instance->getState() == eCombatState::StolenOffense && m_instance->isAttackerPlayer() == true) {
		doStolenOffense(event);
		return;
	}
	if(m_instance->getState() == eCombatState::StealInitiative) {
		m_offenseUI.run(event, player);
		return;
	}
	if(m_instance->getState() == eCombatState::Resolution) {
		resetState();
		m_defenseUI.resetState();
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffenseResolve) {
		resetState();
		return;
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

		assert(m_instance->getSide1()->isPlayer() == true);
		Player* player = static_cast<Player*>(m_instance->getSide1());
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

void CombatUI::doStolenOffense(sf::Event event)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	Player* player = static_cast<Player*>(m_instance->getSide1());

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

	assert(m_instance != nullptr);
	Creature* creature = m_instance->getSide1();
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

	assert(m_instance != nullptr);
	Creature* creature = m_instance->getSide2();
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
