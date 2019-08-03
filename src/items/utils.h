#pragma once

#include <string>

#include "types.h"

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
