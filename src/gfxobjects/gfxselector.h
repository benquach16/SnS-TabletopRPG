#pragma once

#include <SFML/Graphics.hpp>

class SelectorObject;

class GFXSelector {
public:
    GFXSelector();
    void run(const SelectorObject* selector);

private:
    sf::RectangleShape m_shape;
};
