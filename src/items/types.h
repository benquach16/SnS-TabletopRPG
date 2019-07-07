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

static int offenseManueverCost(eOffensiveManuevers manuever) {
	return 0;
}

enum class eDefensiveManuevers : unsigned {
	//special manuevers used to tell combatmanager about special events
	NoDefense,
	StealInitiative,
	
	Parry,
	Dodge	
};

static int defenseManueverCost(eDefensiveManuevers manuever) {
	return 0;
}

enum class eLength : unsigned
{
	Hand = 0,
	Short = 1,
	Medium = 2,
	Long = 3,
	VeryLong = 4,
	Extended = 5,
	Extreme = 6,
	Ludicrous = 7
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

enum class eArmorTypes : unsigned
{
	Metal,
	Rigid
};
