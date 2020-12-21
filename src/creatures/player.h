#pragma once

#include "human.h"

#include <iostream>

class Player : public Human {
public:
    friend class boost::serialization::access;

    Player();
    ~Player() {}
    bool isPlayer() override { return true; }

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<Human>(*this);
    }
};
