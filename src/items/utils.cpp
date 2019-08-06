#include <assert.h>

#include "utils.h"

std::string offensiveManueverToString(eOffensiveManuevers manuever)
{
    switch (manuever) {
    case eOffensiveManuevers::Swing:
        return "Swing";
    case eOffensiveManuevers::Thrust:
        return "Thrust";
    case eOffensiveManuevers::FeintSwing:
        return "Swing";
    case eOffensiveManuevers::FeintThrust:
        return "Thrust";
    case eOffensiveManuevers::PinpointThrust:
        return "Pinpoint Thrust";
    }
    return "";
}

std::string defensiveManueverToString(eDefensiveManuevers manuever)
{
    switch (manuever) {
    case eDefensiveManuevers::Parry:
        return "Parry";
    case eDefensiveManuevers::Dodge:
        return "Dodge";
    }
    return "";
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
    }
    return "";
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
        break;
    case eItemType::Armor:
        return "Armor";
        break;
    case eItemType::Weapon:
        return "Weapon";
        break;
    case eItemType::Food:
        return "Food";
        break;
    case eItemType::Waterskin:
        return "Waterskin";
        break;
    case eItemType::Money:
        return "Money";
        break;
    case eItemType::Bandage:
        return "Bandage";
        break;
    }
    return "";
}

std::string weaponTypeToString(eWeaponTypes type)
{
    switch (type) {
    case eWeaponTypes::Brawling:
        return "Brawling";
        break;
    case eWeaponTypes::Swords:
        return "Swords";
        break;
    case eWeaponTypes::Longswords:
        return "Longswords";
        break;
    case eWeaponTypes::Polearms:
        return "Polearms";
        break;
    case eWeaponTypes::Mass:
        return "Mass Weapons";
        break;
    case eWeaponTypes::Bows:
        return "Bows";
        break;
    case eWeaponTypes::Crossbows:
        return "Crossbows";
        break;
    case eWeaponTypes::Firearms:
        return "Firearms";
        break;
    }

    return "";
}

std::string weaponPropToString(eWeaponProperties prop)
{
    switch (prop) {
    case eWeaponProperties::Crushing:
        return "Crushing";
        break;
    case eWeaponProperties::MaillePiercing:
        return "Maille Piercing";
        break;
    case eWeaponProperties::PlatePiercing:
        return "Plate Piercing";
        break;
    }
    return "";
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
    }
    assert(true);
}
