#include "wound.h"
#include "../dice.h"
#include "utils.h"

#include <assert.h>
#include <fstream>
#include <iostream>

using namespace std;

const string filepath = "data/wounds.json";

WoundTable* WoundTable::singleton = nullptr;

constexpr int woundLevels = 5;

Wound::Wound(eBodyParts location, std::vector<std::string> text, int level, int btn, int pain,
    int impact, std::set<eEffects> effects)
    : m_location(location)
    , m_text(text)
    , m_level(level)
    , m_btn(btn)
    , m_pain(pain)
    , m_impact(impact)
    , m_effects(effects)
{
}

bool Wound::causesDeath()
{
    auto it = m_effects.find(eEffects::Death);
    return (it != m_effects.end());
}

bool Wound::immediateKO()
{
    auto it = m_effects.find(eEffects::KO);
    return (it != m_effects.end());
}

WoundTable::WoundTable()
{
    ifstream file(filepath);

    nlohmann::json parsedWounds;

    file >> parsedWounds;

    auto bluntJson = parsedWounds["blunt"];
    auto piercingJson = parsedWounds["piercing"];
    auto cuttingJson = parsedWounds["cutting"];

    vector<int> bluntBtn = bluntJson["BTN"];
    vector<int> bluntImpact = bluntJson["impact"];
    vector<int> bluntPain = bluntJson["pain"];
    m_btnTable[eDamageTypes::Blunt] = bluntBtn;
    m_impactTable[eDamageTypes::Blunt] = bluntImpact;
    m_painTable[eDamageTypes::Blunt] = bluntPain;
    vector<int> piercingBtn = piercingJson["BTN"];
    vector<int> piercingImpact = piercingJson["impact"];
    vector<int> piercingPain = piercingJson["pain"];
    m_btnTable[eDamageTypes::Piercing] = piercingBtn;
    m_impactTable[eDamageTypes::Piercing] = piercingImpact;
    m_painTable[eDamageTypes::Piercing] = piercingPain;
    vector<int> cuttingBtn = cuttingJson["BTN"];
    vector<int> cuttingImpact = cuttingJson["impact"];
    vector<int> cuttingPain = cuttingJson["pain"];
    m_btnTable[eDamageTypes::Cutting] = cuttingBtn;
    m_impactTable[eDamageTypes::Cutting] = cuttingImpact;
    m_painTable[eDamageTypes::Cutting] = cuttingPain;
    initWoundTable(eDamageTypes::Blunt, bluntJson);
    initWoundTable(eDamageTypes::Piercing, piercingJson);
    initWoundTable(eDamageTypes::Cutting, cuttingJson);
    initHitLocationTable();
}

WoundTable::~WoundTable()
{
    for (auto damage : m_woundTable) {
        for (auto parts : damage.second) {
            for (auto levels : parts.second) {
                delete levels.second;
            }
        }
    }
}

void WoundTable::initWoundTable(eDamageTypes type, nlohmann::json woundJson)
{
    for (auto& iter : woundJson.items()) {
        string key = iter.key();

        // ignore BTN and impact tables
        if (key == "BTN" || key == "impact" || key == "pain") {
            continue;
        }
        eBodyParts bodyPart = stringToBodyPart(key);
        auto values = iter.value();
        for (int i = 1; i <= woundLevels; ++i) {
            std::string index = std::to_string(i);
            auto woundJson = values[index];

            set<eEffects> effects;
            if (woundJson["effects"].is_null() == false) {
                for (unsigned j = 0; j < woundJson["effects"].size(); ++j) {
                    eEffects effect = stringToEffect(woundJson["effects"][j]);
                    effects.insert(effect);
                }
            }
            Wound* wound = new Wound(bodyPart, woundJson["text"], i, m_btnTable[type][i - 1],
                m_painTable[type][i - 1], m_impactTable[type][i - 1], effects);

            m_woundTable[type][bodyPart][i] = wound;
        }
    }
}

