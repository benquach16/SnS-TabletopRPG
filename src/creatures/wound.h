#pragma once
#include <string>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "../items/types.h"
#include "../3rdparty/json.hpp"
#include "types.h"


class Wound
{
public:
	Wound(eBodyParts location, std::vector<std::string> text, int level, int btn, int impact, std::set<eEffects> effects);

	std::string getText() { return m_text[0]; }
	int getBTN() { return m_btn; }
	int getImpact() { return m_impact; }
	bool causesDeath();
private:
	eBodyParts m_location;
	std::vector<std::string> m_text;
	int m_level;
	int m_impact;
	int m_btn;
	std::set<eEffects> m_effects;
};

class WoundTable
{
public:
	~WoundTable();
	eBodyParts getSwing(eHitLocations location);
	eBodyParts getThrust(eHitLocations location);

	Wound* getWound(eDamageTypes type, eBodyParts part, int level);
	
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
	void initWoundTable(eDamageTypes type, nlohmann::json woundJson);
	eBodyParts stringToBodyPart(const std::string& str);
	eEffects stringToEffect(const std::string& str);
	
	static WoundTable *singleton;
	
	std::map<eHitLocations, woundParts> m_hitTable;

	//4d associative array for wound table
	//damage type to body part to wound level
	std::map<eDamageTypes, std::map<eBodyParts, std::map<int, Wound*> > > m_woundTable;

	std::map<eDamageTypes, std::vector<int> > m_btnTable;
	std::map<eDamageTypes, std::vector<int> > m_impactTable;
};

