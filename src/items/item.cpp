#include <assert.h>
#include <fstream>
#include <iostream>

#include "../3rdparty/json.hpp"
#include "armor.h"
#include "item.h"
#include "weapon.h"

ItemTable* ItemTable::singleton = nullptr;

using namespace std;

const string filepath = "data/items.json";

ItemTable::ItemTable()
{
    ifstream file(filepath);

    nlohmann::json parsedItems;

    file >> parsedItems;

    for (auto& iter : parsedItems.items()) {
        int id = stoi(iter.key());
        auto values = iter.value();

        //asert valid json
        assert(values["name"].is_null() == false);
        assert(values["description"].is_null() == false);
        assert(values["cost"].is_null() == false);
        assert(values["type"].is_null() == false);

        string name = values["name"];
        string description = values["description"];
        int cost = values["cost"];
        string type = values["type"];

        Item* item = new Item(name, description, cost);

        assert(m_itemList.find(id) == m_itemList.end());
        m_itemList[id] = item;
    }
}

const Item* ItemTable::get(int id) const
{
    auto it = m_itemList.find(id);
    assert(it != m_itemList.end());
    return it->second;
}

void ItemTable::addWeapon(int id, Weapon* weapon)
{
    //IDs should not overlap. Even across different JSON files
    assert(m_itemList.find(id) == m_itemList.end());
    m_itemList[id] = weapon;
}

void ItemTable::addArmor(int id, Armor* armor)
{
    //IDs should not overlap.
    assert(m_itemList.find(id) == m_itemList.end());
    m_itemList[id] = armor;
}
