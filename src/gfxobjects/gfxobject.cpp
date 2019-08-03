#include "gfxobject.h"

GFXObject::GFXObject(sf::Drawable* draw, vector2d position)
    : m_draw(draw)
{
    m_Z = position.x + position.y;
}

void GFXObject::cleanup()
{
    if (m_draw != nullptr) {
        delete m_draw;
    }
    m_draw = nullptr;
}
