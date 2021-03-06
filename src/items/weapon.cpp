#include <assert.h>
#include <fstream>
#include <iostream>

#include "3rdparty/json.hpp"
#include "creatures/creature.h"
#include "utils.h"
#include "weapon.h"

WeaponTable* WeaponTable::singleton = nullptr;

using namespace std;

const string filepath = "data/weapons.json";

Weapon::Weapon(const std::string& name, const std::string& description, eLength length,
    std::vector<Component*> components, eWeaponTypes type, int cost, int baseTn, int guardTn,
    set<eWeaponProperties> properties, bool secondary, bool shield, bool naturalWeapon,
    int secondaryWeaponId)
    : Item(name, description, cost, eItemType::Weapon)
    , m_length(length)
    , m_components(components)
    , m_type(type)
    , m_properties(properties)
    , m_secondary(secondary)
    , m_tn(baseTn)
    , m_guardTn(guardTn)
    , m_shield(shield)
    , m_naturalWeapon(naturalWeapon)
    , m_secondaryWeaponId(secondaryWeaponId)
{
    for (unsigned i = 0; i < components.size(); ++i) {
        if (components[i]->getAttack() == eAttacks::Thrust) {
            m_thrustComponents.push_back(components[i]);
        }
        if (components[i]->getAttack() == eAttacks::Swing) {
            m_swingComponents.push_back(components[i]);
        }
    }
}

Weapon::~Weapon()
{
    for (unsigned i = 0; i < m_components.size(); ++i) {
        delete m_components[i];
    }
    m_components.clear();
    // don't delete other components, they have shared pointers
}

Component* Weapon::getBestAttack() const
{
    assert(m_components.size() > 0);

    Component* ret = m_components[0];
    for (unsigned i = 1; i < m_components.size(); ++i) {
        if (m_components[i]->getDamage() > ret->getDamage()) {
            ret = m_components[i];
        }
    }

    return ret;
}

Component* Weapon::getBestThrust() const
{
    assert(m_components.size() > 0);
    Component* ret = m_thrustComponents[0];
    for (unsigned i = 1; i < m_thrustComponents.size(); ++i) {
        if (m_thrustComponents[i]->getDamage() > ret->getDamage()) {
            ret = m_thrustComponents[i];
        }
    }
    return ret;
}

Component* Weapon::getBestBlunt() const
{
    assert(m_components.size() > 0);

    Component* ret = nullptr;
    for (unsigned i = 0; i < m_components.size(); ++i) {
        if (m_components[i]->getType() == eDamageTypes::Blunt) {
            if (ret == nullptr || m_components[i]->getDamage() > ret->getDamage()) {
                ret = m_components[i];
            }
        }
    }

    return ret;
}

Component* Weapon::getPommelStrike() const
{
    const Weapon* pommel = getSecondaryWeapon();
    assert(pommel->getSwingComponents().size() > 0);
    return pommel->getSwingComponents()[0];
}

const Weapon* Weapon::getSecondaryWeapon() const
{
    return WeaponTable::getSingleton()->get(m_secondaryWeaponId);
}

WeaponTable::WeaponTable()
{
    ifstream file(filepath);

    nlohmann::json parsedWeapons;

    file >> parsedWeapons;

    for (auto& iter : parsedWeapons.items()) {
        // cout << i.key() << " : " << i.value() << endl;
        int id = std::stoi(iter.key());
        auto values = iter.value();

        auto componentJson = values["components"];

        // assert valid json
        assert(values["name"].is_null() == false);
        assert(values["description"].is_null() == false);
        assert(values["length"].is_null() == false);
        assert(values["type"].is_null() == false);
        assert(values["description"].is_null() == false);
        assert(values["cost"].is_null() == false);
        assert(componentJson.size() > 0);

        string weaponName = values["name"];
        string description = values["description"];
        eLength length = convertLengthFromStr(values["length"]);
        eWeaponTypes weaponType = convertTypeFromStr(values["type"]);
        int cost = values["cost"];
        int secondaryWeaponId = -1;
        int baseTN = cBaseBTN;
        int guardTN = baseTN;
        vector<Component*> weaponComponents;
        set<eWeaponProperties> weaponProperties;
        bool isSecondary = false;
        bool isShield = false;

        if (values["secondary"].is_null() == false) {
            isSecondary = values["secondary"];
        }
        if (values["secondaryId"].is_null() == false) {
            secondaryWeaponId = values["secondaryId"];
        }
        if (values["baseTn"].is_null() == false) {
            baseTN = values["baseTn"];
        }
        if (values["guardTn"].is_null() == false) {
            guardTN = values["guardTn"];
        }
        if (values["shield"].is_null() == false) {
            isShield = values["shield"];
        }

        if (values["properties"].is_null() == false) {
            auto propertiesJson = values["properties"];
            for (unsigned j = 0; j < propertiesJson.size(); ++j) {
                eWeaponProperties property = convertPropertiesFromStr(propertiesJson[j]);
                weaponProperties.insert(property);
            }
        }

        for (unsigned i = 0; i < componentJson.size(); ++i) {
            // cout << components[i] << endl;
            assert(componentJson[i]["name"].is_null() == false);
            assert(componentJson[i]["damage"].is_null() == false);
            assert(componentJson[i]["type"].is_null() == false);
            assert(componentJson[i]["attack"].is_null() == false);

            string componentName = componentJson[i]["name"];
            int damage = componentJson[i]["damage"];
            eDamageTypes damageType = convertDamageFromStr(componentJson[i]["type"]);
            eAttacks attack = convertAttackFromStr(componentJson[i]["attack"]);
            std::set<eWeaponProperties> properties;
            int tn = baseTN;

            // check for component properties
            if (componentJson[i]["properties"].is_null() == false) {
                // is an array
                auto propertiesJson = componentJson[i]["properties"];
                for (unsigned j = 0; j < propertiesJson.size(); ++j) {
                    eWeaponProperties property = convertPropertiesFromStr(propertiesJson[j]);
                    properties.insert(property);
                }
            }
            if (componentJson[i]["tn"].is_null() == false) {
                tn = componentJson[i]["tn"];
            }
            Component* component
                = new Component(componentName, damage, damageType, attack, tn, properties);

            weaponComponents.push_back(component);
        }

        Weapon* weapon
            = new Weapon(weaponName, description, length, weaponComponents, weaponType, cost,
                baseTN, guardTN, weaponProperties, isSecondary, isShield, false, secondaryWeaponId);
        assert(m_weaponsList.find(id) == m_weaponsList.end());
        m_weaponsList[id] = weapon;
        ItemTable::getSingleton()->addWeapon(id, weapon);
    }

    createNaturalWeapons();
}

