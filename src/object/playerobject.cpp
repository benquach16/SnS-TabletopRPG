#include "playerobject.h"

PlayerObject::PlayerObject() : m_player(nullptr)
{
	m_player = new Player();
	m_player->setWeapon(40);
	m_player->equipArmor(41);
	m_player->equipArmor(42);
	m_player->setName("John");
}

PlayerObject::~PlayerObject()
{
	if(m_player != nullptr) {
		delete m_player;
		m_player = nullptr;
	}
}
