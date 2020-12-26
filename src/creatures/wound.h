#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "../3rdparty/json.hpp"
#include "../items/types.h"
#include "types.h"

class Wound {
public:
    Wound(eBodyParts location, std::vector<std::string> text, int level, int btn, int pain,
        int impact, std::set<eEffects> effects);

    std::string getText() const { return m_text[0]; }
    int getBTN() const { return m_btn; }
    int getPain() const { return m_pain; }
    int getImpact() const { return m_impact; }
    int getLevel() const { return m_level; }
    eBodyParts getLocation() const { return m_location; }
    bool causesDeath();
    bool immediateKO();
    const std::set<eEffects>& getEffects() const { return m_effects; }

private:
    eBodyParts m_location;
    std::vector<std::string> m_text;
    int m_level;
    int m_impact;
    int m_btn;
    int m_pain;
    std::set<eEffects> m_effects;
};

class WoundTable {
public:
    ~WoundTable();
    eBodyParts getSwing(eHitLocations location);
    eBodyParts getThrust(eHitLocations location);

    std::vector<eBodyParts> getUniqueParts(eHitLocations location) const
    {
        return m_partsTable.at(location);
    }

    std::vector<eBodyParts> getPinpointThrustTargets(eHitLocations location, bool canHitX) const;

    Wound* getWound(eDamageTypes type, eBodyParts part, int level);

    static constexpr unsigned cPartsPerLocation = 10;

    struct woundParts {
        eBodyParts m_swing[cPartsPerLocation];
        eBodyParts m_thrust[cPartsPerLocation];
    };

    static WoundTable* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new WoundTable;
        }

        return singleton;
    }

    static void cleanupSingleton()
    {
        if (singleton != nullptr) {
            delete singleton;
            singleton = nullptr;
        }
    }

private:
    WoundTable();
    void initHitLocationTable();
    void initWoundTable(eDamageTypes type, nlohmann::json woundJson);
    eEffects stringToEffect(const std::string& str);

    static WoundTable* singleton;

    std::unordered_map<eHitLocations, woundParts> m_hitTable;
    std::unordered_map<eHitLocations, std::vector<eBodyParts>> m_partsTable;

    // 4d associative array for wound table
    // damage type to body part to wound level
    std::unordered_map<eDamageTypes,
        std::unordered_map<eBodyParts, std::unordered_map<int, Wound*>>>
        m_woundTable;

    std::unordered_map<eDamageTypes, std::vector<int>> m_btnTable;
    std::unordered_map<eDamageTypes, std::vector<int>> m_impactTable;
    std::unordered_map<eDamageTypes, std::vector<int>> m_painTable;
};
