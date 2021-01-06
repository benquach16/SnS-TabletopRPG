#pragma once

#include <map>
#include <string>

#include "types.h"
#include "weapon.h"

class Creature;

std::string offensiveManueverToString(eOffensiveManuevers manuever);

std::string defensiveManueverToString(eDefensiveManuevers manuever);

std::string damageTypeToString(eDamageTypes type);

std::string lengthToString(eLength length);

eLayer stringToArmorLayer(const std::string& str);

eArmorTypes stringToArmorType(const std::string& str);

std::string itemTypeToString(eItemType type);

std::string weaponTypeToString(eWeaponTypes type);

std::string weaponPropToString(eWeaponProperties prop);

eItemType stringToItemType(const std::string& str);

eGrips stringToGrip(const std::string& str);

std::string gripToString(eGrips grip);

inline int operator-(eLength length1, eLength length2)
{
    return static_cast<int>(length1) - static_cast<int>(length2);
}
inline int operator<(eLength length1, eLength length2)
{
    return static_cast<int>(length1) < static_cast<int>(length2);
}

int calculateReachCost(eLength length1, eLength length2);
int getFeintCost();
int getOffensiveManueverCost(eOffensiveManuevers manuever, eGrips grip, eLength effectiveReach,
    eLength currentReach, bool payReach);
int getDefensiveManueverCost(
    eDefensiveManuevers manuever, eGrips grip, eLength effectiveReach, eLength currentReach);

//<maneuver, cost>
std::map<eOffensiveManuevers, int> getAvailableOffManuevers(const Creature* creature,
    bool primaryWeapon, eLength currentReach, bool inGrapple, bool payReach);

//<maneuver, cost>
std::map<eDefensiveManuevers, int> getAvailableDefManuevers(const Creature* creature,
    bool primaryWeapon, bool isLastTempo, eLength currentReach, bool inGrapple);

int getQuickdrawCost(const Weapon* weapon, bool inGrapple);