void WoundTable::initHitLocationTable()
{
    // Head
    m_hitTable[eHitLocations::Head].m_swing[0] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_swing[1] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_swing[2] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_swing[3] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_swing[4] = eBodyParts::Face;
    m_hitTable[eHitLocations::Head].m_swing[5] = eBodyParts::Chin;
    m_hitTable[eHitLocations::Head].m_swing[6] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Head].m_swing[7] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Head].m_swing[8] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Head].m_swing[9] = eBodyParts::Shoulder;

    m_hitTable[eHitLocations::Head].m_thrust[0] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_thrust[1] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_thrust[2] = eBodyParts::Crown;
    m_hitTable[eHitLocations::Head].m_thrust[3] = eBodyParts::Face;
    m_hitTable[eHitLocations::Head].m_thrust[4] = eBodyParts::Face;
    m_hitTable[eHitLocations::Head].m_thrust[5] = eBodyParts::Face;
    m_hitTable[eHitLocations::Head].m_thrust[6] = eBodyParts::Chin;
    m_hitTable[eHitLocations::Head].m_thrust[7] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Head].m_thrust[8] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Head].m_thrust[9] = eBodyParts::Neck;

    m_partsTable[eHitLocations::Head].push_back(eBodyParts::Crown);
    m_partsTable[eHitLocations::Head].push_back(eBodyParts::Face);
    m_partsTable[eHitLocations::Head].push_back(eBodyParts::Neck);
    m_partsTable[eHitLocations::Head].push_back(eBodyParts::Chin);

    // chest
    m_hitTable[eHitLocations::Chest].m_swing[0] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Chest].m_swing[1] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Chest].m_swing[2] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Chest].m_swing[3] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Chest].m_swing[4] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Chest].m_swing[5] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_swing[6] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_swing[7] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_swing[8] = eBodyParts::SecondLocationArm;
    m_hitTable[eHitLocations::Chest].m_swing[9] = eBodyParts::SecondLocationHead;

    m_hitTable[eHitLocations::Chest].m_thrust[0] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Chest].m_thrust[1] = eBodyParts::Neck;
    m_hitTable[eHitLocations::Chest].m_thrust[2] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_thrust[3] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_thrust[4] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_thrust[5] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_thrust[6] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_thrust[7] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Chest].m_thrust[8] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Chest].m_thrust[9] = eBodyParts::Abs;

    m_partsTable[eHitLocations::Chest].push_back(eBodyParts::Ribs);
    // m_partsTable[eHitLocations::Chest].push_back(eBodyParts::Abs);
    m_partsTable[eHitLocations::Chest].push_back(eBodyParts::Armpit);

    // arms
    m_hitTable[eHitLocations::Arm].m_swing[0] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Arm].m_swing[1] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Arm].m_swing[2] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Arm].m_swing[3] = eBodyParts::UpperArm;
    m_hitTable[eHitLocations::Arm].m_swing[4] = eBodyParts::UpperArm;
    m_hitTable[eHitLocations::Arm].m_swing[5] = eBodyParts::UpperArm;
    m_hitTable[eHitLocations::Arm].m_swing[6] = eBodyParts::Forearm;
    m_hitTable[eHitLocations::Arm].m_swing[7] = eBodyParts::Forearm;
    m_hitTable[eHitLocations::Arm].m_swing[8] = eBodyParts::Hand;
    m_hitTable[eHitLocations::Arm].m_swing[9] = eBodyParts::Hand;

    m_hitTable[eHitLocations::Arm].m_thrust[0] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Arm].m_thrust[1] = eBodyParts::Shoulder;
    m_hitTable[eHitLocations::Arm].m_thrust[2] = eBodyParts::UpperArm;
    m_hitTable[eHitLocations::Arm].m_thrust[3] = eBodyParts::UpperArm;
    m_hitTable[eHitLocations::Arm].m_thrust[4] = eBodyParts::UpperArm;
    m_hitTable[eHitLocations::Arm].m_thrust[5] = eBodyParts::Forearm;
    m_hitTable[eHitLocations::Arm].m_thrust[6] = eBodyParts::Forearm;
    m_hitTable[eHitLocations::Arm].m_thrust[7] = eBodyParts::Forearm;
    m_hitTable[eHitLocations::Arm].m_thrust[8] = eBodyParts::Hand;
    m_hitTable[eHitLocations::Arm].m_thrust[9] = eBodyParts::Hand;

    m_partsTable[eHitLocations::Arm].push_back(eBodyParts::Shoulder);
    m_partsTable[eHitLocations::Arm].push_back(eBodyParts::UpperArm);
    m_partsTable[eHitLocations::Arm].push_back(eBodyParts::Forearm);
    m_partsTable[eHitLocations::Arm].push_back(eBodyParts::Hand);
    m_partsTable[eHitLocations::Arm].push_back(eBodyParts::Elbow);

    // belly
    m_hitTable[eHitLocations::Belly].m_swing[0] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_swing[1] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_swing[2] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_swing[3] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_swing[4] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_swing[5] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_swing[6] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Belly].m_swing[7] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Belly].m_swing[8] = eBodyParts::Groin;
    m_hitTable[eHitLocations::Belly].m_swing[9] = eBodyParts::Thigh;

    m_hitTable[eHitLocations::Belly].m_thrust[0] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Belly].m_thrust[1] = eBodyParts::Ribs;
    m_hitTable[eHitLocations::Belly].m_thrust[2] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_thrust[3] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_thrust[4] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_thrust[5] = eBodyParts::Abs;
    m_hitTable[eHitLocations::Belly].m_thrust[6] = eBodyParts::SecondLocationArm;
    m_hitTable[eHitLocations::Belly].m_thrust[7] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Belly].m_thrust[8] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Belly].m_thrust[9] = eBodyParts::Groin;

    // m_partsTable[eHitLocations::Belly].push_back(eBodyParts::Ribs);
    m_partsTable[eHitLocations::Belly].push_back(eBodyParts::Abs);
    m_partsTable[eHitLocations::Belly].push_back(eBodyParts::Groin);
    m_partsTable[eHitLocations::Belly].push_back(eBodyParts::Hip);

    // thigh
    m_hitTable[eHitLocations::Thigh].m_swing[0] = eBodyParts::Groin;
    m_hitTable[eHitLocations::Thigh].m_swing[1] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_swing[2] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_swing[3] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_swing[4] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_swing[5] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_swing[6] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_swing[7] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_swing[8] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_swing[9] = eBodyParts::Knee;

    m_hitTable[eHitLocations::Thigh].m_thrust[0] = eBodyParts::Groin;
    m_hitTable[eHitLocations::Thigh].m_thrust[1] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_thrust[2] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_thrust[3] = eBodyParts::Hip;
    m_hitTable[eHitLocations::Thigh].m_thrust[4] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_thrust[5] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_thrust[6] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_thrust[7] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_thrust[8] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Thigh].m_thrust[9] = eBodyParts::Knee;

    // m_partsTable[eHitLocations::Thigh].push_back(eBodyParts::Hip);
    // m_partsTable[eHitLocations::Thigh].push_back(eBodyParts::Groin);
    m_partsTable[eHitLocations::Thigh].push_back(eBodyParts::Thigh);
    m_partsTable[eHitLocations::Thigh].push_back(eBodyParts::Knee);
    // m_partsTable[eHitLocations::Thigh].push_back(eBodyParts::Shin);

    // shin
    m_hitTable[eHitLocations::Shin].m_swing[0] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Shin].m_swing[1] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Shin].m_swing[2] = eBodyParts::Knee;
    m_hitTable[eHitLocations::Shin].m_swing[3] = eBodyParts::Knee;
    m_hitTable[eHitLocations::Shin].m_swing[4] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_swing[5] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_swing[6] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_swing[7] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_swing[8] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_swing[9] = eBodyParts::Foot;

    m_hitTable[eHitLocations::Shin].m_thrust[0] = eBodyParts::Thigh;
    m_hitTable[eHitLocations::Shin].m_thrust[1] = eBodyParts::Knee;
    m_hitTable[eHitLocations::Shin].m_thrust[2] = eBodyParts::Knee;
    m_hitTable[eHitLocations::Shin].m_thrust[3] = eBodyParts::Knee;
    m_hitTable[eHitLocations::Shin].m_thrust[4] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_thrust[5] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_thrust[6] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_thrust[7] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_thrust[8] = eBodyParts::Shin;
    m_hitTable[eHitLocations::Shin].m_thrust[9] = eBodyParts::Foot;

    // m_partsTable[eHitLocations::Shin].push_back(eBodyParts::Thigh);
    // m_partsTable[eHitLocations::Shin].push_back(eBodyParts::Knee);
    m_partsTable[eHitLocations::Shin].push_back(eBodyParts::Shin);
    m_partsTable[eHitLocations::Shin].push_back(eBodyParts::Foot);
}

