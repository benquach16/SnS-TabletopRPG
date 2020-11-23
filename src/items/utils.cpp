#include <assert.h>

#include "utils.h"

std::string offensiveManueverToString(eOffensiveManuevers manuever)
{
    switch (manuever) {
    case eOffensiveManuevers::Swing:
        return "Swing";
    case eOffensiveManuevers::Thrust:
        return "Thrust";
    case eOffensiveManuevers::PinpointThrust:
        return "Pinpoint Thrust";
    case eOffensiveManuevers::Beat:
        return "Beat";
    case eOffensiveManuevers::Hook:
        return "Hook";
    case eOffensiveManuevers::Mordhau:
        return "Mordhau";
    case eOffensiveManuevers::Slam:
        return "Slam";
    case eOffensiveManuevers::Disarm:
        return "Disarm";
    case eOffensiveManuevers::Grab:
        return "Grab";
    case eOffensiveManuevers::Snap:
        return "Break Limb";
    case eOffensiveManuevers::Throw:
        return "Throw";
    default:
        return "";
    }
}

std::string defensiveManueverToString(eDefensiveManuevers manuever)
{
    switch (manuever) {
    case eDefensiveManuevers::Parry:
        return "Parry";
    case eDefensiveManuevers::Dodge:
        return "Dodge";
    case eDefensiveManuevers::Counter:
        return "Riposte";
    case eDefensiveManuevers::ParryLinked:
        return "Block and Strike";
    case eDefensiveManuevers::StealInitiative:
        return "Steal Initiative";
    case eDefensiveManuevers::AttackFromDef:
        return "Attack from Defense";
    case eDefensiveManuevers::Expulsion:
        return "Expulsion";
    default:
        return "";
    }
}

std::string damageTypeToString(eDamageTypes type)
{
    switch (type) {
    case eDamageTypes::Blunt:
        return "Blunt";
    case eDamageTypes::Piercing:
        return "Piercing";
    case eDamageTypes::Cutting:
        return "Cutting";
    }
    return "";
}

std::string lengthToString(eLength length)
{
    switch (length) {
    case eLength::Nonexistent:
        return "Nonexistent";
        break;
    case eLength::Hand:
        return "Hand";
        break;
    case eLength::Close:
        return "Close";
        break;
    case eLength::Short:
        return "Short";
        break;
    case eLength::Medium:
        return "Medium";
        break;
    case eLength::Long:
        return "Long";
        break;
    case eLength::VeryLong:
        return "Very Long";
        break;
    case eLength::Extended:
        return "Extended";
        break;
    case eLength::Ludicrous:
        return "Ludicrous";
        break;
    default:
        return "";
    }
}

eLayer stringToArmorLayer(const std::string& str)
{
    if (str == "surcoat") {
        return eLayer::Surcoat;
    }
    if (str == "plate") {
        return eLayer::Plate;
    }
    if (str == "maille") {
        return eLayer::Maille;
    }

    return eLayer::Cloth;
}

eArmorTypes stringToArmorType(const std::string& str)
{
    if (str == "plate") {
        return eArmorTypes::Plate;
    } else if (str == "maille") {
        return eArmorTypes::Maille;
    }
    return eArmorTypes::None;
}

std::string itemTypeToString(eItemType type)
{
    switch (type) {
    case eItemType::Item:
        return "Item";
    case eItemType::Armor:
        return "Armor";
    case eItemType::Weapon:
        return "Weapon";
    case eItemType::Food:
        return "Food";
    case eItemType::Waterskin:
        return "Waterskin";
    case eItemType::Money:
        return "Money";
    case eItemType::Bandage:
        return "Bandage";
    case eItemType::Bedroll:
        return "Bedroll";
    case eItemType::Firstaid:
        return "First Aid";
    default:
        return "";
    }
}

std::string weaponTypeToString(eWeaponTypes type)
{
    switch (type) {
    case eWeaponTypes::Brawling:
        return "Brawling";
    case eWeaponTypes::Swords:
        return "Swords";
    case eWeaponTypes::Longswords:
        return "Longswords";
    case eWeaponTypes::Polearms:
        return "Polearms";
    case eWeaponTypes::Mass:
        return "Mass Weapons";
    case eWeaponTypes::Bows:
        return "Bows";
    case eWeaponTypes::Crossbows:
        return "Crossbows";
    case eWeaponTypes::Firearms:
        return "Firearms";
    default:
        return "";
    }
}

std::string weaponPropToString(eWeaponProperties prop)
{
    switch (prop) {
    case eWeaponProperties::Crushing:
        return "Crushing";
    case eWeaponProperties::MaillePiercing:
        return "Maille Piercing";
    case eWeaponProperties::PlatePiercing:
        return "Plate Piercing";
    case eWeaponProperties::Defensive:
        return "Defensive";
    default:
        return "";
    }
}

