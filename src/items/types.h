#pragma once

enum class eWeaponTypes : unsigned {
    Brawling,
    Swords,
    Longswords,
    Mass,
    Polearms,
    Bows,
    Crossbows,
    Firearms,
    Count
};

enum class eDamageTypes { Blunt, Piercing, Cutting };

enum class eWeaponProperties {
    Crushing,
    Hook,
    MaillePiercing,
    PlatePiercing,
    Draw,
    Defensive,
    Curved,
    Heavy
};

enum class eAttacks {
    Thrust,
    Swing,
};

enum class eOffensiveManuevers : unsigned {
    Swing,
    Thrust,
    PinpointThrust,
    HeavyBlow,
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
    Bite,
    Strangle,
    Gouge,
    NoOffense
};

enum class ePositionManuevers : unsigned { Invalid = 0, Stand, Pickup };

enum class ePreResolveManuevers : unsigned { Invalid = 0, Stomp, Feint };

enum class eGrips : unsigned { Standard, Staff, HalfSword, Overhand };

enum class eHookTargets : unsigned { Joint, Primary, Secondary };

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
    Hand = 1,
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

enum class eItemType {
    Item,
    Armor,
    Weapon,
    Food,
    Waterskin,
    Bedroll,
    Money,
    Bandage,
    Firstaid,
    Misc // Only used for filtering
};