eEffects WoundTable::stringToEffect(const std::string& str)
{
    if (str == "KO1") {
        return eEffects::KO1;
    } else if (str == "KO2") {
        return eEffects::KO2;
    } else if (str == "KO3") {
        return eEffects::KO3;
    } else if (str == "KO") {
        return eEffects::KO;
    } else if (str == "BL1") {
        return eEffects::BL1;
    } else if (str == "BL2") {
        return eEffects::BL2;
    } else if (str == "BL3") {
        return eEffects::BL3;
    } else if (str == "drop1") {
        return eEffects::drop1;
    } else if (str == "drop2") {
        return eEffects::drop2;
    } else if (str == "drop3") {
        return eEffects::drop3;
    } else if (str == "drop") {
        return eEffects::drop;
    } else if (str == "KD1") {
        return eEffects::KD1;
    } else if (str == "KD2") {
        return eEffects::KD2;
    } else if (str == "KD3") {
        return eEffects::KD3;
    } else if (str == "KD") {
        return eEffects::KD;
    } else if (str == "severed") {
        return eEffects::Severed;
    } else if (str == "infection") {
        return eEffects::Infection;
    }
    return eEffects::Death;
}

bool WoundTable::isLimb(eBodyParts part) const
{
    switch (part) {
    case eBodyParts::Shoulder:
    case eBodyParts::UpperArm:
    case eBodyParts::Elbow:
    case eBodyParts::Forearm:
    case eBodyParts::Hand:
    case eBodyParts::Thigh:
    case eBodyParts::Knee:
    case eBodyParts::Shin:
    case eBodyParts::Foot:
        return true;
    }
    return false;
}

