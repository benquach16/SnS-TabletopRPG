#pragma once

#include <SFML/Graphics.hpp>
#include "../object/object.h"

class GFXObject
{
public:
	GFXObject(sf::Drawable* draw, vector2d position);

	void cleanup();
	void setZ(int z) { m_Z = z; }
	int getZ() const { return m_Z; }
	sf::Drawable* getDraw() { return m_draw; }
protected:
	sf::Drawable *m_draw;
	int m_Z;
};
