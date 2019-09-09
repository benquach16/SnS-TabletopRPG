#pragma once

#include <string>

#include "uiobject.h"

class TextInput : public UIObject {
public:
    TextInput();

    void reset() { m_string = ""; }
    void run(bool hasKeyEvents, sf::Event event) override;
    std::string getString() const { return m_string; }

    void setPosition(sf::Vector2f position) override { m_text.setPosition(position); }

private:
    std::string m_string;
    sf::Text m_text;
};
