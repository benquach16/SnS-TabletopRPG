#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include "../creatures/types.h"
#include "item.h"
#include "types.h"

struct ArmorSegment {
    int AV = 0;
    bool isMetal = false;
    bool isRigid = false;
    eArmorTypes type = eArmorTypes::None;
};

class Armor : public Item {
public:
    Armor(const std::string& name, const std::string& description, int AV, float AP, eLayer layer,
        eArmorTypes type, bool rigid, bool metal, std::set<eBodyParts> coverage, int cost);
    int getAV() const { return m_AV; }
    float getAP() const { return m_AP; }

    bool isRigid() const { return m_rigid; }
    bool isMetal() const { return m_metal; }

    std::set<eBodyParts> getCoverage() const { return m_coverage; }

    eItemType getItemType() const override { return eItemType::Armor; }

    eArmorTypes getType() const { return m_type; }
    eLayer getLayer() const { return m_layer; }

    bool isOverlapping(const Armor* armor) const;

private:
    Armor();
    int m_AV;
    float m_AP;
    bool m_rigid;
    bool m_metal;

    std::set<eBodyParts> m_coverage;

    eLayer m_layer;
    eArmorTypes m_type;
};

class ArmorTable {
public:
    static ArmorTable* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new ArmorTable;
        }
        return singleton;
    }
    const Armor* get(int id) { return m_armorList[id]; }

private:
    ArmorTable();
    static ArmorTable* singleton;
    //<id, armor>
    std::unordered_map<int, Armor*> m_armorList;
};
