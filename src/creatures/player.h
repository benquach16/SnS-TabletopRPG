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

	//players make their offence piecemeal
	void setOffenseManuever(eOffensiveManuevers manuever) { m_currentOffense.manuever = manuever; }
	void setOffenseDice(int dice) { m_currentOffense.dice = dice; }
	void setOffenseComponent(Component* component) { m_currentOffense.component = component; }
	void setOffenseTarget(eHitLocations location) { m_currentOffense.target = location; }

	void setDefenseManuever(eDefensiveManuevers manuever) { m_currentDefense.manuever = manuever; }
	void setDefenseDice(int dice) { m_currentDefense.dice = dice; }

	bool pollForOffense();
	void setOffenseReady() { m_flagOffense = true; }
	bool pollForDefense();
	void setDefenseReady() { m_flagDefense = true; }
	
private:
	eInitiativeRoll m_initiative;

	//wrap in fence object?
	bool m_flagInitiative;
	bool m_flagOffense;
	bool m_flagDefense;
};
