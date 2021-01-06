#pragma once

enum class eCreatureFaction : unsigned {
    None = 0,
    Player = 1,
    Bandit = 2,
    Wildlife = 3,
    Confederacy = 4,
    StoneEmpire = 5,
    JewelCities = 6,
    Goblin = 7,
    ArenaFighter = 8,
    Count = 9
};

enum class eSkills : unsigned {
    Athletics,
    Stealth,
    Riding,
    SleightOfHand,
    Disguise,
    FirstAid,
};

enum class eCreatureRace { Human, Goblin };

enum class eRank : unsigned { Recruit, Soldier, Veteran, Lord, Count };
