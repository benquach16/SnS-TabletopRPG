#pragma once

#include "../combatinstance.h"
#include "../combatmanager.h"
#include "../creatures/player.h"
#include "creatureobject.h"

constexpr unsigned cBleedTicks = 10;

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
    void move();

    // never delete player, otherwise there will be a segfault
    bool deleteMe() const override { return false; }
    bool preserveBetweenLevels() const override { return true; }

    void moveDown() override;
    void moveUp() override;
    void moveLeft() override;
    void moveRight() override;

private:
	unsigned m_bleedTick;
    vector2d m_toMove;
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<CreatureObject>(*this);
    }
};
