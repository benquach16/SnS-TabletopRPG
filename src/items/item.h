#pragma once

#include "nameable.h"
#include "../object/nameable.h"

enum class eItemType {
	Item,
	Armor,
	Weapon
};

class Item : public Nameable
{
public:
	Item(const std::string& name) : Nameable(name) {}

	virtual eItemType getItemType() { return eItemType::Item; }
};

class ItemTable
{
public:
	static const ItemTable* getSingleton() {
		if(singleton == nullptr) {
			singleton = new ItemTable;
		}
		return singleton;
	}
	static ItemTable* singleton;

private:
};
