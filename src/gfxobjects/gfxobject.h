#pragma once

#include <SFML/Graphics.hpp>

class GFXObject
{
public:
	int getZ() const { return m_Z; } 
protected:
	int m_Z;
};
