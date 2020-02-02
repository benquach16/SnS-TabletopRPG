#pragma once

enum class eCreatureFaction : unsigned {
    None = 0,
    Player = 1,
    Bandit = 2,
    Wildlife = 3,
    Confederacy = 4,
    StoneEmpire = 5,
    UhrEmpire = 6,
    Goblin = 7,
    Count = 8
};

enum class eCreatureRace { Human, Goblin };

enum class eRank : unsigned { Recruit, Soldier, Veteran, Lord };
