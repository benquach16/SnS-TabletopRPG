#pragma once

#include "types.h"
#include <set>
#include <string>

// components of weapons
// axe heads, spear heads, buttspikes...
class Component {
public:
    struct grip {
        eGrips grip;
        bool linked = false;
    };

    Component(std::string name, int damage, eDamageTypes type, eAttacks attack,
        std::set<eWeaponProperties> properties);
    std::string getName() const { return m_name; }
    int getDamage() const { return m_damage; }
    eDamageTypes getType() const { return m_damageType; }
    eAttacks getAttack() const { return m_attack; }
    bool hasProperty(eWeaponProperties prop) const
    {
        return m_properties.find(prop) != m_properties.end();
    }
    const std::set<eWeaponProperties>& getProperties() const { return m_properties; }

private:
    std::string m_name;
    int m_damage;
    eDamageTypes m_damageType;
    eAttacks m_attack;
    std::set<eWeaponProperties> m_properties;
    std::set<grip> m_grips;
};
