#include <algorithm>
#include <iostream>
#include "log.h"
#include "game.h"

using namespace std;

std::deque<Log::message> Log::m_queue = std::deque<Log::message>();

constexpr unsigned cCharSize = 14;
constexpr unsigned cLinesDisplayed = 12;
constexpr unsigned cMaxHistory = 50;

void Log::push(const std::string &str, eMessageTypes type)
{
	m_queue.push_back({str, type});
}

void Log::run()
{
	sf::RectangleShape rectangle(sf::Vector2f(800, 200));

	if(m_queue.size() > cMaxHistory) {
		unsigned difference = m_queue.size() - cLinesDisplayed;
		for(unsigned i = 0; i < difference; ++i) {
			m_queue.pop_front();
		}

	}

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
		text.setPosition(0, Game::getWindow().getSize().y - ((cCharSize * (cLinesDisplayed + 1)) - i * cCharSize));
		Game::getWindow().draw(text);		
	}
}