std::vector<eBodyParts> WoundTable::limbConnection(
    eBodyParts part, std::vector<eBodyParts> existingParts) const
{
    assert(isLimb(part));
    // maintain ordering, as it is important here
    vector<eBodyParts> arm;
    arm.push_back(eBodyParts::Shoulder);
    arm.push_back(eBodyParts::UpperArm);
    arm.push_back(eBodyParts::Elbow);
    arm.push_back(eBodyParts::Forearm);
    arm.push_back(eBodyParts::Hand);
    vector<eBodyParts> leg;
    arm.push_back(eBodyParts::Thigh);
    arm.push_back(eBodyParts::Knee);
    arm.push_back(eBodyParts::Shin);
    arm.push_back(eBodyParts::Foot);

    vector<eBodyParts> ret;
    bool found = false;
    for (unsigned i = 0; i < arm.size(); ++i) {
        if (arm[i] == part) {
            found = true;
        }
        if (found) {
            // do not add parts that don't exist
            // so that cutting off hand then forearm doesn't add hand twice
            if (std::find(existingParts.begin(), existingParts.end(), arm[i])
                != existingParts.end()) {
                ret.push_back(arm[i]);
            }
        }
    }
    if (found) {
        return ret;
    }
    for (unsigned i = 0; i < leg.size(); ++i) {
        if (leg[i] == part) {
            found = true;
        }
        if (found) {
            // do not add parts that don't exist
            // so that cutting off hand then forearm doesn't add hand twice
            if (std::find(existingParts.begin(), existingParts.end(), leg[i])
                != existingParts.end()) {
                ret.push_back(leg[i]);
            }
        }
    }
    return ret;
}

eBodyParts WoundTable::getSwing(eHitLocations location)
{
    // dice returns an actual dice roll 1-10, so we have to offset by 1
    int roll = DiceRoller::rollSides(cPartsPerLocation) - 1;
    eBodyParts part = m_hitTable[location].m_swing[roll];
    while (part == eBodyParts::SecondLocationHead) {
        roll = DiceRoller::rollSides(cPartsPerLocation) - 1;
        part = m_hitTable[eHitLocations::Head].m_swing[roll];
    }
    while (part == eBodyParts::SecondLocationArm) {
        roll = DiceRoller::rollSides(cPartsPerLocation) - 1;
        part = m_hitTable[eHitLocations::Arm].m_swing[roll];
    }
    return part;
}

eBodyParts WoundTable::getThrust(eHitLocations location)
{
    // dice returns an actual dice roll 1-10, so we have to offset by 1
    int roll = DiceRoller::rollSides(cPartsPerLocation) - 1;
    eBodyParts part = m_hitTable[location].m_thrust[roll];
    while (part == eBodyParts::SecondLocationHead) {
        roll = DiceRoller::rollSides(cPartsPerLocation) - 1;
        part = m_hitTable[eHitLocations::Head].m_thrust[roll];
    }
    while (part == eBodyParts::SecondLocationArm) {
        roll = DiceRoller::rollSides(cPartsPerLocation) - 1;
        part = m_hitTable[eHitLocations::Arm].m_thrust[roll];
    }
    return part;
}

std::vector<eBodyParts> WoundTable::getPinpointThrustTargets(
    eHitLocations location, bool canHitX) const
{
    std::vector<eBodyParts> ret;
    std::vector<eBodyParts> existingParts = m_partsTable.at(location);
    // do a deep copy for this one
    if (canHitX || (location != eHitLocations::Arm && location != eHitLocations::Chest)) {
        ret = existingParts;
    } else {
        // ignore armpit and elbow locations
        for (auto part : existingParts) {
            if (part != eBodyParts::Armpit && part != eBodyParts::Elbow) {
                ret.push_back(part);
            }
        }
    }

    return ret;
}

Wound* WoundTable::getWound(eDamageTypes type, eBodyParts part, int level)
{
    level = min(level, 5);
    Wound* ret = m_woundTable[type][part][level];
    assert(ret != nullptr);
    return ret;
}
