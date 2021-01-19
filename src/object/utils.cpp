#include "utils.h"

std::string factionToString(eCreatureFaction faction)
{
    switch (faction) {
    case eCreatureFaction::Player:
        return "Player";
        break;
    case eCreatureFaction::Bandit:
        return "Bandit";
        break;
    case eCreatureFaction::Wildlife:
        return "Wildlife";
        break;
    case eCreatureFaction::Confederacy:
        return "Confederacy of the Three";
        break;
    case eCreatureFaction::ArenaFighter:
        return "Arena Fighter";
        break;
    default:
        return "";
        break;
    }
}

std::string rankToString(eRank rank)
{
    switch (rank) {
    case eRank::Recruit:
        return "recruit";
        break;
    case eRank::Soldier:
        return "soldier";
        break;
    case eRank::Veteran:
        return "veteran";
        break;
    case eRank::Lord:
        return "lord";
        break;
    default:
        return "";
        break;
    }
}

std::string raceToString(eCreatureRace race)
{
    switch (race) {
    case eCreatureRace::Human:
        return "Human";
        break;
    case eCreatureRace::Goblin:
        return "Goblin";
        break;
    }
    return "";
}

eCreatureFaction stringToFaction(const std::string& str)
{
    if (str == "bandit") {
        return eCreatureFaction::Bandit;
    } else if (str == "wildlife") {
        return eCreatureFaction::Wildlife;
    } else if (str == "confederacy") {
        return eCreatureFaction::Confederacy;
    }
    return eCreatureFaction::Civilian;
}

eCreatureRace stringToRace(const std::string& str)
{
    if (str == "goblin") {
        return eCreatureRace::Goblin;
    }
    return eCreatureRace::Human;
}

eRank stringToRank(const std::string& str)
{
    if (str == "recruit") {
        return eRank::Recruit;
    } else if (str == "soldier") {
        return eRank::Soldier;
    } else if (str == "veteran") {
        return eRank::Veteran;
    }
    return eRank::Lord;
}
