#pragma once

#include "../creatures/human.h"
#include "creatureobject.h"

class HumanObject : public CreatureObject {
public:
    friend class boost::serialization::access;

    HumanObject();
    ~HumanObject() override;
    eCreatureRace getRace() const override { return eCreatureRace::Human; }
    std::string getDescription() const override;

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<CreatureObject>(*this);
    }
};
