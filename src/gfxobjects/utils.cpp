#include "utils.h"

constexpr float transform = 0.7071;
sf::Vector2f coordsToScreen(sf::Vector2f coords){
	sf::Vector2f newCoords;
	newCoords.x = (coords.x - coords.y) * cWidth * transform;
	newCoords.y = (coords.x + coords.y) * cHeight * transform;

    return newCoords;
}
