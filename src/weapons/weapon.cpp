#include <fstream>
#include <iostream>
#include <assert.h>

#include "weapon.h"
#include "../3rdparty/json.hpp"

WeaponTable* WeaponTable::singleton = nullptr;

using namespace std;

const string filepath = "data/weapons.json";

Weapon::Weapon(std::string name, eLength length, std::vector<Component> components, eWeaponTypes type) :
	m_name(name), m_length(length), m_components(components), m_type(type)
{
	
}

WeaponTable::WeaponTable()
{
	ifstream file(filepath);

	nlohmann::json parsedWeapons;
	
	file >> parsedWeapons;

	int size = parsedWeapons.size();
	cout << size << endl;

	for(auto &iter : parsedWeapons.items()) {
		//cout << i.key() << " : " << i.value() << endl;
		int id = std::stoi(iter.key());
		auto values = iter.value();

		string weaponName = values["name"];
		eLength length = convertLengthFromStr(values["length"]);
		eWeaponTypes weaponType = convertTypeFromStr(values["type"]);
		vector<Component> weaponComponents;
		
		auto components = values["components"];
		for(int i = 0; i < components.size(); ++i) {
			//cout << components[i] << endl;
			string componentName = components[i]["name"];
			int damage = components[i]["damage"];
			eDamageTypes damageType = convertDamageFromStr(components[i]["type"]);
			std::set<eWeaponProperties> properties;

			//check for null here
			if(components[i]["properties"].is_null() == false)	{
				//is an array
				auto properties = components[i]["properties"];
				for(int j = 0; j < properties.size(); ++j) {
					eWeaponProperties property = convertPropertiesFromStr(properties[j]);
				}
			}

			Component component(componentName, damage, damageType, properties);
		}

		Weapon* weapon = new Weapon(weaponName, length, weaponComponents, weaponType);
		assert(m_weaponsList.find(id) == m_weaponsList.end());
		m_weaponsList[id] = weapon;
	}
}

WeaponTable::~WeaponTable()
{
}

Weapon* WeaponTable::get(int id)
{
	auto it = m_weaponsList.find(id);
	assert(it != m_weaponsList.end());
	return it->second;
}

eLength WeaponTable::convertLengthFromStr(const std::string& str)
{
	if(str == "ludicrous") {
		return eLength::Ludicrous;
	}
	else if(str == "extreme") {
		return eLength::Extreme;
	}
	else if(str == "extended") {
		return eLength::Extended;
	}
	else if(str == "long") {
		return eLength::Long;
	}
	else if(str == "medium") {
		return eLength::Medium;
	}
	else if(str == "short") {
		return eLength::Short;
	}
	return eLength::Hand;
}

eWeaponTypes WeaponTable::convertTypeFromStr(const std::string& str)
{
	if(str == "polearm") {
		return eWeaponTypes::Polearms;
	}
	if(str == "sword") {
		return eWeaponTypes::Swords;
	}
	
	return eWeaponTypes::Brawling;
}

eWeaponProperties WeaponTable::convertPropertiesFromStr(const std::string& str)
{
	if(str == "maillepiercing") {
		return eWeaponProperties::MaillePiercing;
	}
	if(str == "crushing") {
		return eWeaponProperties::Crushing;
	}
	if(str == "hook") {
		return eWeaponProperties::Hook;
	}
}

eDamageTypes WeaponTable::convertDamageFromStr(const std::string& str)
{
	if(str == "blunt") {
		return eDamageTypes::Blunt;
	}
	else if (str == "piercing") {
		return eDamageTypes::Piercing;
	}
	return eDamageTypes::Cutting;
}

Weapon::Weapon()
{
}


