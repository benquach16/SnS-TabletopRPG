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

    m_proficiencies[eWeaponTypes::Polearms] = 5;
    m_proficiencies[eWeaponTypes::Swords] = 5;
    m_proficiencies[eWeaponTypes::Longswords] = 5;
    m_proficiencies[eWeaponTypes::Brawling] = 2;

    m_brawn = effolkronium::random_static::get(2, 7);
    m_agility = effolkronium::random_static::get(2, 7);
    m_cunning = effolkronium::random_static::get(2, 7);
    m_perception = effolkronium::random_static::get(2, 7);
    m_will = effolkronium::random_static::get(2, 7);
}
