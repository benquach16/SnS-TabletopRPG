#pragma once
#include "../items/types.h"
#include "types.h"

enum class eManueverTypes { Offense, Defense, Position, PreResolve };

class Component;

struct Manuever {
    virtual eManueverTypes getType() = 0;
    int dice = 0;
    virtual void reset() { dice = 0; }
};

struct Offense : public Manuever {
    eManueverTypes getType() override { return eManueverTypes::Offense; }
    eOffensiveManuevers manuever;
    bool linked = false;
    bool feint = false;
    bool pinpoint = false;
    bool stomp = false;
    int heavyblow = 0;
    eHitLocations target;
    eBodyParts pinpointTarget;
    const Weapon* weapon;
    const Component* component;

    void reset() override
    {
        Manuever::reset();
        manuever = eOffensiveManuevers::Invalid;
        bool linked = false;
        bool feint = false;
        bool pinpoint = false;
        bool stomp = false;
        int heavyblow = 0;
        weapon = nullptr;
        component = nullptr;
    }
};

struct Defense : public Manuever {
    eManueverTypes getType() override { return eManueverTypes::Defense; }
    const Weapon* weapon;
    eDefensiveManuevers manuever;

    void reset() override
    {
        Manuever::reset();
        manuever = eDefensiveManuevers::NoDefense;
        weapon = nullptr;
    }
};

struct Position : public Manuever {
    eManueverTypes getType() override { return eManueverTypes::Position; }
    ePositionManuevers manuever;

    void reset() override
    {
        Manuever::reset();
        manuever = ePositionManuevers::Invalid;
    }
};

struct PreResolve : public Manuever {
    eManueverTypes getType() override { return eManueverTypes::PreResolve; }
};
