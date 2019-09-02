#pragma once

#include <string>

#include "uiobject.h"

class TextInput : public UIObject {
public:
    TextInput();

    void reset() { m_string = ""; }
    void run(bool hasKeyEvents, sf::Event event);

private:
    std::string m_string;
    sf::Text m_text;
};
