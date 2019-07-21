
#include "level.h"

Level::Level(int width, int height) : m_width(width), m_height(height), m_data(width*height)
{
		
}

void Level::run()
{
	for(int i = 0; i < m_objects.size(); ++i) {
		if(m_objects[i]->deleteMe() == true) {
			m_objects.erase(m_objects.begin() + i);
		}
		m_objects[i]->run(this);
	}
}

void Level::cleanup()
{
	for(int i = 0; i < m_toDelete.size(); ++i) {
		delete m_toDelete[i];
	}
	m_toDelete.clear();
}

const Object* Level::getObject(vector2d position)
{
	for (int i = 0; i < m_objects.size(); ++i) {
		vector2d objectPosition = m_objects[i]->getPosition();
		if(position.x == objectPosition.x && position.y == objectPosition.y) {
			return m_objects[i];
		}
	}
	return nullptr;
}

bool Level::isFreeSpace(int x, int y) const
{
	if(x < 0 || y < 0 || x >= m_width || y >= m_height) {
		return false;
	}
	return true;
}
