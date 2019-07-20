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
	Thrust,
	PinpointThrust,
	FeintThrust,
	FeintSwing
};

static int offenseManueverCost(eOffensiveManuevers manuever) {
	return 0;
}

enum class eDefensiveManuevers : unsigned {
	//special manuevers used to tell combatmanager about special events
	NoDefense,
	StealInitiative,
	ParryLinked,
	DodgeLinked,
	Counter,
	
	Parry,
	Dodge	
};

static int defenseManueverCost(eDefensiveManuevers manuever) {
	switch(manuever) {
	case eDefensiveManuevers::ParryLinked:
		return 1;
	}
	return 0;
}

enum class eLength : unsigned
{
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

enum class eInitiativeRoll : unsigned
{
	Attack,
	Defend,
};

enum class eMaterials : unsigned
{
	Iron		
};

enum class eLayer : unsigned
{
	Cloth = 0,
	Maille = 1,
	Plate = 2,
	Surcoat = 3
};

enum class eArmorTypes : unsigned
{
	None = 0,
	Maille = 1,
	Plate = 2
};
