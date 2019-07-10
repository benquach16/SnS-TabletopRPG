#include "wound.h"

#include "../dice.h"

#include <fstream>
#include <iostream>
#include <assert.h>

using namespace std;

const string filepath = "data/wounds.json";

WoundTable *WoundTable::singleton = nullptr;

constexpr int woundLevels = 5;

Wound::Wound(eBodyParts location, std::vector<std::string> text, int level, int btn, int impact, std::set<eEffects> effects) :
	m_location(location), m_text(text), m_level(level), m_btn(btn), m_impact(impact), m_effects(effects)
{
}

bool Wound::causesDeath()
{
	auto it = m_effects.find(eEffects::Death);
	return (it != m_effects.end());
}

WoundTable::WoundTable()
{
	ifstream file(filepath);

	nlohmann::json parsedWounds;
	
	file >> parsedWounds;

	auto bluntJson = parsedWounds["blunt"];
	auto piercingJson = parsedWounds["piercing"];
	auto cuttingJson = parsedWounds["cutting"];

	vector<int> bluntBtn = bluntJson["BTN"];
	vector<int> bluntImpact = bluntJson["impact"];
	m_btnTable[eDamageTypes::Blunt] = bluntBtn;
	m_impactTable[eDamageTypes::Blunt] = bluntImpact;
	vector<int> piercingBtn = piercingJson["BTN"];
	vector<int> piercingImpact = bluntJson["impact"];
	m_btnTable[eDamageTypes::Piercing] = piercingBtn;
	m_impactTable[eDamageTypes::Piercing] = piercingImpact;
	vector<int> cuttingBtn = cuttingJson["BTN"];
	vector<int> cuttingImpact = bluntJson["impact"];
	m_btnTable[eDamageTypes::Cutting] = cuttingBtn;
	m_impactTable[eDamageTypes::Cutting] = cuttingImpact;
	initWoundTable(eDamageTypes::Blunt, bluntJson);
	initWoundTable(eDamageTypes::Piercing, piercingJson);
	initWoundTable(eDamageTypes::Cutting, cuttingJson);
	initHitLocationTable();
}

WoundTable::~WoundTable()
{
	for(auto it : m_woundTable) {
		
	}
}

void WoundTable::initWoundTable(eDamageTypes type, nlohmann::json woundJson)
{
	for(auto &iter : woundJson.items()) {
		string key = iter.key();
		
		//ignore BTN and impact tables
		if(key == "BTN" || key == "impact") continue;
		eBodyParts bodyPart = stringToBodyPart(key);
		auto values = iter.value();
		for(int i = 1; i <= woundLevels; ++i) {
			std::string index = std::to_string(i);
			auto woundJson = values[index];

			set<eEffects> effects;
			if(woundJson["effects"].is_null() == false) {
				for(int j = 0; j < woundJson["effects"].size(); ++j) {	
					eEffects effect = stringToEffect(woundJson["effects"][j]);
					effects.insert(effect);
				}
			}
			Wound* wound = new Wound(bodyPart, woundJson["text"], i, m_btnTable[type][i-1],
									 m_impactTable[type][i-1], effects);

			m_woundTable[type][bodyPart][i] = wound;
		}
	}
}

