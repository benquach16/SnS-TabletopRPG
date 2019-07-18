#include "utils.h"

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
	case eDefensiveManuevers::Dodge:
		return "Dodge";
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

std::string lengthToString(eLength length)
{
	switch(length) {
	case eLength::Hand:
		return "Hand";
		break;
	case eLength::Short:
		return "Short";
		break;
	case eLength::Medium:
		return "Medium";
		break;
	case eLength::Long:
		return "Long";
		break;
	case eLength::VeryLong:
		return "Very Long";
		break;
	case eLength::Extended:
		return "Extended";
		break;
	case eLength::Ludicrous:
		return "Ludicrous";
		break;
	}
	return "";
}

eLayer stringToArmorLayer(const std::string& str)
{
	if(str == "surcoat") {
		return eLayer::Surcoat;
	}
	return eLayer::Cloth;
}
