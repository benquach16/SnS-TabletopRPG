#pragma once

#include <string>

#include "types.h"

std::string offensiveManueverToString(eOffensiveManuevers manuever)
{
	switch(manuever) {
	case eOffensiveManuevers::Swing:
		return "Swing";
	case eOffensiveManuevers::Thrust:
		return "Thrust";
	}
	return "";
}

std::string defensiveManueverToString(eDefensiveManuevers manuever)
{
	switch(manuever) {
	case eDefensiveManuevers::Parry:
		return "Parry";
		
	}
	return "";
}

std::string damageTypeToString(eDamageTypes type)
{
	switch(type) {
	case eDamageTypes::Blunt:
		return "Blunt";
	case eDamageTypes::Piercing:
		return "Piercing";
	case eDamageTypes::Cutting:
		return "Cutting";
	}
	return "";
}
