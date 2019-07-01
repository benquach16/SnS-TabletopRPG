#include <algorithm>
#include <iostream>
#include "log.h"
#include "game.h"

using namespace std;

std::deque<Log::message> Log::m_queue = std::deque<Log::message>();

constexpr unsigned cCharSize = 14;
constexpr unsigned cLinesDisplayed = 12;
constexpr unsigned cMaxHistory = 50;
constexpr unsigned cBorder = 5;

void Log::push(const std::string &str, eMessageTypes type)
{
	m_queue.push_back({str, type});
}

void Log::run()
{
	auto windowSize = Game::getWindow().getSize();

	unsigned rectHeight = cCharSize * (cLinesDisplayed+1);
	sf::RectangleShape rectangle(sf::Vector2f(windowSize.x - 6, rectHeight - 3));
	rectangle.setPosition(3, windowSize.y - rectHeight);
	
	if(m_queue.size() > cMaxHistory) {
		unsigned difference = m_queue.size() - cLinesDisplayed;
		for(unsigned i = 0; i < difference; ++i) {
			m_queue.pop_front();
		}

	}
	rectangle.setFillColor(sf::Color(12, 12, 23));
	rectangle.setOutlineThickness(3);
	rectangle.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(rectangle);
	
	
	//only display last cLinesDisplayed elements of m_queue
	int size = min(static_cast<unsigned>(m_queue.size()), cLinesDisplayed);
	for(int i = 0; i < size; ++i)
	{
		int index = m_queue.size() > cLinesDisplayed ? (i + (m_queue.size() - cLinesDisplayed)) : i;

		sf::Text text;
		text.setString(m_queue[index].text);
		if(m_queue[index].type == eMessageTypes::Announcement) {
			text.setFillColor(sf::Color::Yellow);
		}		
		if(m_queue[index].type == eMessageTypes::Standard) {
			text.setFillColor(sf::Color::White);
		}
		if(m_queue[index].type == eMessageTypes::Damage) {
			text.setFillColor(sf::Color::Red);
			text.setStyle(sf::Text::Bold);
		}
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		//i needs to be from 0 - cLinesDisplayed to format properly
		text.setPosition(cBorder, windowSize.y - ((cCharSize * (cLinesDisplayed + 1)) - i * cCharSize));
		Game::getWindow().draw(text);		
	}
}
