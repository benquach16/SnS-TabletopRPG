#include "human.h"
#include "3rdparty/random.hpp"

constexpr int naturalWeaponId = cFistsId;

Human::Human()
    : Creature(naturalWeaponId)
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
    m_proficiencies[eWeaponTypes::Polearms] = effolkronium::random_static::get(8, 14);
    m_proficiencies[eWeaponTypes::Swords] = effolkronium::random_static::get(8, 14);
    m_proficiencies[eWeaponTypes::Longswords] = effolkronium::random_static::get(8, 14);
    m_proficiencies[eWeaponTypes::Brawling] = effolkronium::random_static::get(8, 14);

    m_strength = effolkronium::random_static::get(2, 9);
    m_agility = effolkronium::random_static::get(2, 9);
    m_intuition = effolkronium::random_static::get(2, 9);
    m_perception = effolkronium::random_static::get(2, 9);
    m_willpower = effolkronium::random_static::get(2, 9);
}
