#pragma once

#include "types.h"
#include <string>

//components of weapons
//axe heads, spear heads, buttspikes...
class Component
{
private:
	std::string m_name;
	int m_damage;
	eDamageTypes m_damageType;
};
