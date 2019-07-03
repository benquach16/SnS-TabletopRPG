#pragma once

enum class eWeaponTypes : unsigned {
	Brawling,
	Swords,
	Longswords,
	Mass,
	Polearms,
	Bows,
	Crossbows
};

enum class eDamageTypes {
	Blunt,
	Piercing,
	Cutting
};

enum class eWeaponProperties {
	Crushing,
	Hook,
	MaillePiercing,
	PlatePiercing
};

enum class eAttacks {
	Thrust,
	Swing,
};


enum class eOffensiveManuevers : unsigned {
	Swing,
	Thrust
};

enum class eDefensiveManuevers : unsigned {
	Parry,
	Dodge	
};

enum class eLength : unsigned
{
	Hand = 0,
	Short = 1,
	Medium = 2,
	Long = 3,
	Extended = 4,
	Extreme = 5,
	Ludicrous = 6
};

enum class eInitiativeRoll : unsigned
{
	Attack,
	Defend,
};

enum class eMaterials : unsigned
{
	Iron		
};
