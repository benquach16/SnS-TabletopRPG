#pragma once

#include <string>

#include "uiobject.h"

class NumberInput : public UIObject {
public:
    NumberInput();

    void run(bool hasKeyEvents, sf::Event event) override;
    bool pollForCompletion();
    int getNumber() const { return m_number; }
    void reset()
    {
        m_number = 0;
        m_max = -1;
    }
    void setMax(int max) { m_max = max; }
    void setPosition(sf::Vector2f position) override { m_text.setPosition(position); }

private:
    int m_number;
    int m_max;

    sf::Text m_text;
};
