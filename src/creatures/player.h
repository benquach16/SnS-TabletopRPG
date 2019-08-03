#pragma once

#include "human.h"

#include <iostream>

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
	void setOffenseLinked() { m_currentOffense.linked = true; }

	void setDefenseManuever(eDefensiveManuevers manuever) { m_currentDefense.manuever = manuever; }
	void setDefenseDice(int dice) { m_currentDefense.dice = dice; }

	void setPositionDice(int dice) { m_currentPosition.dice = dice; }

	void setOffenseReady() { m_hasOffense = true; }
	void setDefenseReady() { m_hasDefense = true; }
	void setPositionReady() { m_hasPosition = true; }

	void setDefenseOff() { m_hasDefense = false; }

	void clearCreatureManuevers() override;
	
private:
	eInitiativeRoll m_initiative;

	//wrap in fence object?
	bool m_flagInitiative;
};