void WeaponTable::createNaturalWeapons()
{
    string name = "Fists";
    set<eWeaponProperties> properties;
    std::vector<Component*> components;
    components.push_back(
        new Component("Fist", -1, eDamageTypes::Blunt, eAttacks::Swing, 6, properties));
    components.push_back(
        new Component("Fist", -1, eDamageTypes::Blunt, eAttacks::Thrust, 6, properties));
    Weapon* fists = new Weapon(name, "For punching", eLength::Hand, components,
        eWeaponTypes::Brawling, 0, 6, 7, properties, false, false, true, -1);
    assert(m_weaponsList.find(cFistsId) == m_weaponsList.end());
    m_weaponsList[cFistsId] = fists;
    ItemTable::getSingleton()->addWeapon(cFistsId, fists);
}

WeaponTable::~WeaponTable()
{
    for (auto it : m_weaponsList) {
        delete it.second;
    }
    m_weaponsList.clear();
}

const Weapon* WeaponTable::get(int id) const
{
    if (id == -1) {
        return nullptr;
    }
    auto it = m_weaponsList.find(id);
    assert(it != m_weaponsList.end());
    return it->second;
}

eLength WeaponTable::convertLengthFromStr(const std::string& str)
{
    if (str == "ludicrous") {
        return eLength::Ludicrous;
    } else if (str == "extreme") {
        return eLength::Extreme;
    } else if (str == "extended") {
        return eLength::Extended;
    } else if (str == "verylong") {
        return eLength::VeryLong;
    } else if (str == "long") {
        return eLength::Long;
    } else if (str == "medium") {
        return eLength::Medium;
    } else if (str == "short") {
        return eLength::Short;
    }
    return eLength::Hand;
}

eWeaponTypes WeaponTable::convertTypeFromStr(const std::string& str)
{
    if (str == "massweapon") {
        return eWeaponTypes::Mass;
    }
    if (str == "polearm") {
        return eWeaponTypes::Polearms;
    }
    if (str == "sword") {
        return eWeaponTypes::Swords;
    }
    if (str == "longsword") {
        return eWeaponTypes::Longswords;
    }

    return eWeaponTypes::Brawling;
}

eWeaponProperties WeaponTable::convertPropertiesFromStr(const std::string& str)
{
    if (str == "maillepiercing") {
        return eWeaponProperties::MaillePiercing;
    }
    if (str == "crushing") {
        return eWeaponProperties::Crushing;
    }
    if (str == "hook") {
        return eWeaponProperties::Hook;
    }
    if (str == "heavy") {
        return eWeaponProperties::Heavy;
    }
    if (str == "platepiercing") {
        return eWeaponProperties::PlatePiercing;
    }
    assert(true);
    return eWeaponProperties::Hook;
}

eDamageTypes WeaponTable::convertDamageFromStr(const std::string& str)
{
    if (str == "blunt") {
        return eDamageTypes::Blunt;
    } else if (str == "piercing") {
        return eDamageTypes::Piercing;
    }
    return eDamageTypes::Cutting;
}

eAttacks WeaponTable::convertAttackFromStr(const std::string& str)
{
    if (str == "thrust") {
        return eAttacks::Thrust;
    }
    return eAttacks::Swing;
}
