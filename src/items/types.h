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

enum class eWeaponProperties { Crushing, Hook, MaillePiercing, PlatePiercing, Draw, Defensive };

enum class eAttacks {
    Thrust,
    Swing,
};

enum class eOffensiveManuevers : unsigned {
    Invalid = 0,
    Swing,
    Thrust,
    PinpointThrust,
    Beat,
    Mordhau,
    Hook,
    Slam,
    Grab,
    Disarm,
	Draw,
    // grapple moves
    Snap,
    Throw,
    Pin,
    VisorThrust,
    Bite
};

enum class ePositionManuevers : unsigned { Invalid = 0, Stand, Pickup };

enum class ePreResolveManuevers : unsigned { Invalid = 0, Stomp, Feint };

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
enum class eDefensiveManuevers : unsigned {
    // special manuevers used to tell combatmanager about special events
    NoDefense,
    StealInitiative,
    AttackFromDef,
    ParryLinked,
    Overrun,
    Counter,
    Parry,
    Dodge,
	DodgeTakeInitiative,
    Expulsion,
    // grapple manuevers
    Break,
    Resist,
    Reverse
};

enum class eLength : unsigned {
    Nonexistent = 0,
    Half = 1,
    Hand = 2,
    Short = 3,
    Medium = 4,
    Long = 5,
    VeryLong = 6,
    Extended = 7,
    Extreme = 8,
    Ludicrous = 9
};

enum class eInitiativeRoll : unsigned {
    Attack,
    Defend,
};

enum class eMaterials : unsigned { Iron };

enum class eLayer : unsigned { Cloth = 0, Maille = 1, Plate = 2, Surcoat = 3 };

enum class eArmorTypes : unsigned { None = 0, Maille = 1, Plate = 2 };

enum class eItemType { Item, Armor, Weapon, Food, Waterskin, Bedroll, Money, Bandage, Firstaid };
