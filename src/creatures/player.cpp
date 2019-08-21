#include <iostream>

#include "player.h"

Player::Player()
    : Human()
    , m_flagInitiative(false)
{
    m_brawn = 7;
    m_agility = 7;
    m_cunning = 7;
    m_will = 5;
    m_perception = 6;
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
