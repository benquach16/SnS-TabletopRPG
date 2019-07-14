#pragma once

#include "object.h"
#include "../creatures/player.h"

class PlayerObject : public Object
{
public:
	PlayerObject();
	~PlayerObject();
	Player* getPlayerComponent() { return m_player; };
private:
	Player* m_player;
};
