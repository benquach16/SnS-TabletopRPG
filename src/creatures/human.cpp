#include "human.h"

Human::Human() : Creature()
{
	m_hitLocations.push_back(eHitLocations::Head);
	m_hitLocations.push_back(eHitLocations::Arm);
	m_hitLocations.push_back(eHitLocations::Chest);
	m_hitLocations.push_back(eHitLocations::Belly);
	m_hitLocations.push_back(eHitLocations::Thigh);
	m_hitLocations.push_back(eHitLocations::Shin);

	m_proficiencies[eWeaponTypes::Polearms] = 5;
	m_proficiencies[eWeaponTypes::Swords] = 5;
}




