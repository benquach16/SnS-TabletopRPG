#pragma once

#include <string>

#include "types.h"

std::string offensiveManueverToString(eOffensiveManuevers manuever);

std::string defensiveManueverToString(eDefensiveManuevers manuever);

std::string damageTypeToString(eDamageTypes type);

eLayer stringToArmorLayer(const std::string& str);
