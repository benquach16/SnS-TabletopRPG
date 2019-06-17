#pragma once

#include <string>
#include <map>
#include "types.h"

class WoundTable
{
public:
	WoundTable();

	static constexpr unsigned cPartsPerLocation = 6;

	struct woundParts {
		eBodyParts m_swing[cPartsPerLocation];
		eBodyParts m_thrust[cPartsPerLocation];
	};

private:
	std::map<eHitLocations, woundParts> m_hitTable;
};

class Wound
{
private:
	eHitLocations m_location;
	std::string m_flavorText;
	int m_level;
};
