#include <algorithm>
#include "log.h"
#include "game.h"

using namespace std;

std::deque<std::string> Log::m_queue = std::deque<std::string>();

constexpr unsigned cCharSize = 16;
constexpr unsigned cLinesDisplayed = 10;
void Log::push(const std::string &str)
{
	m_queue.push_back(str);
}

void Log::run()
{
	sf::RectangleShape rectangle(sf::Vector2f(800, 200));

	if(m_queue.size() > cLinesDisplayed) {
		unsigned difference = m_queue.size() - cLinesDisplayed;
		for(unsigned i = 0; i < difference; ++i) {
			m_queue.pop_front();
		}

	}
	
	for(int i = 0; i < m_queue.size(); ++i)
	{
		sf::Text text;
		text.setString(m_queue[i]);
		text.setCharacterSize(cCharSize);
		text.setFont(Game::getDefaultFont());
		text.setPosition(0, Game::getWindow().getSize().y - ((cCharSize * (cLinesDisplayed + 1)) - i * cCharSize));
		Game::getWindow().draw(text);		
	}
}
