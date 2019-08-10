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

bool Component::isLinked(eGrips grip) const
{
    auto it = m_grips.find(grip);
    // if we couldn't find it, either its because of bad code or its a pommel, so return value for
    // pommel
    if (it == m_grips.end()) {
        return true;
    }
    return it->second;
}

std::set<eGrips> Component::getGrips() const
{
    std::set<eGrips> ret;
    for (auto it : m_grips) {
        ret.insert(it.first);
    }
    return ret;
}
