#include "gfxobject.h"

GFXObject::GFXObject(sf::Drawable* draw, vector2d position, bool persistent, int zmod)
    : m_draw(draw)
    , m_persistent(persistent)
{
    m_Z = position.x + position.y + zmod;
}

void GFXObject::cleanup()
{
    if (m_draw != nullptr) {
        delete m_draw;
    }
    m_draw = nullptr;
}
