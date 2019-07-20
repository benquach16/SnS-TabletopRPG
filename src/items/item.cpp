#include <assert.h>
#include <iostream>
#include <fstream>

#include "item.h"
#include "weapon.h"
#include "armor.h"
#include "../3rdparty/json.hpp"

ItemTable* ItemTable::singleton = nullptr;

using namespace std;

const string filepath = "data/items.json";

ItemTable::ItemTable()
{
	ifstream file(filepath);

	nlohmann::json parsedItems;

	file >> parsedItems;

	for(auto &iter : parsedItems.items()) {
		int id = stoi(iter.key());
		auto values = iter.value();

		//asert valid json
		assert(values["name"].is_null() == false);
		assert(values["description"].is_null() == false);
		assert(values["cost"].is_null() == false);
		assert(values["type"].is_null() == false);
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

