#include "numberinput.h"
#include "types.h"
#include "../game.h"

NumberInput::NumberInput() : m_number(0), m_max(-1)
{
	m_text.setCharacterSize(cCharSize);
	m_text.setFont(Game::getDefaultFont());
}

void NumberInput::run(sf::Event event)
{
	if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		if(c >= '0' && c <= '9') {
			int val = c - '0';

			m_number *= 10;
			m_number += val;

			if(m_max != -1 && m_number > m_max) {
				m_number = m_max;
			}
		}
		if(c == '\b') {
			m_number /= 10;
		}
	}

	m_text.setString(std::to_string(m_number));
	Game::getWindow().draw(m_text);
}

void NumberInput::setPosition(sf::Vector2f position)
{
	m_text.setPosition(position);
}
