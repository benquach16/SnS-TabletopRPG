#include <iostream>

#include "player.h"

Player::Player()
    : Human()
    , m_flagInitiative(false)
{
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
    //todo: switch this to the creature base m_hasOffense
    Creature::clearCreatureManuevers();
    m_flagInitiative = false;
}
