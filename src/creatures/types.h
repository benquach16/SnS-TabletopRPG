#pragma once

#include <map>

enum class eHitLocations : unsigned
{	
	Head,
	Chest,
	Arm,
	Belly,
	Thigh,
	Shin,
	Wing,
	Tail
};

enum class eBodyParts : unsigned
{
	Crown = 0,
	Face = 1,
	Neck = 2,
	Shoulder = 3,
	UpperArm = 4,
	Elbow = 5,
	Forearm = 6,
	Hand = 7,
	Armpit = 8,
	Ribs = 9,
	Abs = 10,
	Hip = 11,
	Groin = 12,
	Thigh = 13,
	Knee = 14,
	Shin = 15,
	Foot = 16,
	Back = 17,
	Hamstring = 18,
	Wing = 19,
	Tail = 20,
	// roll for another location
	SecondLocationHead,
	SecondLocationArm
};

enum class eEffects
{
	KO1,
	KO2,
	KO3,
	BL1,
	BL2,
	BL3,
	drop1,
	drop2,
	drop3,
	Death,
};
