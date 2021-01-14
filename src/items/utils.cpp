#include "utils.h"
#include "../creatures/creature.h"
#include <algorithm>
#include <assert.h>

std::string offensiveManueverToString(eOffensiveManuevers manuever)
{
    switch (manuever) {
    case eOffensiveManuevers::NoOffense:
        return "Do Nothing";
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
    case eOffensiveManuevers::VisorThrust:
        return "Visor Thrust";
    case eOffensiveManuevers::Bite:
        return "Bite";
    case eOffensiveManuevers::Draw:
        return "Draw Cut";
    case eOffensiveManuevers::HeavyBlow:
        return "Heavy Blow";
    case eOffensiveManuevers::Gouge:
        return "Gouge";
    case eOffensiveManuevers::Strangle:
        return "Strangle";
    default:
        assert(false);
        return "";
    }
}

std::string defensiveManueverToString(eDefensiveManuevers manuever)
{
    switch (manuever) {
    case eDefensiveManuevers::NoDefense:
        return "Do Nothing";
    case eDefensiveManuevers::Parry:
        return "Parry";
    case eDefensiveManuevers::Dodge:
        return "Dodge";
    case eDefensiveManuevers::Counter:
        return "Riposte";
    case eDefensiveManuevers::ParryLinked:
        return "Masterstrike";
    case eDefensiveManuevers::StealInitiative:
        return "Steal Initiative";
    case eDefensiveManuevers::AttackFromDef:
        return "Attack from Defense";
    case eDefensiveManuevers::Expulsion:
        return "Expulsion";
    case eDefensiveManuevers::Resist:
        return "Resist";
    case eDefensiveManuevers::Reverse:
        return "Reverse";
    case eDefensiveManuevers::Break:
        return "Break";
    default:
        assert(false);
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

std::string tnToString(int tn)
{
    switch (tn) {
    case 9:
        return "Nonexistent";
    case 8:
        return "Lumbering";
    case 7:
        return "Slow";
    case 6:
        return "Average";
    case 5:
        return "Nimble";
    case 4:
        return "Fast";
    }
    return "";
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

int getGripChangeCost(bool isLastTempo)
{
    if (isLastTempo) {
        return 1;
    }
    return 0;
}

int getDisadvantagedTN(int tn) { return std::min(cMaxBTN, tn + 1); }

int getAdvantagedTN(int tn) { return std::max(cMinBTN, tn - 1); }

int getFeintCost() { return 2; }

int getOffensiveManueverCost(eOffensiveManuevers manuever, eGrips grip, eLength effectiveReach,
    eLength currentReach, bool payReach)
{
    int reachCost = calculateReachCost(effectiveReach, currentReach);
    if (payReach == false) {
        reachCost = 0;
    }
    switch (manuever) {
    case eOffensiveManuevers::HeavyBlow: {
        int cost = 2 + reachCost;
        if (grip == eGrips::Staff) {
            // cost--;
        }
        return cost;
    }

    case eOffensiveManuevers::Disarm:
    case eOffensiveManuevers::Hook:
        return 1 + reachCost;
    case eOffensiveManuevers::PinpointThrust: {
        int cost = 2;
        if (effectiveReach > eLength::Medium) {
            cost++;
        }
        if (currentReach == eLength::Hand) {
            cost = 0;
        }
        return cost + reachCost;
    }
    case eOffensiveManuevers::Mordhau: {
        int cost = 0;
        if (grip != eGrips::HalfSword) {
            cost = 2;
        }
        return cost + reachCost;
    }
    case eOffensiveManuevers::Beat: {
        int cost = (reachCost) / 2;
        if (reachCost > 0 && cost == 0) {
            cost = 1;
        }
        return cost;
    }
    case eOffensiveManuevers::Grab:
        return calculateReachCost(eLength::Hand, currentReach);
    case eOffensiveManuevers::VisorThrust:
    case eOffensiveManuevers::Throw:
        return 2;
    case eOffensiveManuevers::Snap:
        return 0;
    case eOffensiveManuevers::Draw: {
        // draw removes any reach disadvantage if target is too close
        if (effectiveReach > currentReach) {
            return 0;
        }
        return reachCost;
    }
    }

    return reachCost;
}

std::map<eHitLocations, int> getHitLocationCost(const Creature* target)
{
    std::map<eHitLocations, int> ret;

    for (auto location : target->getHitLocations()) {
        switch (location) {
        case eHitLocations::Arm:
        case eHitLocations::Thigh:
            ret[location] = 1;
        case eHitLocations::Tail:
        case eHitLocations::Shin:
            ret[location] = 2;
        default:
            ret[location] = 0;
        }
    }

    return ret;
}

int getDefensiveManueverCost(
    eDefensiveManuevers manuever, eGrips grip, eLength effectiveReach, eLength currentReach)
{
    int reachCost = 0;
    // defense costs only apply if your weapon is too long, not the other way around
    if (effectiveReach > currentReach) {
        reachCost = calculateReachCost(effectiveReach, currentReach);
    }
    switch (manuever) {
    case eDefensiveManuevers::StealInitiative:
        return 1;
    case eDefensiveManuevers::AttackFromDef:
    case eDefensiveManuevers::Dodge:
        return 0;
    case eDefensiveManuevers::DodgeTakeInitiative:
        return 2;
    case eDefensiveManuevers::Expulsion:
    case eDefensiveManuevers::ParryLinked:
    case eDefensiveManuevers::Counter:
    case eDefensiveManuevers::Reverse:
    case eDefensiveManuevers::Break:
        return 2 + reachCost;
    default:
        return reachCost;
    }
    return reachCost;
}

std::map<eOffensiveManuevers, int> getAvailableOffManuevers(const Creature* creature,
    bool primaryWeapon, eLength currentReach, bool inGrapple, bool payReach, bool feint)
{
    const Weapon* weapon
        = primaryWeapon ? creature->getPrimaryWeapon() : creature->getSecondaryWeapon();

    eLength effectiveReach
        = primaryWeapon ? creature->getCurrentReach() : creature->getSecondaryWeaponReach();
    eGrips grip = creature->getGrip();
    std::map<eOffensiveManuevers, int> ret;

	// theorectically already paid for reach
    if (feint) {
        payReach = false;
    }

    ret[eOffensiveManuevers::Swing] = getOffensiveManueverCost(
        eOffensiveManuevers::Swing, grip, effectiveReach, currentReach, payReach);
    ret[eOffensiveManuevers::Thrust] = getOffensiveManueverCost(
        eOffensiveManuevers::Thrust, grip, effectiveReach, currentReach, payReach);

	// only can swing or thrust in a feint
    if (feint) {
        return ret;
    }

    ret[eOffensiveManuevers::PinpointThrust] = getOffensiveManueverCost(
        eOffensiveManuevers::PinpointThrust, grip, effectiveReach, currentReach, payReach);

    if (weapon->getType() == eWeaponTypes::Longswords
        || weapon->getType() == eWeaponTypes::Swords) {
        ret[eOffensiveManuevers::Draw] = getOffensiveManueverCost(
            eOffensiveManuevers::Draw, grip, effectiveReach, currentReach, payReach);
    }

    if (inGrapple == false) {
        if (weapon->getNaturalWeapon() == false) {
            ret[eOffensiveManuevers::Beat] = getOffensiveManueverCost(
                eOffensiveManuevers::Beat, grip, effectiveReach, currentReach, payReach);
        }

        ret[eOffensiveManuevers::Grab] = getOffensiveManueverCost(
            eOffensiveManuevers::Grab, grip, effectiveReach, currentReach, payReach);

        ret[eOffensiveManuevers::Disarm] = getOffensiveManueverCost(
            eOffensiveManuevers::Disarm, grip, effectiveReach, currentReach, payReach);
        eWeaponTypes type = weapon->getType();
        // don't need to do mordhau if doing a pommel strike
        if ((type == eWeaponTypes::Swords || type == eWeaponTypes::Longswords)
            && weapon->isSecondary() == false) {
            ret[eOffensiveManuevers::Mordhau] = getOffensiveManueverCost(
                eOffensiveManuevers::Mordhau, grip, effectiveReach, currentReach, payReach);
        }
        if (type == eWeaponTypes::Mass || type == eWeaponTypes::Polearms) {
            ret[eOffensiveManuevers::HeavyBlow] = getOffensiveManueverCost(
                eOffensiveManuevers::HeavyBlow, grip, effectiveReach, currentReach, payReach);
        }
        if (weapon->canHook() || grip == eGrips::Staff || grip == eGrips::HalfSword) {
            ret[eOffensiveManuevers::Hook] = getOffensiveManueverCost(
                eOffensiveManuevers::Hook, grip, effectiveReach, currentReach, payReach);
        }
    } else {
        ret[eOffensiveManuevers::Throw] = getOffensiveManueverCost(
            eOffensiveManuevers::Throw, grip, effectiveReach, currentReach, payReach);
        ret[eOffensiveManuevers::Snap] = getOffensiveManueverCost(
            eOffensiveManuevers::Snap, grip, effectiveReach, currentReach, payReach);
        ret[eOffensiveManuevers::VisorThrust] = getOffensiveManueverCost(
            eOffensiveManuevers::VisorThrust, grip, effectiveReach, currentReach, payReach);
    }
    ret[eOffensiveManuevers::NoOffense] = 0;
    return ret;
}

std::map<eDefensiveManuevers, int> getAvailableDefManuevers(const Creature* creature,
    bool primaryWeapon, bool isLastTempo, eLength currentReach, bool inGrapple)
{
    const Weapon* weapon
        = primaryWeapon ? creature->getPrimaryWeapon() : creature->getSecondaryWeapon();

    eLength effectiveReach
        = primaryWeapon ? creature->getCurrentReach() : creature->getSecondaryWeaponReach();
    eGrips grip = creature->getGrip();
    std::map<eDefensiveManuevers, int> ret;

    if (inGrapple == false) {
        ret[eDefensiveManuevers::Dodge] = getDefensiveManueverCost(
            eDefensiveManuevers::Dodge, grip, effectiveReach, currentReach);
        ret[eDefensiveManuevers::Parry] = getDefensiveManueverCost(
            eDefensiveManuevers::Parry, grip, effectiveReach, currentReach);

        ret[eDefensiveManuevers::Counter] = getDefensiveManueverCost(
            eDefensiveManuevers::Counter, grip, effectiveReach, currentReach);
        if (weapon->getNaturalWeapon() == false) {
            ret[eDefensiveManuevers::Expulsion] = getDefensiveManueverCost(
                eDefensiveManuevers::Expulsion, grip, effectiveReach, currentReach);

            // can't single tempo counter with a secondary part of a weapon (queue, pommel)
            if (weapon->isSecondary() == false) {
                ret[eDefensiveManuevers::ParryLinked] = getDefensiveManueverCost(
                    eDefensiveManuevers::ParryLinked, grip, effectiveReach, currentReach);
            }
        }

    } else {
        ret[eDefensiveManuevers::Reverse] = getDefensiveManueverCost(
            eDefensiveManuevers::Reverse, grip, effectiveReach, currentReach);
        ret[eDefensiveManuevers::Resist] = getDefensiveManueverCost(
            eDefensiveManuevers::Resist, grip, effectiveReach, currentReach);
        ret[eDefensiveManuevers::Break] = getDefensiveManueverCost(
            eDefensiveManuevers::Break, grip, effectiveReach, currentReach);
    }
    if (isLastTempo == false) {
        ret[eDefensiveManuevers::StealInitiative] = getDefensiveManueverCost(
            eDefensiveManuevers::StealInitiative, grip, effectiveReach, currentReach);
    }
    ret[eDefensiveManuevers::AttackFromDef] = getDefensiveManueverCost(
        eDefensiveManuevers::AttackFromDef, grip, effectiveReach, currentReach);

    return ret;
}

int getQuickdrawCost(const Weapon* weapon, bool inGrapple)
{
    int cost = 1;
    cost += calculateReachCost(weapon->getLength(), eLength::Hand) / 2;
    return cost;
}
