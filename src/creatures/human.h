#pragma once

#include "creature.h"

class Human : public Creature {
public:
    friend class boost::serialization::access;
    Human();
    ~Human() {}
    eCreatureType getCreatureType() override { return eCreatureType::Human; }
    void randomizeStats();

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<Creature>(*this);
    }
};
