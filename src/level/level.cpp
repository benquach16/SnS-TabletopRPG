#include "level.h"
#include "../object/corpseobject.h"
#include "../object/creatureobject.h"
#include "../3rdparty/random.hpp"

using namespace effolkronium;
using namespace std;

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
	std::vector<Room> m_rooms;
	for(int i = 0; i <5; ++i) {
		//makeRoom();
	}

	for(int x = 1; x < m_width; ++x) {
		for(int y = 1; y < m_height; ++y) {
			(*this)(x,y).m_type = eTileType::Wall;
		}		
	}

	m_rooms.push_back(carveRoom(1, 1, 6, 6, 10, 10));
	
	for(int i = 0; i < 10; ++i) {
		m_rooms.push_back(carveRoom());
		int idx = m_rooms.size() - 1;
		createCorridor(m_rooms[idx -1], m_rooms[idx]);
	}

}

Room Level::carveRoom()
{
	int xlen = random_static::get(4, 10);
	int ylen = random_static::get(4, 10);
	int xStart = random_static::get(2, 29);
	int yStart = random_static::get(2, 29);

	for(int x = xStart; x < xStart + xlen; ++x) {
		for(int y = yStart; y < yStart + ylen; ++y) {
			(*this)(x, y).m_type = eTileType::Ground;
		}
	}

	return { random_static::get(xStart + 1, xlen + xStart - 1),
			 random_static::get(yStart + 1, ylen + yStart - 1)
	};
}

Room Level::carveRoom(int xStart, int yStart, int minSizeX, int minSizeY, int maxSizeX, int maxSizeY)
{
	int xlen = random_static::get(minSizeX, maxSizeX);
	int ylen = random_static::get(minSizeY, maxSizeY);

	for(int x = xStart; x < xStart + xlen; ++x) {
		for(int y = yStart; y < yStart + ylen; ++y) {
			(*this)(x, y).m_type = eTileType::Ground;
		}
	}

	return { random_static::get(xStart + 1, xlen + xStart - 1),
			 random_static::get(yStart + 1, ylen + yStart - 1)
	};
}

void Level::createCorridor(Room room1, Room room2)
{
	for(int x = min(room1.x, room2.x); x < max(room1.x, room2.x); ++x) {
		(*this)(x, room2.y).m_type = eTileType::Ground;		
	}
	for(int y = min(room1.y, room2.y); y < max(room1.y, room2.y); ++y) {
		(*this)(room2.x, y).m_type = eTileType::Ground;		
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
		for(int x = xStart - 1; x < xlen+xStart + 1; x++) {
			if((*this)(x, yStart).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}
		for(int y = yStart - 1;y < ylen+yStart + 1; y++) {
			if((*this)(xStart, y).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}
		for(int x = xStart - 1; x < xlen+xStart + 1; x++) {
			if((*this)(x, yStart + ylen).m_type == eTileType::Wall) {
				hasWall = true;
				break;
			}
		}
		for(int y = yStart - 1; y < ylen+yStart+1; y++) {
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
	int xDoor = effolkronium::random_static::get(xStart + 1, xlen + xStart - 1);
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
