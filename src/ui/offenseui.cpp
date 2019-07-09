#include "offenseui.h"
#include "types.h"
#include "../game.h"

void OffenseUI::run(sf::Event event, Player* player)
{
	switch(m_currentState) {
	case eUiState::ChooseManuever:
		doManuever(event, player);
		break;
	case eUiState::ChooseComponent:
		doComponent(event, player);
		break;
	case eUiState::ChooseDice:
		doDice(event, player);
		break;
	case eUiState::ChooseTarget:
		doTarget(event, player);
		break;
	case eUiState::Finished:
		break;
	}
}

void OffenseUI::doManuever(sf::Event event, Player* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	
	sf::Text text;
	text.setCharacterSize(cCharSize);
	text.setFont(Game::getDefaultFont());
	text.setString("Choose attack:\na - Swing\nb - Thrust");
	Game::getWindow().draw(text);

	if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		if(c == 'a') {
			player->setOffenseManuever(eOffensiveManuevers::Swing);
			m_currentState = eUiState::ChooseComponent;
		}
		if(c == 'b') {
			player->setOffenseManuever(eOffensiveManuevers::Thrust);
			m_currentState = eUiState::ChooseComponent;				
		}
	}
}

void OffenseUI::doComponent(sf::Event event, Player* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);
	
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
					m_currentState = eUiState::ChooseTarget;
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
					m_currentState = eUiState::ChooseTarget;
				}
			}
		}
	}

	text.setString(str);
	Game::getWindow().draw(text);
}

void OffenseUI::doDice(sf::Event event, Player* player)
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
		player->setOffenseDice(m_numberInput.getNumber());
		m_currentState = eUiState::Finished;
		//last one so set flag
		player->setOffenseReady();
		m_numberInput.reset();
	}

	m_numberInput.setMax(player->getCombatPool());
	m_numberInput.run(event);
	m_numberInput.setPosition(sf::Vector2f(0, cCharSize));

}

void OffenseUI::doTarget(sf::Event event, Player* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize*12));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	sf::Text text;
	text.setCharacterSize(cCharSize);
	text.setFont(Game::getDefaultFont());
	text.setString("Choose target location:\na - Head\nb - Chest\nc - Arms\nd - Belly\ne - Thigh\nf - Shin\n");
	if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		if(c == 'a') {
			player->setOffenseTarget(eHitLocations::Head);
			m_currentState = eUiState::ChooseDice;
		} else if(c == 'b') {
			player->setOffenseTarget(eHitLocations::Chest);
			m_currentState = eUiState::ChooseDice;
		} else if(c == 'c') {
			player->setOffenseTarget(eHitLocations::Arm);
			m_currentState = eUiState::ChooseDice;
		} else if(c == 'd') {
			player->setOffenseTarget(eHitLocations::Head);
			m_currentState = eUiState::ChooseDice;
		} else if(c == 'e') {
			player->setOffenseTarget(eHitLocations::Head);
			m_currentState = eUiState::ChooseDice;
		} else if(c == 'f') {
			player->setOffenseTarget(eHitLocations::Head);
			m_currentState = eUiState::ChooseDice;
		}
	}
	Game::getWindow().draw(text);
}
