#include <fstream>

#include "equipmentmanager.h"
#include "utils.h"
#include "../3rdparty/json.hpp"
#include "../3rdparty/random.hpp"

EquipmentManager *EquipmentManager::singleton = nullptr;

using namespace std;

const string filepath = "data/loadouts.json";
const string nameFilepath = "data/names.json";

EquipmentManager::EquipmentManager()
{
    ifstream file(filepath);

    nlohmann::json parsedLoadouts;

    file >> parsedLoadouts;

    for(auto &iter : parsedLoadouts.items()) {
        string key = iter.key();
        auto values = iter.value();

        std::vector<int> baseChests = values["baseChests"];
		std::vector<int> baseLegs = values["baseLegs"];
		std::vector<int> helmets = values["helmets"];
		std::vector<int> chests = values["chests"];
		std::vector<int> leggings = values["leggings"];
        std::vector<int> weapons = values["weapons"];
        std::vector<int> gloves = values["gloves"];
        std::vector<int> shoes = values["shoes"];
		
		eCreatureFaction faction = stringToFaction(key);

		m_loadouts[faction].baseChests = baseChests;
		m_loadouts[faction].helmets = helmets;
		m_loadouts[faction].baseLegs = baseLegs;
		m_loadouts[faction].chests = chests;
		m_loadouts[faction].leggings = leggings;		
		m_loadouts[faction].weapons = weapons;
		m_loadouts[faction].gloves = gloves;
		m_loadouts[faction].shoes = shoes;
    }

	loadNames();
}

void EquipmentManager::loadNames()
{
    ifstream file(nameFilepath);

    nlohmann::json parsedNames;

    file >> parsedNames;

	for(auto &iter : parsedNames.items()) {
		string key = iter.key();
		auto values = iter.value();
		assert(values["first"].is_null() == false);
		std::vector<string> names = values["first"];
		eCreatureRace race = stringToRace(key);

		m_names[race] = names;
	}
}

std::string EquipmentManager::getRandomName(eCreatureRace race) const
{
	auto it = m_names.find(race);
	assert(it != m_names.end());
	std::vector<string> names = it->second;
	return names[effolkronium::random_static::get(0, static_cast<int>(names.size())-1)];	
}

std::vector<int> EquipmentManager::getRandomArmors(eCreatureFaction faction) const
{
	auto it = m_loadouts.find(faction);
	assert(it != m_loadouts.end());

	Loadout loadout = it->second;
	std::vector<int> ret;
	if(loadout.baseChests.size() > 0) {
		ret.push_back(
			loadout.baseChests[effolkronium::random_static::get(0, static_cast<int>(loadout.baseChests.size())-1)]);
	}
	if(loadout.baseLegs.size() > 0) {
		ret.push_back(
			loadout.baseLegs[effolkronium::random_static::get(0, static_cast<int>(loadout.baseLegs.size())-1)]);
	}
	if(loadout.chests.size() > 0) {
		ret.push_back(
			loadout.helmets[effolkronium::random_static::get(0, static_cast<int>(loadout.helmets.size())-1)]);
	}
	if(loadout.chests.size() > 0) {
		ret.push_back(
			loadout.chests[effolkronium::random_static::get(0, static_cast<int>(loadout.chests.size())-1)]);
	}
	if(loadout.leggings.size() > 0) {
		ret.push_back(
			loadout.leggings[effolkronium::random_static::get(0, static_cast<int>(loadout.leggings.size())-1)]);
	}
	if(loadout.gloves.size() > 0) {
		ret.push_back(
			loadout.gloves[effolkronium::random_static::get(0, static_cast<int>(loadout.gloves.size())-1)]);
	}
	if(loadout.shoes.size() > 0) {
		ret.push_back(
			loadout.shoes[effolkronium::random_static::get(0, static_cast<int>(loadout.shoes.size())-1)]);
	}
	return ret;
}

int EquipmentManager::getRandomWeapon(eCreatureFaction faction) const
{
	auto it = m_loadouts.find(faction);
	assert(it != m_loadouts.end());

	std::vector<int> weapons = it->second.weapons;
	return weapons[effolkronium::random_static::get(0, static_cast<int>(weapons.size())-1)];
}
