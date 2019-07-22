#pragma once

#include <map>

#include "nameable.h"
#include "../object/nameable.h"
#include "types.h"

class Item : public Nameable
{
public:
	Item(const std::string& name, const std::string& description, int cost) : Nameable(name, description), m_cost(cost) {}
	
	virtual eItemType getItemType() const { return eItemType::Item; }
	int getCost() const { return m_cost; }
private:
	int m_cost;
};

class Weapon;
class Armor;

class ItemTable
{
public:
	static ItemTable* getSingleton() {
		if(singleton == nullptr) {
			singleton = new ItemTable;
		}
		return singleton;
	}
	static ItemTable* singleton;

	const Item* get(int id) const;
private:
	//would it be better just to have all items/armors/weapons in the same json and stored here?
	//probably, it would aovid this weird bit setting indexes
	//but i already wrote this code so i have to live with it
	friend class ArmorTable;
	friend class WeaponTable;
	void addWeapon(int id, Weapon* weapon);
	void addArmor(int id, Armor* armor);
	ItemTable();
	std::map<int, Item*> m_itemList;
};
