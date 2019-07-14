#include "level.h"

Level::Level(int width, int height) : m_width(width), m_height(height), m_data(width*height)
{
		
}

void Level::run()
{
	
}

bool Level::isFreeSpace(int x, int y) const
{
	if(x < 0 || y < 0 || x >= m_width || y >= m_height) {
		return false;
	}
	return true;
}
