#include <algorithm>
#include <iostream>
#include "log.h"
#include "ui/types.h"
#include "game.h"

using namespace std;

std::deque<Log::message> Log::m_queue = std::deque<Log::message>();

void Log::push(const std::string &str, eMessageTypes type)
{
	m_queue.push_back({str, type});
}

void Log::run()
{
	auto windowSize = Game::getWindow().getSize();

	unsigned rectHeight = cCharSize * (cLinesDisplayed+1);
	//magic numbers
	sf::RectangleShape logBkg(sf::Vector2f(windowSize.x - 6, rectHeight - 3));
	logBkg.setPosition(3, windowSize.y - rectHeight);

	if(m_queue.size() > cMaxHistory) {
		unsigned difference = m_queue.size() - cMaxHistory;
		for(unsigned i = 0; i < difference; ++i) {
			m_queue.pop_front();
		}

	}
	
	logBkg.setFillColor(sf::Color(12, 12, 23));
	logBkg.setOutlineThickness(3);
	logBkg.setOutlineColor(sf::Color(22, 22, 33));

	Game::getWindow().draw(logBkg);
	
	//only display last cLinesDisplayed elements of m_queue
	int size = min(static_cast<unsigned>(m_queue.size()), cLinesDisplayed);
	for(int i = 0; i < size; ++i)
	{
		int index = m_queue.size() > cLinesDisplayed ? (i + (m_queue.size() - cLinesDisplayed)) : i;

		sf::Text text = createLogText(m_queue[index].text, m_queue[index].type);

		//i needs to be from 0 - cLinesDisplayed to format properly
		text.setPosition(cBorder, windowSize.y - ((cCharSize * (cLinesDisplayed + 1)) - i * cCharSize));
		Game::getWindow().draw(text);		
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
		//lots of magic numbers
		constexpr int cSpace = 50;
		sf::RectangleShape historyBkg(sf::Vector2f(windowSize.x - cSpace, windowSize.y - cSpace));
		historyBkg.setPosition(cSpace/2, cSpace/2);
		historyBkg.setFillColor(sf::Color(12,12,23));
		historyBkg.setOutlineThickness(3);
		historyBkg.setOutlineColor(sf::Color(22, 22, 33));
		
		Game::getWindow().draw(historyBkg);

		for(int i = 0; i < m_queue.size(); ++i) {
			sf::Text text = createLogText(m_queue[i].text, m_queue[i].type);
			text.setPosition(cSpace/2, cSpace/2 + (i * cCharSize));
			Game::getWindow().draw(text);
		}
	}
}

sf::Text Log::createLogText(const std::string& str, eMessageTypes type)
{
	sf::Text text;
	text.setString(str);
		
	text.setCharacterSize(cCharSize);
	text.setFont(Game::getDefaultFont());		
	if(type == eMessageTypes::Announcement) {
		text.setFillColor(sf::Color::Yellow);
	}		
	if(type == eMessageTypes::Standard) {
		text.setFillColor(sf::Color::White);
	}
	if(type == eMessageTypes::Alert) {
		text.setFillColor(sf::Color::Cyan);
	}
	if(type == eMessageTypes::Damage) {
		text.setFillColor(sf::Color::Red);
		text.setStyle(sf::Text::Bold);
	}
	return text;
}
