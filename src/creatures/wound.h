#pragma once

#include <string>
#include <map>
#include <memory>

#include "types.h"


class Wound
{
private:
	eBodyParts m_location;
	std::string m_flavorText;
	int m_level;
};

class WoundTable
{
public:

	eBodyParts getSwing(eHitLocations location);
	eBodyParts getThrust(eHitLocations location);
	
	static constexpr unsigned cPartsPerLocation = 6;

	struct woundParts {
		eBodyParts m_swing[cPartsPerLocation];
		eBodyParts m_thrust[cPartsPerLocation];
	};
	
	static WoundTable* getSingleton() {
		if(singleton == nullptr) {
			singleton = new WoundTable;
		}

		return singleton;
	}

private:
	WoundTable();
	static WoundTable *singleton;
	
	std::map<eHitLocations, woundParts> m_hitTable;
};

