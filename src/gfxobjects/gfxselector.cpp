#include "gfxselector.h"
#include "game.h"
#include "object/selectorobject.h"
#include "utils.h"

GFXSelector::GFXSelector()
    : m_shape(sf::Vector2f(cWidth, cHeight))
{
    m_shape.setFillColor(sf::Color(255, 20, 20, 50));
    m_shape.setRotation(45.f);
}

void GFXSelector::run(const SelectorObject* selector, vector2d center, float zoom)
{
    sf::View original = Game::getWindow().getView();
    sf::View v = Game::getWindow().getView();
    v.setSize(v.getSize().x, v.getSize().y * 2);
    // v.setCenter(v.getSize() *.5f);
    sf::Vector2f centerScreenSpace = coordsToScreen(sf::Vector2f(center.x, center.y));
    v.setCenter(centerScreenSpace.x, centerScreenSpace.y + 200);
    v.zoom(zoom);
    Game::getWindow().setView(v);
    vector2d position = selector->getPosition();
    m_shape.setPosition(coordsToScreen(sf::Vector2f(position.x, position.y)));

    Game::getWindow().draw(m_shape);

    Game::getWindow().setView(original);
}
