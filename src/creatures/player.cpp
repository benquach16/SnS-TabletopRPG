#include <iostream>

#include "player.h"

BOOST_CLASS_EXPORT(Player)

Player::Player()
    : Human()
{
    m_strength = 1;
    m_agility = 1;
    m_intuition = 1;
    m_willpower = 1;
    m_perception = 1;

    m_proficiencies[eWeaponTypes::Polearms] = 1;
    m_proficiencies[eWeaponTypes::Swords] = 1;
    m_proficiencies[eWeaponTypes::Longswords] = 1;
    m_proficiencies[eWeaponTypes::Brawling] = 1;
    m_proficiencies[eWeaponTypes::Mass] = 1;
}
