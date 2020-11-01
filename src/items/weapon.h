#pragma once
#include "component.h"
#include "item.h"

#include <string>
#include <unordered_map>
#include <vector>

constexpr int cFistsId = 1000;

class Weapon : public Item {
public:
    Weapon(const std::string& name, const std::string& description, eLength length,
        std::vector<Component*> components, eWeaponTypes type, int cost, bool hook,
        bool naturalWeapon = false, int secondaryWeaponId = -1);
    ~Weapon();
    eItemType getItemType() const override { return eItemType::Weapon; }

    eLength getLength() const { return m_length; }
    eWeaponTypes getType() const { return m_type; }
    const std::vector<Component*>& getComponents() const { return m_components; }
    std::vector<Component*> getThrustComponents() const { return m_thrustComponents; }
    std::vector<Component*> getSwingComponents() const { return m_swingComponents; }
    // convinence functions
    Component* getBestAttack() const;

    Component* getBestThrust() const;

    Component* getBestBlunt() const;

    Component* getPommelStrike() const;

    bool canHook() const { return m_hook; }

    bool getNaturalWeapon() const { return m_naturalWeapon; }

    const Weapon* getSecondaryWeapon() const;

private:
    Weapon();
    eLength m_length;
    std::vector<Component*> m_components;
    std::vector<Component*> m_thrustComponents;
    std::vector<Component*> m_swingComponents;

    eWeaponTypes m_type;

    int m_secondaryWeaponId;

    int m_hands;

    bool canBeOffhand() const { return m_hands == 1; }
    bool m_naturalWeapon;
    bool m_hook;
};

class WeaponTable {
public:
    static const WeaponTable* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new WeaponTable;
        }
        return singleton;
    }
    const Weapon* get(int id) const;

private:
    WeaponTable();
    ~WeaponTable();

    void createNaturalWeapons();

    eLength convertLengthFromStr(const std::string& str);
    eWeaponTypes convertTypeFromStr(const std::string& str);
    eWeaponProperties convertPropertiesFromStr(const std::string& str);
    eDamageTypes convertDamageFromStr(const std::string& str);
    eAttacks convertAttackFromStr(const std::string& str);

    static WeaponTable* singleton;

    //<id, weapon>
    std::unordered_map<int, Weapon*> m_weaponsList;
};
