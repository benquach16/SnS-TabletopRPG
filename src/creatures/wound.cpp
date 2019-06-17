#include "wound.h"
#include "../3rdparty/json.hpp"

#include <fstream>

using namespace std;

const string filepath = "data/wounds.json";

WoundTable::WoundTable()
{
	ifstream file(filepath);

	nlohmann::json parsedWounds;
	
	file >> parsedWounds;

	//Head
	//swing
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

	m_hitTable[eHitLocations::Arm].m_thrust[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Arm].m_thrust[1] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Arm].m_thrust[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Arm].m_thrust[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Arm].m_thrust[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Arm].m_thrust[5] = eBodyParts::Neck;

	//belly
	m_hitTable[eHitLocations::Belly].m_swing[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_swing[1] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Belly].m_swing[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_swing[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_swing[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_swing[5] = eBodyParts::Neck;

	m_hitTable[eHitLocations::Belly].m_thrust[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_thrust[1] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_thrust[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_thrust[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_thrust[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Belly].m_thrust[5] = eBodyParts::Neck;

	//thigh
	m_hitTable[eHitLocations::Thigh].m_swing[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_swing[1] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Thigh].m_swing[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_swing[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_swing[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_swing[5] = eBodyParts::Neck;

	m_hitTable[eHitLocations::Thigh].m_thrust[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_thrust[1] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_thrust[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_thrust[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_thrust[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Thigh].m_thrust[5] = eBodyParts::Neck;

	//shin
	m_hitTable[eHitLocations::Shin].m_swing[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_swing[1] = eBodyParts::Shoulder;
	m_hitTable[eHitLocations::Shin].m_swing[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_swing[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_swing[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_swing[5] = eBodyParts::Neck;

	m_hitTable[eHitLocations::Shin].m_thrust[0] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_thrust[1] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_thrust[2] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_thrust[3] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_thrust[4] = eBodyParts::Neck;
	m_hitTable[eHitLocations::Shin].m_thrust[5] = eBodyParts::Neck;
}