void WoundTable::initHitLocationTable()
{
	//Head
	m_hitTable[eHitLocations::Head].m_swing[0] = eBodyParts::Crown;
	m_hitTable[eHitLocations::Head].m_swing[1] = eBodyParts::Crown;
	m_hitTable[eHitLocations::Head].m_swing[2] = eBodyParts::Crown;
	m_hitTable[eHitLocations::Head].m_swing[3] = eBodyParts::Face;
	m_hitTable[eHitLocations::Head].m_swing[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Head].m_swing[5] = eBodyParts::Shoulder;

	m_hitTable[eHitLocations::Head].m_thrust[0] = eBodyParts::Crown;
	m_hitTable[eHitLocations::Head].m_thrust[1] = eBodyParts::Crown;
	m_hitTable[eHitLocations::Head].m_thrust[2] = eBodyParts::Face;
	m_hitTable[eHitLocations::Head].m_thrust[3] = eBodyParts::Face;
	m_hitTable[eHitLocations::Head].m_thrust[4] = eBodyParts::Face;
	m_hitTable[eHitLocations::Head].m_thrust[5] = eBodyParts::Neck;

	//chest
	m_hitTable[eHitLocations::Chest].m_swing[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Chest].m_swing[1] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Chest].m_swing[2] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Chest].m_swing[3] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Chest].m_swing[4] = eBodyParts::SecondLocationArm;
	m_hitTable[eHitLocations::Chest].m_swing[5] = eBodyParts::SecondLocationHead;

	m_hitTable[eHitLocations::Chest].m_thrust[0] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Chest].m_thrust[1] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Chest].m_thrust[2] = eBodyParts::Armpit;
	m_hitTable[eHitLocations::Chest].m_thrust[3] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Chest].m_thrust[4] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Chest].m_thrust[5] = eBodyParts::Abs;

	//arms
	m_hitTable[eHitLocations::Arm].m_swing[0] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Arm].m_swing[1] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Arm].m_swing[2] = eBodyParts::UpperArm;
	m_hitTable[eHitLocations::Arm].m_swing[3] = eBodyParts::Elbow;
	m_hitTable[eHitLocations::Arm].m_swing[4] = eBodyParts::Forearm;
	m_hitTable[eHitLocations::Arm].m_swing[5] = eBodyParts::Hand;

	m_hitTable[eHitLocations::Arm].m_thrust[0] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Arm].m_thrust[1] = eBodyParts::Armpit;
	m_hitTable[eHitLocations::Arm].m_thrust[2] = eBodyParts::UpperArm;
	m_hitTable[eHitLocations::Arm].m_thrust[3] = eBodyParts::Elbow;
	m_hitTable[eHitLocations::Arm].m_thrust[4] = eBodyParts::Forearm;
	m_hitTable[eHitLocations::Arm].m_thrust[5] = eBodyParts::Hand;

	//belly
	m_hitTable[eHitLocations::Belly].m_swing[0] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Belly].m_swing[1] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Belly].m_swing[2] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Belly].m_swing[3] = eBodyParts::Hip;
	m_hitTable[eHitLocations::Belly].m_swing[4] = eBodyParts::Groin;
	m_hitTable[eHitLocations::Belly].m_swing[5] = eBodyParts::Thigh;

	m_hitTable[eHitLocations::Belly].m_thrust[0] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Belly].m_thrust[1] = eBodyParts::Ribs;
	m_hitTable[eHitLocations::Belly].m_thrust[2] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Belly].m_thrust[3] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Belly].m_thrust[4] = eBodyParts::Hip;
	m_hitTable[eHitLocations::Belly].m_thrust[5] = eBodyParts::Groin;

	//thigh
	m_hitTable[eHitLocations::Thigh].m_swing[0] = eBodyParts::Hip;
	m_hitTable[eHitLocations::Thigh].m_swing[1] = eBodyParts::Hip;
	m_hitTable[eHitLocations::Thigh].m_swing[2] = eBodyParts::Thigh;
	m_hitTable[eHitLocations::Thigh].m_swing[3] = eBodyParts::Thigh;
	m_hitTable[eHitLocations::Thigh].m_swing[4] = eBodyParts::Thigh;
	m_hitTable[eHitLocations::Thigh].m_swing[5] = eBodyParts::Knee;

	m_hitTable[eHitLocations::Thigh].m_thrust[0] = eBodyParts::Abs;
	m_hitTable[eHitLocations::Thigh].m_thrust[1] = eBodyParts::Hip;
	m_hitTable[eHitLocations::Thigh].m_thrust[2] = eBodyParts::Groin;
	m_hitTable[eHitLocations::Thigh].m_thrust[3] = eBodyParts::Thigh;
	m_hitTable[eHitLocations::Thigh].m_thrust[4] = eBodyParts::Knee;
	m_hitTable[eHitLocations::Thigh].m_thrust[5] = eBodyParts::Shin;

	//shin
	m_hitTable[eHitLocations::Shin].m_swing[0] = eBodyParts::Thigh;
	m_hitTable[eHitLocations::Shin].m_swing[1] = eBodyParts::Knee;
	m_hitTable[eHitLocations::Shin].m_swing[2] = eBodyParts::Shin;
	m_hitTable[eHitLocations::Shin].m_swing[3] = eBodyParts::Shin;
	m_hitTable[eHitLocations::Shin].m_swing[4] = eBodyParts::Shin;
	m_hitTable[eHitLocations::Shin].m_swing[5] = eBodyParts::Foot;

	m_hitTable[eHitLocations::Shin].m_thrust[0] = eBodyParts::Thigh;
	m_hitTable[eHitLocations::Shin].m_thrust[1] = eBodyParts::Knee;
	m_hitTable[eHitLocations::Shin].m_thrust[2] = eBodyParts::Shin;
	m_hitTable[eHitLocations::Shin].m_thrust[3] = eBodyParts::Shin;
	m_hitTable[eHitLocations::Shin].m_thrust[4] = eBodyParts::Shin;
	m_hitTable[eHitLocations::Shin].m_thrust[5] = eBodyParts::Foot;	
}

