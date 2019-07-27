#include <iostream>

#include "combatui.h"
#include "../game.h"
#include "../combatinstance.h"
#include "../items/weapon.h"
#include "../items/utils.h"
#include "../creatures/player.h"
#include "../creatures/utils.h"
#include "types.h"
#include "common.h"

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
	m_stolenOffenseState = eStolenOffenseSubState::ChooseDice;
	m_dualRedState = eDualRedStealSubState::ChooseInitiative;
}

void CombatUI::run(sf::Event event)
{
	if(m_instance == nullptr) {
		cout << "this shouldnt happen" << endl;
		return;
	}

	if(m_instance->getState() == eCombatState::Uninitialized) {
		return;
	}

	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape combatBkg(sf::Vector2f(windowSize.x/2 - 4, rectHeight));
	combatBkg.setPosition(2, windowSize.y - logHeight - rectHeight - 3);
	combatBkg.setFillColor(sf::Color(12, 12, 23));
	combatBkg.setOutlineThickness(2);
	combatBkg.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(combatBkg);
	sf::RectangleShape combatBkg2(sf::Vector2f(windowSize.x/2 - 2, rectHeight));
	combatBkg2.setPosition(windowSize.x/2, windowSize.y - logHeight - rectHeight - 3);
	combatBkg2.setFillColor(sf::Color(12, 12, 23));
	combatBkg2.setOutlineThickness(2);
	combatBkg2.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(combatBkg2);

	showSide1Stats();
	showSide2Stats();

	assert(m_instance->getSide1()->isPlayer() == true);
	Player *player = static_cast<Player *>(m_instance->getSide1());
	Creature *target = m_instance->getSide2();

	sf::RectangleShape reachBkg(sf::Vector2f(windowSize.x - 4, cCharSize));
	reachBkg.setFillColor(sf::Color(12, 12, 23));
	reachBkg.setPosition(2, windowSize.y - logHeight - rectHeight - cCharSize - 2);
	reachBkg.setOutlineThickness(2);
	reachBkg.setOutlineColor(sf::Color(22, 22, 33));
	int reachCost = static_cast<int>(m_instance->getCurrentReach()) -
		static_cast<int>(player->getPrimaryWeapon()->getLength());
	reachCost = abs(reachCost);
	sf::Text reachTxt;
	reachTxt.setCharacterSize(cCharSize);
	reachTxt.setFont(Game::getDefaultFont());
	reachTxt.setString("Current reach is " + lengthToString(m_instance->getCurrentReach()) + " (" +
					   to_string(reachCost) + "AP to attack)");
	reachTxt.setPosition(5, windowSize.y - logHeight - rectHeight - cCharSize - 6);
	Game::getWindow().draw(reachBkg);
	Game::getWindow().draw(reachTxt);

	if (m_instance->getState() == eCombatState::Initialized)
	{
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
		m_offenseUI.run(event, player, target);
		return;
	}
	if(m_instance->getState() == eCombatState::Defense && m_instance->isDefenderPlayer() == true) {
		m_defenseUI.run(event, player);
		return;
	}
	if(m_instance->getState() == eCombatState::ParryLinked) {
		m_offenseUI.run(event, player, target, false, true);
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffense1 && m_instance->isAttackerPlayer() == true) {
		if(m_dualRedState == eDualRedStealSubState::Finished) {
			m_offenseUI.run(event, player, target, false);
		} else {
			doDualRedSteal(event);
		}
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffense2 && m_instance->isAttackerPlayer() == true) {
		if(m_dualRedState == eDualRedStealSubState::Finished) {
			m_offenseUI.run(event, player, target, false);
		} else {
			doDualRedSteal(event);
		}
		return;
	}
	if(m_instance->getState() == eCombatState::StolenOffense && m_instance->isAttackerPlayer() == true) {
		doStolenOffense(event);
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffenseStealInitiative && m_instance->isDefenderPlayer() == true) {
		if(m_stolenOffenseState == eStolenOffenseSubState::Finished) {
			m_offenseUI.run(event, player, target);
		} else {
			doStolenOffense(event);
		}
		return;
	}
	if(m_instance->getState() == eCombatState::DualOffenseSecondInitiative && m_instance->isDefenderPlayer() == true) {
		doStolenOffense(event);	
		return;
	}
	if(m_instance->getState() == eCombatState::StealInitiative) {
		m_offenseUI.run(event, player, target);
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
	if(m_instance->getState() == eCombatState::FinishedCombat) {
		return;
	}
}

void CombatUI::doInitiative()
{
	if(m_initiativeState == eInitiativeSubState::ChooseInitiative) {
		UiCommon::drawTopPanel();

		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Choose initiative:\na - Attack \nb - Defend\nc - Inspect target");
		Game::getWindow().draw(text);

		//temporary, maybe
		assert(m_instance->getSide1()->isPlayer() == true);
		Player* player = static_cast<Player*>(m_instance->getSide1());
		Creature *target = m_instance->getSide2();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) == true) {
			player->setInitiative(eInitiativeRoll::Attack);
			m_initiativeState = eInitiativeSubState::Finished;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::B) == true) {
			player->setInitiative(eInitiativeRoll::Defend);
			m_initiativeState = eInitiativeSubState::Finished;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::C) == true) {
			m_initiativeState = eInitiativeSubState::InspectTarget;
		}
	} 
	else if(m_initiativeState == eInitiativeSubState::InspectTarget) {
		UiCommon::drawTopPanel();

		Creature *target = m_instance->getSide2();
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString(target->getName() + " looks aggressive");
		Game::getWindow().draw(text);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) == true) {
			m_initiativeState = eInitiativeSubState::ChooseInitiative;
		}		
	}
}

