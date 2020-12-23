#pragma once

#include <iostream>
#include <unordered_map>

#include "../object/nameable.h"
#include "nameable.h"
#include "types.h"

class Item : public Nameable {
public:
    Item(const std::string& name, const std::string& description, int cost, eItemType type)
        : Nameable(name, description)
        , m_cost(cost)
        , m_type(type)
    {
    }

    virtual ~Item() {}
    virtual eItemType getItemType() const { return m_type; }
    int getCost() const { return m_cost; }

private:
    int m_cost;
    eItemType m_type;
};

class Weapon;
class Armor;

class ItemTable {
public:
    ~ItemTable();
    static ItemTable* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new ItemTable;
        }
        return singleton;
    }
    static void cleanupSingleton()
    {
        if (singleton != nullptr) {
            delete singleton;
            singleton = nullptr;
        }
    }

    const Item* get(int id) const;

private:
    // would it be better just to have all items/armors/weapons in the same json
    // and stored here? probably, it would aovid this weird bit setting indexes
    // but i already wrote this code so i have to live with it
    friend class ArmorTable;
    friend class WeaponTable;
    void addWeapon(int id, Weapon* weapon);
    void addArmor(int id, Armor* armor);
    ItemTable();
    static ItemTable* singleton;
    std::unordered_map<int, Item*> m_itemList;
};
