#include <fstream>
#include <iostream>
#include <assert.h>

#include "weapon.h"
#include "../3rdparty/json.hpp"

WeaponTable* WeaponTable::singleton = nullptr;

using namespace std;

const string filepath = "data/weapons.json";

Weapon::Weapon(std::string name, eLength length, std::vector<Component*> components, eWeaponTypes type) :
	m_name(name), m_length(length), m_components(components), m_type(type)
{
	
}

Weapon::~Weapon()
{
	for(int i = 0; i < m_components.size(); ++i) {
		delete m_components[i];
	}
	m_components.clear();
}

Component* Weapon::getBestAttack()
{
	assert(m_components.size() > 0);

	Component* ret = m_components[0];
	for(int i = 1; i < m_components.size(); ++i) {
		if(m_components[i]->getDamage() > ret->getDamage()) {
			ret = m_components[i];
		}
	}

	return ret;
}

WeaponTable::WeaponTable()
{
	ifstream file(filepath);

	nlohmann::json parsedWeapons;
	
	file >> parsedWeapons;

	int size = parsedWeapons.size();

	for(auto &iter : parsedWeapons.items()) {
		//cout << i.key() << " : " << i.value() << endl;
		int id = std::stoi(iter.key());
		auto values = iter.value();

		auto componentJson = values["components"];
		
		//assert valid json
		assert(values["name"].is_null() == false);
		assert(values["length"].is_null() == false);
		assert(values["type"].is_null() == false);
		assert(componentJson.size() > 0);

		string weaponName = values["name"];
		eLength length = convertLengthFromStr(values["length"]);
		eWeaponTypes weaponType = convertTypeFromStr(values["type"]);
		vector<Component*> weaponComponents;

		for(int i = 0; i < componentJson.size(); ++i) {
			//cout << components[i] << endl;
			assert(componentJson[i]["name"].is_null() == false);
			assert(componentJson[i]["damage"].is_null() == false);
			assert(componentJson[i]["type"].is_null() == false);
			assert(componentJson[i]["attack"].is_null() == false);
			
			string componentName = componentJson[i]["name"];
			int damage = componentJson[i]["damage"];
			eDamageTypes damageType = convertDamageFromStr(componentJson[i]["type"]);
			eAttacks attack = convertAttackFromStr(componentJson[i]["attack"]);
			std::set<eWeaponProperties> properties;

			//check for component properties
			if(componentJson[i]["properties"].is_null() == false)	{
				//is an array
				auto properties = componentJson[i]["properties"];
				for(int j = 0; j < properties.size(); ++j) {
					eWeaponProperties property = convertPropertiesFromStr(properties[j]);
				}
			}

			Component* component = new Component(componentName, damage, damageType, attack, properties);

			weaponComponents.push_back(component);
		}

		Weapon* weapon = new Weapon(weaponName, length, weaponComponents, weaponType);
		assert(m_weaponsList.find(id) == m_weaponsList.end());
		m_weaponsList[id] = weapon;
	}
}

WeaponTable::~WeaponTable()
{
	for(auto it : m_weaponsList) {
		delete it.second;
	}
	m_weaponsList.clear();
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

eAttacks WeaponTable::convertAttackFromStr(const std::string& str)
{
	if(str == "thrust") {
		return eAttacks::Thrust;
	}
	return eAttacks::Swing;
}

Weapon::Weapon()
{
}


