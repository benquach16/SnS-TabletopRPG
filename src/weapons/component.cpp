#include "component.h"

Component::Component(std::string name, int damage, eDamageTypes type, std::set<eWeaponProperties> properties) :
	m_name(name), m_damage(damage), m_damageType(type), m_properties(properties)
{
	
}
