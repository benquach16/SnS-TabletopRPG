#include "component.h"

Component::Component(std::string name, int damage, eDamageTypes type, eAttacks attack,
    std::set<eWeaponProperties> properties, std::unordered_map<eGrips, bool> grips, bool pommel)
    : m_name(name)
    , m_damage(damage)
    , m_damageType(type)
    , m_attack(attack)
    , m_properties(properties)
    , m_grips(grips)
    , m_pommel(pommel)
{
}

std::set<eGrips> Component::getGrips() const
{
    std::set<eGrips> ret;
    for (auto it : m_grips) {
        ret.insert(it.first);
    }
    return ret;
}
