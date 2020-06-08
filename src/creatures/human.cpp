#include "human.h"
#include "3rdparty/random.hpp"

Human::Human()
    : Creature()
{
    m_hitLocations.push_back(eHitLocations::Head);
    m_hitLocations.push_back(eHitLocations::Arm);
    m_hitLocations.push_back(eHitLocations::Chest);
    m_hitLocations.push_back(eHitLocations::Belly);
    m_hitLocations.push_back(eHitLocations::Thigh);
    m_hitLocations.push_back(eHitLocations::Shin);

    randomizeStats();
}

void Human::randomizeStats()
{
    m_proficiencies[eWeaponTypes::Polearms] = 5;
    m_proficiencies[eWeaponTypes::Swords] = 5;
    m_proficiencies[eWeaponTypes::Longswords] = 5;
    m_proficiencies[eWeaponTypes::Brawling] = 2;

    m_strength = effolkronium::random_static::get(2, 7);
    m_agility = effolkronium::random_static::get(2, 7);
    m_intuition = effolkronium::random_static::get(2, 7);
    m_perception = effolkronium::random_static::get(2, 7);
    m_willpower = effolkronium::random_static::get(2, 7);
}
