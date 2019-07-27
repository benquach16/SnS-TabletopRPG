#include <iostream>

#include "humanobject.h"
#include "equipmentmanager.h"
#include "utils.h"
#include "../3rdparty/random.hpp"

using namespace std;

HumanObject::HumanObject() : CreatureObject(new Human)
{
	//m_creature->setWeapon(1041); //arming sword
	int weaponId = EquipmentManager::getSingleton()->getRandomWeapon(eCreatureFaction::Bandit);
	m_creature->setWeapon(weaponId);
	std::vector<int> armor = EquipmentManager::getSingleton()->getRandomArmors(eCreatureFaction::Bandit);
	for(auto i : armor) {
		if(m_creature->canEquipArmor(i)) {
			m_creature->equipArmor(i);
		} else {
			cout << "Failed to equip " << i << endl;
		}
		m_inventory[i]++;
	}
	//m_creature->equipArmor(2041); //chainmail
	//m_creature->equipArmor(2051); //maille coif
	m_creature->setName(EquipmentManager::getSingleton()->getRandomName(eCreatureRace::Human));

	m_inventory[0] = effolkronium::random_static::get(1, 25);
	//m_inventory[1041] = 1;
	m_inventory[weaponId] = 1;
	//m_inventory[2041] = 1;
	//m_inventory[2051] = 1;
	
	m_creatureFaction = eCreatureFaction::Bandit;
}

HumanObject::~HumanObject()
{
}

std::string HumanObject::getDescription() const
{
	std::string ret;
	ret += m_creature->getName() + ", human " + factionToString(m_creatureFaction) + " armed with " +
		m_creature->getPrimaryWeapon()->getName();

	return ret;
}
