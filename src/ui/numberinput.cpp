#include "numberinput.h"
#include "../game.h"
#include "types.h"

using namespace std;

NumberInput::NumberInput()
    : m_number(0)
    , m_max(-1)
{
    m_text.setCharacterSize(cCharSize);
    m_text.setFont(Game::getDefaultFont());
}

void NumberInput::run(bool hasKeyEvents, sf::Event event)
{
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c >= '0' && c <= '9') {
            int val = c - '0';

            m_number *= 10;
            m_number += val;

            if (m_max != -1 && m_number > m_max) {
                m_number = m_max;
            }
        }
        if (c == '\b') {
            m_number /= 10;
        }
        m_number = max(0, m_number);
    }

    m_text.setString(std::to_string(m_number));
    Game::getWindow().draw(m_text);
}
