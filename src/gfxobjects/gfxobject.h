#pragma once

#include "../object/object.h"
#include <SFML/Graphics.hpp>

class GFXObject {
public:
    GFXObject(sf::Drawable* draw, vector2d position, bool persistent = false, int zmod = 0);

    void cleanup();
    void setZ(int z) { m_Z = z; }
    int getZ() const { return m_Z; }
	bool getPersistent() const { return m_persistent; }
    sf::Drawable* getDraw() { return m_draw; }

protected:
	bool m_persistent;
    sf::Drawable* m_draw;
    int m_Z;
};
