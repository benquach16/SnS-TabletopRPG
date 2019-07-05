#include "player.h"

Player::Player() : Human(), m_flagInitiative(false), m_flagOffense(false), m_flagDefense(false)
{
}

bool Player::pollForInitiative()
{
	bool ret = m_flagInitiative;
	if(ret == true) {
		m_flagInitiative = false;
	}
	return ret;
}

void Player::setInitiative(eInitiativeRoll initiative)
{
	m_initiative = initiative;
	m_flagInitiative = true;
}

bool Player::pollForOffense()
{
	bool ret = m_flagOffense;
	if(ret == true) {
		m_flagOffense = false;
	}
	return ret;
}