eItemType stringToItemType(const std::string& str)
{
    if (str == "item") {
        return eItemType::Item;
    } else if (str == "food") {
        return eItemType::Food;
    } else if (str == "waterskin") {
        return eItemType::Waterskin;
    } else if (str == "bedroll") {
        return eItemType::Bedroll;
    } else if (str == "armor") {
        return eItemType::Armor;
    } else if (str == "weapon") {
        return eItemType::Weapon;
    } else if (str == "money") {
        return eItemType::Money;
    } else if (str == "bandage") {
        return eItemType::Bandage;
    } else if (str == "firstaid") {
        return eItemType::Firstaid;
    }
    assert(true);
    return eItemType::Item;
}

eGrips stringToGrip(const std::string& str)
{
    if (str == "staff") {
        return eGrips::Staff;
    } else if (str == "overhand") {
        return eGrips::Overhand;
    } else if (str == "halfsword") {
        return eGrips::HalfSword;
    }
    return eGrips::Standard;
}

std::string gripToString(eGrips grip)
{
    switch (grip) {
    case eGrips::Standard:
        return "Standard";
    case eGrips::HalfSword:
        return "Half-Sword";
    case eGrips::Staff:
        return "Staff";
    case eGrips::Overhand:
        return "Reverse";
    default:
        return "";
    }
}

int calculateReachCost(eLength length1, eLength length2)
{
    int reachCost = length1 - length2;
    reachCost = abs(reachCost);
    // reachCost = (reachCost + 1) / 2;
    return reachCost;
}

int getFeintCost() { return 2; }

int getOffensiveManueverCost(
    eOffensiveManuevers manuever, eGrips grip, const Weapon* weapon, eLength currentReach)
{
    switch (manuever) {
    case eOffensiveManuevers::Disarm:
    case eOffensiveManuevers::Hook:
        return 1;
    case eOffensiveManuevers::PinpointThrust: {
        int cost = 2;
        if (weapon->getLength() > eLength::Medium) {
            cost++;
        }
        return cost;
    }
    case eOffensiveManuevers::Mordhau:
        if (grip == eGrips::HalfSword) {
            return 0;
        }
        return 2;
    case eOffensiveManuevers::Grab:
        return std::max(static_cast<int>(currentReach - eLength::Hand), 0);
    }

    return 0;
}

int getDefensiveManueverCost(eDefensiveManuevers manuever, eGrips grip)
{
    switch (manuever) {
    case eDefensiveManuevers::Expulsion:
    case eDefensiveManuevers::ParryLinked:
    case eDefensiveManuevers::Counter:
        return 2;
    default:
        return 0;
    }
    return 0;
}

std::map<eOffensiveManuevers, int> getAvailableOffManuevers(
    const Weapon* weapon, eGrips grip, eLength currentReach)
{
    std::map<eOffensiveManuevers, int> ret;

    ret[eOffensiveManuevers::Swing]
        = getOffensiveManueverCost(eOffensiveManuevers::Swing, grip, weapon, currentReach);
    ret[eOffensiveManuevers::Thrust]
        = getOffensiveManueverCost(eOffensiveManuevers::Thrust, grip, weapon, currentReach);
    ret[eOffensiveManuevers::Beat]
        = getOffensiveManueverCost(eOffensiveManuevers::Beat, grip, weapon, currentReach);
    ret[eOffensiveManuevers::PinpointThrust]
        = getOffensiveManueverCost(eOffensiveManuevers::PinpointThrust, grip, weapon, currentReach);
    ret[eOffensiveManuevers::Grab]
        = getOffensiveManueverCost(eOffensiveManuevers::Grab, grip, weapon, currentReach);
    eWeaponTypes type = weapon->getType();
    if (type == eWeaponTypes::Swords || type == eWeaponTypes::Longswords) {
        ret[eOffensiveManuevers::Mordhau] = (grip == eGrips::HalfSword) ? 0 : 2;
    }

    if (weapon->canHook()) {
        ret[eOffensiveManuevers::Hook]
            = getOffensiveManueverCost(eOffensiveManuevers::Hook, grip, weapon, currentReach);
    }

    return ret;
}

std::map<eDefensiveManuevers, int> getAvailableDefManuevers(
    const Weapon* weapon, eGrips grip, bool isLastTempo)
{
    std::map<eDefensiveManuevers, int> ret;

    ret[eDefensiveManuevers::Dodge] = getDefensiveManueverCost(eDefensiveManuevers::Dodge, grip);
    ret[eDefensiveManuevers::Parry] = getDefensiveManueverCost(eDefensiveManuevers::Parry, grip);
    ret[eDefensiveManuevers::ParryLinked]
        = getDefensiveManueverCost(eDefensiveManuevers::ParryLinked, grip);
    ret[eDefensiveManuevers::Counter]
        = getDefensiveManueverCost(eDefensiveManuevers::Counter, grip);
    ret[eDefensiveManuevers::Expulsion]
        = getDefensiveManueverCost(eDefensiveManuevers::Expulsion, grip);
    if (isLastTempo == false) {
        ret[eDefensiveManuevers::StealInitiative]
            = getDefensiveManueverCost(eDefensiveManuevers::StealInitiative, grip);
    }
    ret[eDefensiveManuevers::AttackFromDef]
        = getDefensiveManueverCost(eDefensiveManuevers::AttackFromDef, grip);

    return ret;
}
