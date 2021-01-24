#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "types.h"

class Creature;

std::string bodyPartToString(eBodyParts part);

eBodyParts stringToBodyPart(const std::string& str);

std::string hitLocationToString(eHitLocations location);

std::string stanceToString(eCreatureStance stance);

bool isLimb(eHitLocations location);

eHitLocations getHitLocation(eBodyParts part);

eCombatGuard getLocationGuard(eHitLocations location);
