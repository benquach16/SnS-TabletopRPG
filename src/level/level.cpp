#include "level.h"
#include "../object/corpseobject.h"
#include "../object/creatureobject.h"


Level::Level(int width, int height) : m_width(width), m_height(height), m_data(width*height)
{
		
}

void Level::run()
{
	for(int i = 0; i < m_objects.size(); ++i) {
		if(m_objects[i]->deleteMe() == true) {
			Object* object = m_objects[i];
			if(object->getObjectType() == eObjectTypes::Creature) {
				CreatureObject* creatureObject = static_cast<CreatureObject*>(object);
				CorpseObject* corpse = new CorpseObject(creatureObject->getCreatureComponent()->getName());
				corpse->setPosition(creatureObject->getPosition());
				corpse->setInventory(creatureObject->getInventory());
				m_objects.push_back(corpse);
			}
			m_toDelete.push_back(m_objects[i]);
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

Object* Level::getObjectMutable(vector2d position,const Object* exclude) {
	for (int i = 0; i < m_objects.size(); ++i) {
		vector2d objectPosition = m_objects[i]->getPosition();
		if(position.x == objectPosition.x && position.y == objectPosition.y &&
			exclude != m_objects[i]) {
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
	for(int i = 0; i < m_objects.size(); ++i) {
		vector2d position = m_objects[i]->getPosition();
		if(position.x == x && position.y == y && m_objects[i]->hasCollision() == true) {
			return false;
		}
	}
	return true;
}

std::vector<Object*> Level::getObjectsAtLocation(vector2d position)
{
	std::vector<Object*> ret;
	for (int i = 0; i < m_objects.size(); ++i) {
		vector2d objectPosition = m_objects[i]->getPosition();
		if(position.x == objectPosition.x && position.y == objectPosition.y) {
			ret.push_back(m_objects[i]);
		}
	}
	
	return ret;
}