void CombatUI::doStolenOffense(sf::Event event)
{
	UiCommon::drawTopPanel();
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

void CombatUI::doDualRedSteal(sf::Event event)
{
	UiCommon::drawTopPanel();

	Player* player = static_cast<Player*>(m_instance->getSide1());
	if(m_dualRedState == eDualRedStealSubState::ChooseInitiative) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Steal Initiative?\na - Yes\nb - No");
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) == true) {
			player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
			m_dualRedState = eDualRedStealSubState::ChooseDice;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::B) == true) {
			m_dualRedState = eDualRedStealSubState::Finished;
		}
		Game::getWindow().draw(text);
	} else if (m_dualRedState == eDualRedStealSubState::ChooseDice) {
		sf::Text text;
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setString("Initiative roll, allocate action points (" + std::to_string(player->getCombatPool()) + " action points left):");

		Game::getWindow().draw(text);

		if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
			player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
			player->setDefenseDice(m_numberInput.getNumber());
			player->reduceCombatPool(m_numberInput.getNumber());
			//last one so set flag
			player->setDefenseReady();
			m_numberInput.reset();
			m_dualRedState = eDualRedStealSubState::Finished;
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
	side1Info.setString(creature->getName() + " - " + creature->getPrimaryWeapon()->getName() + " - " +
						lengthToString(creature->getPrimaryWeapon()->getLength()));
	side1Info.setCharacterSize(cCharSize);
	side1Info.setFont(Game::getDefaultFont());
	side1Info.setPosition(6, windowSize.y - logHeight - rectHeight);
	
	Game::getWindow().draw(side1Info);

	sf::Text ap;
	ap.setCharacterSize(cCharSize);
	ap.setFont(Game::getDefaultFont());
	ap.setPosition(6, windowSize.y - logHeight - rectHeight + cCharSize);
	ap.setString("Action Points : " + to_string(creature->getCombatPool()) +
				 " - Success rate: " + to_string(creature->getSuccessRate()) + "%" +
				 '\n' + "Blood loss: " + to_string(creature->getBloodLoss()) + '\n' +
				 stanceToString(creature->getStance()));
	
	Game::getWindow().draw(ap);
}

void CombatUI::showSide2Stats()
{
	auto windowSize = Game::getWindow().getSize();

	assert(m_instance != nullptr);
	Creature* creature = m_instance->getSide2();
	sf::Text side1Info;
	side1Info.setString(creature->getName() + " - " + creature->getPrimaryWeapon()->getName() + " - " +
						lengthToString(creature->getPrimaryWeapon()->getLength()));
	side1Info.setCharacterSize(cCharSize);
	side1Info.setFont(Game::getDefaultFont());
	side1Info.setPosition(windowSize.x/2 + 5, windowSize.y - logHeight - rectHeight);
	
	Game::getWindow().draw(side1Info);

	sf::Text ap;
	ap.setCharacterSize(cCharSize);
	ap.setFont(Game::getDefaultFont());
	ap.setPosition(windowSize.x/2 + 5, windowSize.y - logHeight - rectHeight + cCharSize);
	ap.setString("Action Points : " + to_string(creature->getCombatPool()) +
				 " - Success rate: " + to_string(creature->getSuccessRate()) + "%"+
				 '\n' + "Blood loss: " + to_string(creature->getBloodLoss()) + '\n' +
				 stanceToString(creature->getStance()));
	
	Game::getWindow().draw(ap);
}
