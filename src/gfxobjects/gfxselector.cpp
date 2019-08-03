#include "gfxselector.h"
#include "../game.h"
#include "../object/selectorobject.h"
#include "utils.h"

GFXSelector::GFXSelector()
    : m_shape(sf::Vector2f(cWidth, cHeight))
{
    m_shape.setFillColor(sf::Color(255, 20, 20, 50));
    m_shape.setRotation(45.f);
}

void GFXSelector::run(const SelectorObject* selector)
{
    vector2d position = selector->getPosition();
    m_shape.setPosition(coordsToScreen(sf::Vector2f(position.x, position.y)));

    Game::getWindow().draw(m_shape);
}
