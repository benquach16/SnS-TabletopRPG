#pragma once
#include <string>
#include <map>
#include <memory>
#include <set>

#include "../weapons/types.h"
#include "types.h"


class Wound
{
	Wound();
private:
	eBodyParts m_location;
	std::string m_text;
	int m_level;
	int m_btn;
	std::set<eEffects> m_effects;
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
	void initHitLocationTable();
	eBodyParts stringToBodyPart(const std::string& str);
	
	static WoundTable *singleton;
	
	std::map<eHitLocations, woundParts> m_hitTable;

	//4d associative array for wound table
	//damage type to body part to wound level
	std::map<eDamageTypes, std::map<eBodyParts, std::map<int, Wound*> > > m_woundTable;
};

