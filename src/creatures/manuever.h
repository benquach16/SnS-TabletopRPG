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
    eHookTargets hookTarget;
    bool linked = false;
    bool feint = false;
    bool pinpoint = false;
    bool stomp = false;
    int heavyblow = 0;
    int feintdie = 0;
    eHitLocations target;
    eBodyParts pinpointTarget;
    // const Weapon* weapon;
    bool withPrimaryWeapon = true;
    const Component* component = nullptr;

    void reset() override
    {
        Manuever::reset();
        manuever = eOffensiveManuevers::NoOffense;
        hookTarget = eHookTargets::Joint;
        linked = false;
        feint = false;
        pinpoint = false;
        stomp = false;
        heavyblow = 0;
        feintdie = 0;
        // weapon = nullptr;
        withPrimaryWeapon = true;
        component = nullptr;
    }
};

struct Defense : public Manuever {
    eManueverTypes getType() override { return eManueverTypes::Defense; }
    // const Weapon* weapon;
    bool withPrimaryWeapon = true;
    eDefensiveManuevers manuever;

    void reset() override
    {
        Manuever::reset();
        manuever = eDefensiveManuevers::NoDefense;
        withPrimaryWeapon = true;
        // weapon = nullptr;
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
