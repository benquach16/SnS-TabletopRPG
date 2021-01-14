#pragma once

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
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

class Scene;
class Level;
class Object;

class Trigger {
public:
    friend class boost::serialization::access;

    static const int cPersistentTrigger = -1;

    enum class eTriggerConditions : unsigned {
        FactionCleared,
        ItemInInventory,
        CreatureCleared,
        ChangeLevel
    };

    Trigger(int timesToFire);
    // triggers might need a ton of state
    virtual bool run(Scene* scene, Level* level, Object* triggeringObject) = 0;

    virtual eTriggerConditions getTriggerCondition() const = 0;
    void decrement();

private:
    // for boost
    Trigger();

protected:
    int m_timesToFire;

    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& m_timesToFire;
    }
};
