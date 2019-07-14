#pragma once

#include <vector>
#include <assert.h>

#include "../object/object.h"

enum eTileType
{
	Stone
};

struct Tile
{
	eTileType m_type = eTileType::Stone;
};

class Level
{
public:
	Level(int width, int height);

	virtual void run();

	Tile& operator()(int x, int y) {
		assert(x >= 0);
		assert(y >= 0);
		return m_data[x * m_height + y];
	}

	Tile operator()(int x, int y) const {
		assert(x >= 0);
		assert(y >= 0);
		return m_data[x * m_height + y];
	}	

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	bool isFreeSpace(int x, int y) const;
	void addObject(Object* object) { m_objects.push_back(object); }
	const std::vector<Object*>& getObjects() const { return m_objects; }
private:
	std::vector<Object*> m_objects;
	int m_width;
	int m_height;
	std::vector<Tile> m_data;	
};
