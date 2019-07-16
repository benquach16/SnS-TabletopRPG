#include "defenseui.h"
#include "../game.h"
#include "types.h"

void DefenseUI::run(sf::Event event, Player* player)
{
	switch(m_currentState) {
	case eUiState::ChooseManuever:
		doManuever(event, player);
		break;
	case eUiState::ChooseDice:
		doChooseDice(event, player);
		break;
	case eUiState::Finished:
		break;
	}
}


void DefenseUI::doManuever(sf::Event event, Player* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	
	sf::Text text;
	text.setCharacterSize(cCharSize);
	text.setFont(Game::getDefaultFont());
	text.setString("Choose defense:\na - Parry\nb - Dodge\nc - Linked Parry (1AP)\nd - Steal Initiative\ne - Counter (2AP)");
	Game::getWindow().draw(text);
	if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		if(c == 'a') {
			player->setDefenseManuever(eDefensiveManuevers::Parry);
			m_currentState = eUiState::ChooseDice;
		}
		if(c == 'b') {
			player->setDefenseManuever(eDefensiveManuevers::Dodge);
			m_currentState = eUiState::ChooseDice;
		}
		if(c == 'c') {
			//costs 1 die
			if(player->getCombatPool() <= 0) {	
				Log::push("Requires 1 action point");
			} else {
				player->setDefenseManuever(eDefensiveManuevers::ParryLinked);
				player->reduceCombatPool(1);
				m_currentState = eUiState::ChooseDice;
			}
		}
		if(c == 'd') {
			player->setDefenseManuever(eDefensiveManuevers::StealInitiative);
			m_currentState = eUiState::ChooseDice;
		}
		if(c == 'e') {
			if(player->getCombatPool() <= 1) {
				Log::push("Requires 2 action points");
			} else {
				player->setDefenseManuever(eDefensiveManuevers::Counter);
				player->reduceCombatPool(2);
				m_currentState = eUiState::ChooseDice;
			}
		}
	}	
}

void DefenseUI::doChooseDice(sf::Event event, Player* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	
	sf::Text text;
	text.setCharacterSize(cCharSize);
	text.setFont(Game::getDefaultFont());
	text.setString("Allocate action points (" + std::to_string(player->getCombatPool()) + " action points left):");
	Game::getWindow().draw(text);

	if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
		player->setDefenseDice(m_numberInput.getNumber());
		m_currentState = eUiState::Finished;
		//last one so set flag
		player->setDefenseReady();
		m_numberInput.reset();
	}

	m_numberInput.setMax(player->getCombatPool());
	m_numberInput.run(event);
	m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}

void DefenseUI::doLinkedManuever(sf::Event event, Player* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
}
