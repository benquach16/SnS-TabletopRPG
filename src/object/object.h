// An object that can be placed on a level

#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/strong_typedef.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>

struct vector2d {
    int x = 0;
    int y = 0;
    vector2d(int x, int y)
        : x(x)
        , y(y)
    {
    }

    vector2d()
        : x(0)
        , y(0)
    {
    }

    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& x;
        ar& y;
    }
};

enum class eObjectTypes { Misc, Item, Creature, Corpse, Chest, Usable, Campfire };

class Level;
// might be too late to switch this to an entity component system
class Object {
public:
    friend class boost::serialization::access;
    // replace with boost::uuid
    BOOST_STRONG_TYPEDEF(unsigned, ObjectId);

    Object();

    virtual ~Object() {};
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

    virtual void moveDown() { m_position.y++; }
    virtual void moveUp() { m_position.y--; }
    virtual void moveLeft() { m_position.x--; }
    virtual void moveRight() { m_position.x++; }

    const std::map<int, int>& getInventory() const { return m_inventory; }
    std::map<int, int>& getInventoryMutable() { return m_inventory; }

    ObjectId getId() const { return m_id; }

protected:
    vector2d m_position;

    std::map<int, int> m_inventory;
    // should be assigned a unique id on creation
    ObjectId m_id;

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_position;
        ar& m_inventory;
    }
};