eBodyParts WoundTable::stringToBodyPart(const std::string& str)
{
	if(str == "crown") {
		return eBodyParts::Crown;
	} else if(str == "face") {
		return eBodyParts::Face;
	} else if(str == "neck") {
		return eBodyParts::Neck;
	} else if(str == "shoulder") {
		return eBodyParts::Shoulder;
	} else if(str == "upperarm") {
		return eBodyParts::UpperArm;
	} else if(str == "elbow") {
		return eBodyParts::Elbow;
	} else if(str == "forearm") {
		return eBodyParts::Forearm;
	} else if(str == "hand") {
		return eBodyParts::Hand;
	} else if(str == "armpit") {
		return eBodyParts::Armpit;
	} else if(str == "ribs") {
		return eBodyParts::Ribs;
	} else if(str == "abs") {
		return eBodyParts::Abs;
	} else if(str == "hip") {
		return eBodyParts::Hip;
	} else if(str == "groin") {
		return eBodyParts::Groin;
	} else if(str == "thigh") {
		return eBodyParts::Thigh;
	} else if(str == "knee") {
		return eBodyParts::Knee;
	} else if(str == "shin") {
		return eBodyParts::Shin;
	} else if(str == "foot") {
		return eBodyParts::Foot;
	}
	return eBodyParts::Tail;
}

eEffects WoundTable::stringToEffect(const std::string& str)
{
	if(str == "KO1") {
		return eEffects::KO1;
	} else if(str == "KO2") {
		return eEffects::KO2;
	} else if(str == "KO3") {
		return eEffects::KO3;
	} else if(str == "BL1") {
		return eEffects::KO2;
	} else if(str == "BL2") {
		return eEffects::KO2;
	} else if(str == "BL3") {
		return eEffects::KO2;
	} else if(str == "drop1") {
		return eEffects::KO2;
	} else if(str == "drop2") {
		return eEffects::KO2;
	} else if(str == "drop3") {
		return eEffects::KO2;
	}
	return eEffects::Death;
}

eBodyParts WoundTable::getSwing(eHitLocations location)
{
	//dice returns an actual dice roll 1-6, so we have to offset by 1
	int roll = DiceRoller::roll() - 1;
	eBodyParts part= m_hitTable[location].m_swing[roll];
	if(part == eBodyParts::SecondLocationHead) {
		roll = DiceRoller::roll() - 1;
		return m_hitTable[eHitLocations::Head].m_swing[roll];
	}
	if(part == eBodyParts::SecondLocationArm) {
		roll = DiceRoller::roll() - 1;
		return m_hitTable[eHitLocations::Arm].m_swing[roll];		
	}
	return part;
}

eBodyParts WoundTable::getThrust(eHitLocations location)
{
	//dice returns an actual dice roll 1-6, so we have to offset by 1
	int roll = DiceRoller::roll() - 1;
	return m_hitTable[location].m_thrust[roll];
}

Wound* WoundTable::getWound(eDamageTypes type, eBodyParts part, int level)
{
	level = min(level, 5);
	Wound* ret = m_woundTable[type][part][level];
	assert(ret != nullptr);
	return ret;
}

