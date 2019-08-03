// An object that can be placed on a level

#pragma once

#include <map>
#include <string>
#include <vector>

struct vector2d {
    int x = 0;
    int y = 0;
    vector2d(int x, int y)
        : x(x)
        , y(y)
    {
    }
};

enum class eObjectTypes {
    Misc,
    Item,
    Creature,
    Corpse,
    Chest
};

class Level;

class Object {
public:
    Object();
    virtual ~Object();
    vector2d getPosition() const { return m_position; }
    virtual bool hasCollision() const { return false; }
    virtual std::string getDescription() const = 0;
    virtual eObjectTypes getObjectType() const = 0;
    virtual bool deleteMe() const { return false; }
    virtual bool preserveBetweenLevels() const { return false; }
    virtual void run(const Level*);
    void setPosition(int x, int y)
    {
        m_position.x = x;
        m_position.y = y;
    }

    void setPosition(vector2d position) { m_position = position; }

    void moveDown() { m_position.y++; }
    void moveUp() { m_position.y--; }
    void moveLeft() { m_position.x--; }
    void moveRight() { m_position.x++; }

    const std::map<int, int>& getInventory() const { return m_inventory; }
    std::map<int, int>& getInventoryMutable() { return m_inventory; }

    unsigned getId() const { return m_id; }

protected:
    vector2d m_position;

    std::map<int, int> m_inventory;
    //should be assigned a unique id on creation
    unsigned m_id;
};
