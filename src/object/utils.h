#pragma once

#include <string>

#include "types.h"

std::string factionToString(eCreatureFaction faction);
std::string raceToString(eCreatureRace race);
eCreatureFaction stringToFaction(const std::string &str);
eCreatureRace stringToRace(const std::string& str);
