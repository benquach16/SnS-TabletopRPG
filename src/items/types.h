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

enum class eWeaponProperties { Crushing, Hook, MaillePiercing, PlatePiercing };

enum class eAttacks {
    Thrust,
    Swing,
};

enum class eOffensiveManuevers : unsigned { Swing, Thrust, PinpointThrust, Beat, Mordhau, Hook };

enum class ePositionManuevers : unsigned { Stand, Pickup };

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
        return 2;
    case eOffensiveManuevers::Hook:
    case eOffensiveManuevers::Beat:
    case eOffensiveManuevers::Mordhau:
        return 1;
    default:
        return 0;
    }
}

enum class eDefensiveManuevers : unsigned {
    // special manuevers used to tell combatmanager about special events
    NoDefense,
    StealInitiative,
    ParryLinked,
    DodgeLinked,
    Counter,
    Parry,
    Dodge,
    Expulsion
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

enum class eLength : unsigned {
    Hand = 0,
    Close = 1,
    Short = 2,
    Medium = 3,
    Long = 4,
    VeryLong = 5,
    Extended = 6,
    Extreme = 7,
    Ludicrous = 8
};

enum class eInitiativeRoll : unsigned {
    Attack,
    Defend,
};

enum class eMaterials : unsigned { Iron };

enum class eLayer : unsigned { Cloth = 0, Maille = 1, Plate = 2, Surcoat = 3 };

enum class eArmorTypes : unsigned { None = 0, Maille = 1, Plate = 2 };

enum class eItemType { Item, Armor, Weapon, Food, Waterskin, Bedroll, Money, Bandage };
