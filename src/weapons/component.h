#pragma once

#include "types.h"
#include <string>
#include <set>

//components of weapons
//axe heads, spear heads, buttspikes...
class Component
{
public:
	Component(std::string name, int damage, eDamageTypes type, std::set<eWeaponProperties> properties);
	std::string getName() { return m_name; }
	int getDamage() { return m_damage; }
	eDamageTypes getType() { return m_damageType; }
	
private:
	std::string m_name;
	int m_damage;
	eDamageTypes m_damageType;
	std::set<eWeaponProperties> m_properties;
};
