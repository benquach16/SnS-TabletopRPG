#pragma once

#include "human.h"

#include <iostream>

class Player : public Human {
public:
    Player();
    ~Player() {}
    bool isPlayer() override { return true; }

    bool pollForInitiative();
    void setInitiative(eInitiativeRoll initiative);
    eInitiativeRoll getInitiative() { return m_initiative; }

    // players make their offence piecemeal
    void setOffenseWeapon(const Weapon* weapon) { m_currentOffense.weapon = weapon; }
    void setOffenseManuever(eOffensiveManuevers manuever) { m_currentOffense.manuever = manuever; }
    void setOffenseDice(int dice) { m_currentOffense.dice = dice; }
    void setOffenseComponent(const Component* component) { m_currentOffense.component = component; }
    void setOffenseTarget(eHitLocations location) { m_currentOffense.target = location; }
    void setOffensePinpointTarget(eBodyParts part) { m_currentOffense.pinpointTarget = part; }
    void setOffensePinpoint() { m_currentOffense.pinpoint = true; }
    void setOffenseLinked() { m_currentOffense.linked = true; }
    void setOffenseFeint() { m_currentOffense.feint = true; }
    void setOffenseHeavyDice(int dice) { m_currentOffense.heavyblow = dice; }

    void setDefenseWeapon(const Weapon* weapon) { m_currentDefense.weapon = weapon; }
    void setDefenseManuever(eDefensiveManuevers manuever) { m_currentDefense.manuever = manuever; }
    void setDefenseDice(int dice) { m_currentDefense.dice = dice; }

    void setPositionManuever(ePositionManuevers manuever) { m_currentPosition.manuever = manuever; }
    void setPositionDice(int dice) { m_currentPosition.dice = dice; }

    void setOffenseReady() { m_hasOffense = true; }
    void setDefenseReady() { m_hasDefense = true; }
    void setPositionReady() { m_hasPosition = true; }
    void addFavored(eHitLocations location) { m_favoredLocations.insert(location); }
    void setPrecombatReady() { m_hasPrecombat = true; }
    void setPreResolutionReady() { m_hasPreResolution = true; }

    void clearCreatureManuevers(bool skipDisable = false) override;

private:
    eInitiativeRoll m_initiative;

    // wrap in fence object?
    bool m_flagInitiative;
};
