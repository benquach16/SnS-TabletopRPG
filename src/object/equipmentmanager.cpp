#include <fstream>

#include "../3rdparty/json.hpp"
#include "../3rdparty/random.hpp"
#include "equipmentmanager.h"
#include "utils.h"

EquipmentManager* EquipmentManager::singleton = nullptr;

using namespace std;
using namespace effolkronium;

const string filepath = "data/loadouts.json";
const string nameFilepath = "data/names.json";

EquipmentManager::EquipmentManager()
{
    ifstream file(filepath);

    nlohmann::json parsedLoadouts;

    file >> parsedLoadouts;

    for (auto& iter : parsedLoadouts.items()) {
        string key = iter.key();
        auto values = iter.value();

        std::vector<int> baseChests = values["baseChests"];
        std::vector<int> baseLegs = values["baseLegs"];
        std::vector<int> helmets = values["helmets"];
        std::vector<int> chests = values["chests"];
        std::vector<int> plateChests = values["plateChests"];
        std::vector<int> leggings = values["leggings"];
        std::vector<int> plateLeggings = values["plateLeggings"];
        std::vector<int> skirts = values["skirts"];
        std::vector<int> weapons = values["weapons"];
        std::vector<int> gloves = values["gloves"];
        std::vector<int> plateGloves = values["plateGloves"];
        std::vector<int> shoes = values["shoes"];

        eCreatureFaction faction = stringToFaction(key);

        m_loadouts[faction].baseChests = baseChests;
        m_loadouts[faction].helmets = helmets;
        m_loadouts[faction].baseLegs = baseLegs;
        m_loadouts[faction].chests = chests;
        m_loadouts[faction].plateChests = plateChests;
        m_loadouts[faction].leggings = leggings;
        m_loadouts[faction].skirts = skirts;
        m_loadouts[faction].plateLeggings = plateLeggings;
        m_loadouts[faction].weapons = weapons;
        m_loadouts[faction].gloves = gloves;
        m_loadouts[faction].plateGloves = plateGloves;
        m_loadouts[faction].shoes = shoes;
    }

    loadNames();
}

void EquipmentManager::loadNames()
{
    ifstream file(nameFilepath);

    nlohmann::json parsedNames;

    file >> parsedNames;

    for (auto& iter : parsedNames.items()) {
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
    return names[effolkronium::random_static::get(0, static_cast<int>(names.size()) - 1)];
}

std::vector<int> EquipmentManager::getRandomArmors(eCreatureFaction faction) const
{
    auto it = m_loadouts.find(faction);
    assert(it != m_loadouts.end());

    Loadout loadout = it->second;
    std::vector<int> ret;
    if (loadout.baseChests.size() > 0) {
        ret.push_back(loadout.baseChests[random_static::get(
            0, static_cast<int>(loadout.baseChests.size()) - 1)]);
    }
    if (loadout.baseLegs.size() > 0) {
        ret.push_back(
            loadout.baseLegs[random_static::get(0, static_cast<int>(loadout.baseLegs.size()) - 1)]);
    }
    if (loadout.helmets.size() > 0) {
        ret.push_back(
            loadout.helmets[random_static::get(0, static_cast<int>(loadout.helmets.size()) - 1)]);
    }
    if (loadout.chests.size() > 0) {
        ret.push_back(
            loadout.chests[random_static::get(0, static_cast<int>(loadout.chests.size()) - 1)]);
    }
    if (loadout.plateChests.size() > 0) {
        ret.push_back(loadout.plateChests[random_static::get(
            0, static_cast<int>(loadout.plateChests.size()) - 1)]);
    }
    if (loadout.leggings.size() > 0) {
        ret.push_back(
            loadout.leggings[random_static::get(0, static_cast<int>(loadout.leggings.size()) - 1)]);
    }
    if (loadout.skirts.size() > 0) {
        ret.push_back(
            loadout.skirts[random_static::get(0, static_cast<int>(loadout.skirts.size()) - 1)]);
    }
    if (loadout.plateLeggings.size() > 0) {
        ret.push_back(loadout.plateLeggings[random_static::get(
            0, static_cast<int>(loadout.plateLeggings.size()) - 1)]);
    }
    if (loadout.gloves.size() > 0) {
        ret.push_back(
            loadout.gloves[random_static::get(0, static_cast<int>(loadout.gloves.size()) - 1)]);
    }
    if (loadout.plateGloves.size() > 0) {
        ret.push_back(loadout.plateGloves[random_static::get(
            0, static_cast<int>(loadout.plateGloves.size()) - 1)]);
    }
    if (loadout.shoes.size() > 0) {
        ret.push_back(
            loadout.shoes[random_static::get(0, static_cast<int>(loadout.shoes.size()) - 1)]);
    }
    return ret;
}

int EquipmentManager::getRandomWeapon(eCreatureFaction faction) const
{
    auto it = m_loadouts.find(faction);
    assert(it != m_loadouts.end());

    std::vector<int> weapons = it->second.weapons;
    return weapons[effolkronium::random_static::get(0, static_cast<int>(weapons.size()) - 1)];
}
