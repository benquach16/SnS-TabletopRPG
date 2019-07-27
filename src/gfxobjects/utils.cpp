#include "utils.h"

sf::Vector2f coordsToScreen(sf::Vector2f coords){
	sf::Vector2f newCoords;
	newCoords.x = (coords.x - coords.y) * mult;
	newCoords.y = (coords.x + coords.y) * mult;
    return newCoords;
}
