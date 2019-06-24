#pragma once

#include <iostream>

#include "types.h"

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
