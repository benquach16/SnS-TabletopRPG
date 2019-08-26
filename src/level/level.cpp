#include "level.h"
#include "../3rdparty/random.hpp"
#include "../object/corpseobject.h"
#include "../object/creatureobject.h"
#include "../object/humanobject.h"

using namespace effolkronium;
using namespace std;

Level::Level(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_data(width * height)
{
}

Level::~Level() { clearObjects(); }

void Level::load() {}

void Level::run()
{
    for (unsigned i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i]->deleteMe() == true) {
            Object* object = m_objects[i];
            if (object->getObjectType() == eObjectTypes::Creature) {
                CreatureObject* creatureObject = static_cast<CreatureObject*>(object);
                CorpseObject* corpse
                    = new CorpseObject(creatureObject->getCreatureComponent()->getName());
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
    std::vector<Room> rooms;

    for (int x = 1; x < m_width; ++x) {
        for (int y = 1; y < m_height; ++y) {
            (*this)(x, y).m_type = eTileType::Wall;
        }
    }

    rooms.push_back(carveRoom(1, 1, 6, 6, 10, 10));

    for (int i = 0; i < 9; ++i) {
        rooms.push_back(carveRoom());
        int idx = rooms.size() - 1;
        createCorridor(rooms[idx - 1], rooms[idx]);
    }
    for (int i = 0; i < 5; ++i) {
        rooms.push_back(carveSeperateRoom());
        int idx = rooms.size() - 1;
        createCorridor(rooms[idx - 1], rooms[idx]);
    }

    // makeRoom();
    removeIslands();

    // temporary, to add enemies
    for (unsigned i = 1; i < rooms.size(); ++i) {
        if (random_static::get(1, 3) == 1) {
            HumanObject* object = new HumanObject;
            object->setPosition(rooms[i].x, rooms[i].y);
            object->setFaction(eCreatureFaction::Bandit);
            object->setLoadout(eCreatureFaction::Bandit, eRank::Recruit);
            m_objects.push_back(object);
        }
    }

    // add enemy to farthest room as a boss
    int minDist = 0;
    int idx = 0;
    for (unsigned i = 1; i < rooms.size(); ++i) {
        int a = rooms[i].x - rooms[0].x;
        int b = rooms[i].y - rooms[0].y;

        int dist = (a * a) + (b * b);
        if (dist > minDist) {
            minDist = dist;
            idx = i;
        }
    }
    HumanObject* object = new HumanObject;
    object->setPosition(rooms[idx].x, rooms[idx].y);
    object->setFaction(eCreatureFaction::Bandit);
    object->setLoadout(eCreatureFaction::Bandit, eRank::Veteran);
    object->getCreatureComponent()->setAgility(6);
    object->getCreatureComponent()->setCunning(6);
    object->setName("One-Eyed Doyt");
    m_objects.push_back(object);
}

Room Level::carveRoom()
{
    constexpr int max = 10;
    int xlen = random_static::get(4, max);
    int ylen = random_static::get(4, max);
    int xStart = random_static::get(2, m_width - max - 1);
    int yStart = random_static::get(2, m_height - max - 1);

    for (int x = xStart; x < xStart + xlen; ++x) {
        for (int y = yStart; y < yStart + ylen; ++y) {
            (*this)(x, y).m_type = eTileType::Ground;
        }
    }

    return { random_static::get(xStart + 1, xlen + xStart - 1),
        random_static::get(yStart + 1, ylen + yStart - 1) };
}

Room Level::carveRoom(
    int xStart, int yStart, int minSizeX, int minSizeY, int maxSizeX, int maxSizeY)
{
    int xlen = random_static::get(minSizeX, maxSizeX);
    int ylen = random_static::get(minSizeY, maxSizeY);

    for (int x = xStart; x < xStart + xlen; ++x) {
        for (int y = yStart; y < yStart + ylen; ++y) {
            (*this)(x, y).m_type = eTileType::Ground;
        }
    }

    return { random_static::get(xStart + 1, xlen + xStart - 1),
        random_static::get(yStart + 1, ylen + yStart - 1) };
}

Room Level::carveSeperateRoom()
{
    constexpr int cMax = 10;
    int size = cMax;
    int xlen, ylen, xStart, yStart;

    bool canPlace = false;
    unsigned iteration = 0;
    while (canPlace == false) {
        if (iteration > 10000) {
            // lower our standards
            size = max(size - 1, 2);
        }
        bool hasGround = false;
        xlen = random_static::get(2, size);
        ylen = random_static::get(2, size);
        xStart = random_static::get(3, m_width - size - 4);
        yStart = random_static::get(3, m_height - size - 4);

        for (int x = xStart - 2; x < xStart + xlen + 2; ++x) {
            for (int y = yStart - 2; y < yStart + ylen + 2; ++y) {
                if ((*this)(x, y).m_type == eTileType::Ground) {
                    hasGround = true;
                }
            }
        }
        canPlace = !hasGround;
        iteration++;
        cout << "Iteration: " << iteration << endl;
    }

    for (int x = xStart; x < xStart + xlen; ++x) {
        for (int y = yStart; y < yStart + ylen; ++y) {
            (*this)(x, y).m_type = eTileType::Ground;
        }
    }
    return { random_static::get(xStart + 1, xlen + xStart - 1),
        random_static::get(yStart + 1, ylen + yStart - 1) };
}

void Level::createCorridor(Room room1, Room room2)
{
    int x = 0;
    if (room1.x < room2.x) {
        for (x = room1.x; x < room2.x; ++x) {
            (*this)(x, room1.y).m_type = eTileType::Ground;
        }
    } else {
        for (x = room1.x; x >= room2.x; --x) {
            (*this)(x, room1.y).m_type = eTileType::Ground;
        }
    }

    if (room1.y < room2.y) {
        for (int y = room1.y; y < room2.y; ++y) {
            (*this)(x, y).m_type = eTileType::Ground;
        }
    } else {
        for (int y = room1.y; y >= room2.y; --y) {
            (*this)(x, y).m_type = eTileType::Ground;
        }
    }
}

void Level::makeRoom()
{
    bool canPlace = false;
    int xlen;
    int ylen;
    int xStart;
    int yStart;
    while (canPlace == false) {
        xlen = random_static::get(4, 10);
        ylen = random_static::get(4, 10);
        xStart = random_static::get(2, 20);
        yStart = random_static::get(2, 20);

        // check for existing room
        bool hasWall = false;
        for (int x = xStart - 2; x < xlen + xStart + 2; x++) {
            if ((*this)(x, yStart).m_type == eTileType::Wall) {
                hasWall = true;
                break;
            }
        }
        for (int y = yStart - 2; y < ylen + yStart + 2; y++) {
            if ((*this)(xStart, y).m_type == eTileType::Wall) {
                hasWall = true;
                break;
            }
        }
        for (int x = xStart - 2; x < xlen + xStart + 2; x++) {
            if ((*this)(x, yStart + ylen).m_type == eTileType::Wall) {
                hasWall = true;
                break;
            }
        }
        for (int y = yStart - 2; y < ylen + yStart + 2; y++) {
            if ((*this)(xStart + xlen, y).m_type == eTileType::Wall) {
                hasWall = true;
                break;
            }
        }

        canPlace = !hasWall;
    }

    for (int x = xStart; x < xlen + xStart; x++) {
        (*this)(x, yStart).m_type = eTileType::Wall;
    }
    for (int y = yStart; y < ylen + yStart; y++) {
        (*this)(xStart, y).m_type = eTileType::Wall;
    }

    for (int x = xStart; x < xlen + xStart; x++) {
        (*this)(x, yStart + ylen).m_type = eTileType::Wall;
    }

    for (int y = yStart; y < ylen + yStart + 1; y++) {
        (*this)(xStart + xlen, y).m_type = eTileType::Wall;
    }

    int dir = effolkronium::random_static::get(1, 4);
    int xDoor = effolkronium::random_static::get(xStart + 1, xlen + xStart - 1);
    int yDoor = effolkronium::random_static::get(yStart + 2, ylen + yStart - 2);
    switch (dir) {
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

void Level::removeIslands()
{
    for (int x = 1; x < m_width - 2; ++x) {
        for (int y = 1; y < m_height - 2; ++y) {
            if ((*this)(x, y).m_type == eTileType::Wall) {
                bool single = true;
                if ((*this)(x - 1, y).m_type == eTileType::Wall) {
                    single = false;
                }
                if ((*this)(x + 1, y).m_type == eTileType::Wall) {
                    single = false;
                }
                if ((*this)(x, y - 1).m_type == eTileType::Wall) {
                    single = false;
                }
                if ((*this)(x, y + 1).m_type == eTileType::Wall) {
                    single = false;
                }

                if (single == true) {
                    (*this)(x, y).m_type = eTileType::Ground;
                }
            }
        }
    }
}

void Level::cleanup()
{
    for (unsigned i = 0; i < m_toDelete.size(); ++i) {
        cout << "clearing.." << endl;
        delete m_toDelete[i];
    }
    m_toDelete.clear();
}

void Level::clearObjects()
{
    for (unsigned i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i]->preserveBetweenLevels() == false) {
            delete m_objects[i];
        }
    };
    m_objects.clear();
}

const Object* Level::getObject(vector2d position)
{
    for (unsigned i = 0; i < m_objects.size(); ++i) {
        vector2d objectPosition = m_objects[i]->getPosition();
        if (position.x == objectPosition.x && position.y == objectPosition.y) {
            return m_objects[i];
        }
    }
    return nullptr;
}

Object* Level::getObjectMutable(vector2d position, const Object* exclude)
{
    for (unsigned i = 0; i < m_objects.size(); ++i) {
        vector2d objectPosition = m_objects[i]->getPosition();
        if (position.x == objectPosition.x && position.y == objectPosition.y
            && exclude != m_objects[i]) {
            return m_objects[i];
        }
    }
    return nullptr;
}

bool Level::isFreeSpace(int x, int y) const
{
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return false;
    }
    if ((*this)(x, y).m_type == eTileType::Wall) {
        return false;
    }
    for (unsigned i = 0; i < m_objects.size(); ++i) {
        vector2d position = m_objects[i]->getPosition();
        if (position.x == x && position.y == y && m_objects[i]->hasCollision() == true) {
            return false;
        }
    }
    return true;
}

std::vector<Object*> Level::getObjectsAtLocation(vector2d position)
{
    std::vector<Object*> ret;
    for (unsigned i = 0; i < m_objects.size(); ++i) {
        vector2d objectPosition = m_objects[i]->getPosition();
        if (position.x == objectPosition.x && position.y == objectPosition.y) {
            ret.push_back(m_objects[i]);
        }
    }

    return ret;
}
