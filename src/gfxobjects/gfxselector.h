#pragma once

#include <SFML/Graphics.hpp>

#include "object/object.h"

class SelectorObject;

class GFXSelector {
public:
    GFXSelector();
    void run(const SelectorObject* selector, vector2d center, float zoom);

private:
    sf::RectangleShape m_shape;
};
