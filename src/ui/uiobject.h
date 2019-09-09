#pragma once

#include <SFML/Graphics.hpp>

class UIObject {
public:
    virtual void run(bool hasKeyEvents, sf::Event event) = 0;
    virtual void setPosition(sf::Vector2f position) = 0;
};
