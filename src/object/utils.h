#pragma once

#include <string>

#include "types.h"

std::string factionToString(eCreatureFaction faction);
std::string rankToString(eRank rank);
std::string raceToString(eCreatureRace race);
eCreatureFaction stringToFaction(const std::string& str);
eCreatureRace stringToRace(const std::string& str);
eRank stringToRank(const std::string& str);
