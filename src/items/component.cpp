#include "component.h"

Component::Component(std::string name, int damage, eDamageTypes type, eAttacks attack, int tn,
    std::set<eWeaponProperties> properties)
    : m_name(name)
    , m_damage(damage)
    , m_damageType(type)
    , m_attack(attack)
    , m_tn(tn)
    , m_properties(properties)
{
}
