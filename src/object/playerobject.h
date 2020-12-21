#pragma once

#include "../combatinstance.h"
#include "../combatmanager.h"
#include "../creatures/player.h"
#include "creatureobject.h"

class PlayerObject : public CreatureObject {
public:
    friend class boost::serialization::access;

    PlayerObject();
    ~PlayerObject();
    bool runCombat(float tick);

    eCreatureFaction getFaction() const override { return eCreatureFaction::Player; }
    eCreatureRace getRace() const override { return eCreatureRace::Human; }
    std::string getDescription() const override { return "yourself."; }
    bool isPlayer() const override { return true; }
    void run(const Level* level) override;

    // never delete player, otherwise there will be a segfault
    bool deleteMe() const override { return false; }
    bool preserveBetweenLevels() const override { return true; }

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<CreatureObject>(*this);
    }
};
