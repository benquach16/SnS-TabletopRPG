#pragma once

#include <vector>
#include <assert.h>

#include "../object/object.h"

enum eTileMaterial
{
	Stone,
	Wood,
};
enum eTileType
{
	Ground,
	Wall
};

struct Tile
{
	eTileType m_type = eTileType::Ground;
	eTileMaterial m_material = eTileMaterial::Stone;
};

class Level
{
public:
	Level(int width, int height);

	virtual void run();
	void cleanup();

	Tile& operator()(int x, int y) {
		assert(x >= 0);
		assert(y >= 0);
		assert(x < m_width);
		assert(y < m_height);
		return m_data[x * m_height + y];
	}

	Tile operator()(int x, int y) const {
		assert(x >= 0);
		assert(y >= 0);
		assert(x < m_width);
		assert(y < m_height);
		return m_data[x * m_height + y];
	}

	const Object *getObject(vector2d position);
	Object* getObjectMutable(vector2d position, const Object* exclude);

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	bool isFreeSpace(int x, int y) const;
	void addObject(Object* object) { m_objects.push_back(object); }
	const std::vector<Object*>& getObjects() const { return m_objects; }
	std::vector<Object*> getObjectsAtLocation(vector2d position);
	
private:
	std::vector<Object*> m_objects;
	std::vector<Object*> m_toDelete;
	int m_width;
	int m_height;
	std::vector<Tile> m_data;	
};
