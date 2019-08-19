#pragma once

#include <SFML/Graphics.hpp>

class UIObject {
public:
    virtual void run(bool hasKeyEvents, sf::Event event) = 0;
};
