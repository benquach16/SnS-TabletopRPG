#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "../3rdparty/json.hpp"
#include "types.h"

class EquipmentManager {
public:
    static EquipmentManager* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new EquipmentManager;
        }
        return singleton;
    }

    std::string getRandomName(eCreatureRace race) const;

    std::vector<int> getRandomArmors(eCreatureFaction faction, eRank rank) const;
    int getRandomWeapon(eCreatureFaction faction, eRank rank) const;

private:
    struct Loadout {
        std::vector<int> baseChests;
        std::vector<int> baseLegs;
        std::vector<int> helmets;
        std::vector<int> neck;
        std::vector<int> chests;
        std::vector<int> plateChests;
        std::vector<int> skirts;
        std::vector<int> leggings;
        std::vector<int> plateLeggings;
        std::vector<int> plateShoulders;
        std::vector<int> plateArms;
        std::vector<int> gloves;
        std::vector<int> plateGloves;
        std::vector<int> shoes;
        std::vector<int> weapons;
    };
    EquipmentManager();
    void loadNames();
    void loadLoadout(eCreatureFaction faction, eRank rank, nlohmann::json values);

    static EquipmentManager* singleton;

    std::unordered_map<eCreatureFaction, std::unordered_map<eRank, Loadout>> m_loadouts;
    std::unordered_map<eCreatureRace, std::vector<std::string>> m_names;
};
