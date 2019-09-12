#include <iostream>

#include "player.h"

Player::Player()
    : Human()
    , m_flagInitiative(false)
{
    m_brawn = 1;
    m_agility = 1;
    m_cunning = 1;
    m_will = 1;
    m_perception = 1;

    m_proficiencies[eWeaponTypes::Polearms] = 1;
    m_proficiencies[eWeaponTypes::Swords] = 1;
    m_proficiencies[eWeaponTypes::Longswords] = 1;
    m_proficiencies[eWeaponTypes::Brawling] = 1;
    m_proficiencies[eWeaponTypes::Mass] = 1;
}

bool Player::pollForInitiative()
{
    bool ret = m_flagInitiative;
    if (ret == true) {
        m_flagInitiative = false;
    }
    return ret;
}

void Player::setInitiative(eInitiativeRoll initiative)
{
    m_initiative = initiative;
    m_flagInitiative = true;
}

void Player::clearCreatureManuevers()
{
    // todo: switch this to the creature base m_hasOffense
    Creature::clearCreatureManuevers();
    m_flagInitiative = false;
}
