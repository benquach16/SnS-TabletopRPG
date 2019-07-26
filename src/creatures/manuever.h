#pragma once
#include "../items/types.h"

enum class eManueverTypes {
	Offense,
	Defense,
	Position
};

struct Manuever
{
	virtual eManueverTypes getType() = 0;
	int dice = 0;
};

struct Offense : public Manuever
{
	eManueverTypes getType() override { return eManueverTypes::Offense; }
	eOffensiveManuevers manuever;
	bool linked = false;
	bool feint = false;
	eHitLocations target;
	eBodyParts pinpoint;
	Component* component;
};

struct Defense : public Manuever
{
	eManueverTypes getType() override { return eManueverTypes::Defense; }
	eDefensiveManuevers manuever;
};

struct Position : public Manuever
{
	eManueverTypes getType() override { return eManueverTypes::Position; }
};
