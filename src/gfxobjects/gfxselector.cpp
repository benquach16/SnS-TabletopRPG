#include "gfxselector.h"
#include "../game.h"
#include "../object/selectorobject.h"

GFXSelector::GFXSelector() : m_shape(sf::Vector2f(32, 32))
{
    m_shape.setFillColor(sf::Color(255, 20, 20, 50));
}

void GFXSelector::run(const SelectorObject* selector)
{
    vector2d position = selector->getPosition();
    m_shape.setPosition(sf::Vector2f(position.x * 32, position.y * 32));
    Game::getWindow().draw(m_shape);
}
