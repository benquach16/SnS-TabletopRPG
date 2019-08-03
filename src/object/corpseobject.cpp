#include "corpseobject.h"
#include "utils.h"

CorpseObject::CorpseObject(const std::string& name)
    : m_name(name)
    , m_race(eCreatureRace::Human)
{
}

std::string CorpseObject::getDescription() const
{
    std::string ret;
    ret = "the corpse of " + raceToString(m_race) + " " + m_name;
    return ret;
}
