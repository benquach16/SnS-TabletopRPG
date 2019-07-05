#pragma once

#include "human.h"

class Player : public Human
{
public:
	Player();
	bool isPlayer() override { return true; }

	bool pollForInitiative();
	void setInitiative(eInitiativeRoll initiative);
	eInitiativeRoll getInitiative() { return m_initiative; }

	bool pollForOffense();
	bool pollForDefense();
private:
	eInitiativeRoll m_initiative;

	//wrap in fence object?
	bool m_flagInitiative;
	bool m_flagOffense;
	bool m_flagDefense;
};
