#include "textinput.h"
#include "../game.h"
#include "types.h"

TextInput::TextInput()
{
    m_text.setCharacterSize(cCharSize);
    m_text.setFont(Game::getDefaultFont());
}

void TextInput::run(bool hasKeyEvents, sf::Event event)
{
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        m_string += c;
        if (c == '\b' && m_string.size() > 0) {
            m_string.pop_back();
        }
    }

    m_text.setString(m_string);
    Game::getWindow().draw(m_text);
}
