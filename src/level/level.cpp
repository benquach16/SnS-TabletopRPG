#include "level.h"
#include "../object/corpseobject.h"
#include "../object/creatureobject.h"
#include "../3rdparty/random.hpp"

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

void Level::generate()
{
	for(int i = 0; i <5; ++i) {
		makeRoom();
	}
}

void Level::makeRoom()
{
	bool canPlace = false;
	int xlen;
	int ylen;
	int xStart;
	int yStart;
	while(canPlace == false) {
		xlen = effolkronium::random_static::get(4, 10);
		ylen = effolkronium::random_static::get(4, 10);
		xStart = effolkronium::random_static::get(2, 29);
		yStart = effolkronium::random_static::get(2, 29);

		//check for existing room
		bool hasWall = false;
		for(int x = xStart; x < xlen+xStart; x++) {
			if((*this)(x, yStart).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}
		for(int y = yStart;y < ylen+yStart; y++) {
			if((*this)(xStart, y).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}
		for(int x = xStart; x < xlen+xStart; x++) {
			if((*this)(x, yStart + ylen).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}
		for(int y = yStart; y < ylen+yStart+1; y++) {
			if((*this)(xStart + xlen, y).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}

		canPlace = !hasWall;
	}
	
	for(int x = xStart; x < xlen+xStart; x++) {
		(*this)(x, yStart).m_type = eTileType::Wall;
	}
	for(int y = yStart; y < ylen+yStart; y++) {
		(*this)(xStart, y).m_type = eTileType::Wall;
	}
	
	for(int x = xStart; x < xlen+xStart; x++) {
		(*this)(x, yStart + ylen).m_type = eTileType::Wall;
	}

	for(int y = yStart; y < ylen+yStart+1; y++) {
		(*this)(xStart + xlen, y).m_type = eTileType::Wall;
	}
	int dir = effolkronium::random_static::get(1, 4);
	int xDoor = effolkronium::random_static::get(xStart, xlen + xStart);
	int yDoor = effolkronium::random_static::get(yStart + 2, ylen + yStart - 2);	
	switch(dir) {
	case 1:
		(*this)(xDoor, yStart).m_type = eTileType::Ground;
		break;
	case 2:
		(*this)(xStart, yDoor).m_type = eTileType::Ground;
		break;
	case 3:
		(*this)(xDoor, yStart + ylen).m_type = eTileType::Ground;
		break;
	case 4:
		(*this)(xStart + xlen, yDoor).m_type = eTileType::Ground;
		break;
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
	if((*this)(x, y).m_type == eTileType::Wall) {
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
