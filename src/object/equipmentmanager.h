#pragma once

#include <map>
#include <string>
#include <vector>

#include "types.h"

enum class eRank : unsigned { Recruit, Soldier, Veteran, Knight, Lord };

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

    std::vector<int> getRandomArmors(eCreatureFaction) const;
    int getRandomWeapon(eCreatureFaction) const;

private:
    struct Loadout {
        std::vector<int> baseChests;
        std::vector<int> baseLegs;
        std::vector<int> helmets;
        std::vector<int> chests;
        std::vector<int> leggings;
        std::vector<int> gloves;
        std::vector<int> shoes;
        std::vector<int> weapons;
    };
    EquipmentManager();
    void loadNames();

    static EquipmentManager* singleton;

    std::map<eCreatureFaction, Loadout> m_loadouts;
    std::map<eCreatureRace, std::vector<std::string>> m_names;
};
