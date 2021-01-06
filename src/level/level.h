#pragma once

#include <assert.h>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

#include "../object/object.h"
#include "trigger.h"

enum eTileMaterial { Stone, Wood, Dirt, Grass };
enum eTileType { Ground, Wall };

struct Tile {
    eTileType m_type = eTileType::Ground;
    eTileMaterial m_material = eTileMaterial::Stone;
    int m_levelChangeIdx = -1;
	std::vector<Trigger*> m_triggers;

    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_type;
        ar& m_material;
        ar& m_levelChangeIdx;
    }
};

enum eLighting { Sunny, Dark, Cave };

class Scene;
class Level {
public:
    friend class boost::serialization::access;

    // level wide triggers
    enum eLevelLogic {
        None,
        Arena,
    };

    Level(int width, int height);
    ~Level();
    void save();
    void load();
    void run(Scene* scene);
    void generate();
    void generateTown();
    void makeRoom();
    void cleanup();
    void clearObjects();

    Tile& operator()(int x, int y)
    {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < m_width);
        assert(y < m_height);
        return m_data[x * m_height + y];
    }

    Tile operator()(int x, int y) const
    {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < m_width);
        assert(y < m_height);
        return m_data[x * m_height + y];
    }

    Tile get(vector2d pos) const { return (*this)(pos.x, pos.y); }

    const Object* getObject(vector2d position);
    Object* getObjectMutable(vector2d position, const Object* exclude);
	void assignLogic(eLevelLogic logic) { m_logic = logic; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isFreeSpace(int x, int y) const;
    void addObject(Object* object) { m_objects.push_back(object); }
    // does not clear object - whoever is calling this responsible for ensuring it gets deleted
    Object* removeObject(Object::ObjectId id);
    const std::vector<Object*>& getObjects() const { return m_objects; }
    std::vector<Object*> getObjectsAtLocation(vector2d position);

    eLighting getLighting() const { return m_lighting; }

private:
    struct Room {
        int x;
        int y;
    };

    struct Building {
        int x;
        int y;
        int width;
        int height;
        int xDoor;
        int yDoor;
    };

    // for boost only
    Level() {}
    Building createBuilding();
    Room carveRoom();
    Room carveRoom(int xStart, int yStart, int minSizeX, int minSizeY, int maxSizeX, int maxSizeY);
    Room carveSeperateRoom();
    void removeIslands();
    void createCorridor(Room room1, Room room2);
    void generateEnemy();
    
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_data;
        ar& m_objects;
        ar& m_toDelete;
        ar& m_width;
        ar& m_height;
        ar& m_lighting;
        ar& m_logic;
    }

    std::vector<Trigger*> m_globalTriggers;
    std::vector<Object*> m_objects;
    std::vector<Object*> m_toDelete;
    int m_width;
    int m_height;
    eLighting m_lighting;
    eLevelLogic m_logic;
    std::vector<Tile> m_data;
};
