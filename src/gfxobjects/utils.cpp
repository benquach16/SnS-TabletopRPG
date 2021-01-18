#include "utils.h"

constexpr float transform = 0.7071f;
sf::Vector2f coordsToScreen(sf::Vector2f coords)
{
    sf::Vector2f newCoords;
    newCoords.x = (coords.x - coords.y) * cWidth * transform;
    newCoords.y = (coords.x + coords.y) * cHeight * transform;

    return newCoords;
}

sf::Vector2f screenToCoords(sf::Vector2f coords)
{
    sf::Vector2f newCoords;
    /*
    newCoords.x = coords.x / cWidth / transform;
    newCoords.y = coords.y / cHeight / transform;
    float total = newCoords.x + newCoords.y;
    float x = total / 2.f;
    float y = newCoords.y - x;
    newCoords.x = x;
    newCoords.y = y;
    */

    float X = coords.y / (cHeight * transform) + coords.x / (2.f * (cWidth * transform));
    float y = coords.y / (cHeight * transform) - coords.x / (2.f * (cWidth * transform));
    newCoords.x = X;
    newCoords.y = y;
    return newCoords;
}
