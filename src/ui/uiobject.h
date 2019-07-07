#pragma once

#include <SFML/Graphics.hpp>

class UIObject
{
public:
	virtual void run(sf::Event event) = 0;
};
