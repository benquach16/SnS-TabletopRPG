#include "wound.h"
#include "../3rdparty/json.hpp"
#include "../dice.h"

#include <fstream>
#include <assert.h>

using namespace std;

const string filepath = "data/wounds.json";

WoundTable *WoundTable::singleton = nullptr;

WoundTable::WoundTable()
{
	ifstream file(filepath);

	nlohmann::json parsedWounds;
	
	file >> parsedWounds;

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
	return m_hitTable[location].m_swing[roll];
}

