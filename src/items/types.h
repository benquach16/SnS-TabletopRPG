#pragma once

enum class eWeaponTypes : unsigned {
    Brawling,
    Swords,
    Longswords,
    Mass,
    Polearms,
    Bows,
    Crossbows,
    Firearms
};

enum class eDamageTypes { Blunt, Piercing, Cutting };

enum class eWeaponProperties { Crushing, Hook, MaillePiercing, PlatePiercing, Draw };

enum class eAttacks {
    Thrust,
    Swing,
};

enum class eOffensiveManuevers : unsigned {
    Swing,
    Thrust,
    PinpointThrust,
    Beat,
    Mordhau,
    Hook,
    Slam,
    Grab,
    Disarm,
    // grapple moves
    Snap,
    Throw,
    Pin
};

enum class ePositionManuevers : unsigned { Stand, Pickup };

enum class ePreResolveManuevers : unsigned { Stomp, Feint };

enum class eGrips : unsigned { Standard, Staff, HalfSword, Overhand };

inline int gripReachDifference(eGrips grip)
{
    switch (grip) {
    case eGrips::Staff:
        return 2;
    case eGrips::HalfSword:
        return 1;
    case eGrips::Overhand:
        return 1;
    default:
        return 0;
    }
}

inline int offenseManueverCost(eOffensiveManuevers manuever)
{
    switch (manuever) {
    case eOffensiveManuevers::PinpointThrust:
    case eOffensiveManuevers::Pin:
        return 3;
    case eOffensiveManuevers::Beat:
    case eOffensiveManuevers::Mordhau:
    case eOffensiveManuevers::Snap:
    case eOffensiveManuevers::Throw:
    case eOffensiveManuevers::Disarm:
    case eOffensiveManuevers::Grab:
    case eOffensiveManuevers::Hook:
        return 1;
    default:
        return 0;
    }
}

enum class eDefensiveManuevers : unsigned {
    // special manuevers used to tell combatmanager about special events
    NoDefense,
    StealInitiative,
    AttackFromDef,
    ParryLinked,
    DodgeLinked,
    Counter,
    Parry,
    Dodge,
    Expulsion,
    // grapple manuevers
    Break,
    Resist,
    Reverse
};

inline int defenseManueverCost(eDefensiveManuevers manuever)
{
    switch (manuever) {
    case eDefensiveManuevers::ParryLinked:
        return 1;
    case eDefensiveManuevers::Counter:
        return 2;
    default:
        return 0;
    }
}

inline int feintManueverCost() { return 1; }

inline int positionManueverCost() { return 3; }

enum class eLength : unsigned {
    Nonexistent = 0,
    Half = 1,
    Hand = 2,
    Close = 3,
    Short = 4,
    Medium = 5,
    Long = 6,
    VeryLong = 7,
    Extended = 8,
    Extreme = 9,
    Ludicrous = 10
};

enum class eInitiativeRoll : unsigned {
    Attack,
    Defend,
};

enum class eMaterials : unsigned { Iron };

enum class eLayer : unsigned { Cloth = 0, Maille = 1, Plate = 2, Surcoat = 3 };

enum class eArmorTypes : unsigned { None = 0, Maille = 1, Plate = 2 };

enum class eItemType { Item, Armor, Weapon, Food, Waterskin, Bedroll, Money, Bandage, Firstaid };
