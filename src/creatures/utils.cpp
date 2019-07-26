#include "utils.h"

std::string bodyPartToString(eBodyParts part) {
	switch(part) {
	case eBodyParts::Crown:
		return "Crown";
	case eBodyParts::Face:
		return "Face";
	case eBodyParts::Neck:
		return "Neck";
	case eBodyParts::Shoulder:
		return "Shoulder";
	case eBodyParts::UpperArm:
		return "Upper Arm";
	case eBodyParts::Elbow:
		return "Elbow";
	case eBodyParts::Forearm:
		return "Forearm";
	case eBodyParts::Hand:
		return "Hand";
	case eBodyParts::Armpit:
		return "Armpit";
	case eBodyParts::Ribs:
		return "Ribs";
	case eBodyParts::Abs:
		return "Abs";
	case eBodyParts::Hip:
		return "Hip";
	case eBodyParts::Groin:
		return "Groin";
	case eBodyParts::Thigh:
		return "Thigh";
	case eBodyParts::Knee:
		return "Knee";
	case eBodyParts::Shin:
		return "Shin";
	case eBodyParts::Foot:
		return "Foot";
	case eBodyParts::Back:
		return "Back";
	case eBodyParts::Hamstring:
		return "Hamstring";
	case eBodyParts::Wing:
		return "Wing";
	case eBodyParts::Tail:
		return "Tail";

	}
	return "";
}

eBodyParts stringToBodyPart(const std::string& str)
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

std::string hitLocationToString(eHitLocations location)
{
	switch(location) {
	case eHitLocations::Head:
		return "Head";
	case eHitLocations::Chest:
		return "Chest";
	case eHitLocations::Arm:
		return "Arm";
	case eHitLocations::Belly:
		return "Belly";
	case eHitLocations::Thigh:
		return "Thigh";
	case eHitLocations::Shin:
		return "Shin";
	case eHitLocations::Wing:
		return "Wing";
	}
	//std::cout << "bad thing : " << static_cast<int>(location) << std::endl;
	return "";
}

std::string stanceToString(eCreatureStance stance)
{
	switch(stance) {
	case eCreatureStance::Standing:
		return "Standing";
	case eCreatureStance::Prone:
		return "Prone";
	}
	return "";
}
