#include "utils.h"

std::string factionToString(eCreatureFaction faction)
{
    switch(faction) {
    case eCreatureFaction::Player:
        return "Player";
        break;
	case eCreatureFaction::Bandit:
		return "Bandit";
		break;
	case eCreatureFaction::Wildlife:
		return "Wildlife";
		break;
	}
	return "";
}

std::string raceToString(eCreatureRace race)
{
	switch(race) {
	case eCreatureRace::Human:
		return "Human";
		break;
	case eCreatureRace::Goblin:
		return "Goblin";
		break;
	}
	return "";
}
