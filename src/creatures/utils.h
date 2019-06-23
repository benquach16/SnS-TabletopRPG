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
	}
	return "";
}
