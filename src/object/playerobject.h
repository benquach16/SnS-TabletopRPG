#pragma once

#include "../combatinstance.h"
#include "../combatmanager.h"
#include "../creatures/player.h"
#include "creatureobject.h"

class PlayerObject : public CreatureObject {
public:
    PlayerObject();
    ~PlayerObject();
    void startCombatWith(Creature* creature);
    bool runCombat(float tick);
    CombatInstance& getCombatInstance() { return m_instance; }
    const CombatManager* getCombatManager() { return m_manager; }

    eCreatureFaction getFaction() const override { return eCreatureFaction::Player; }
    eCreatureRace getRace() const override { return eCreatureRace::Human; }
    std::string getDescription() const override { return "yourself."; }
    bool isPlayer() const override { return true; }
    void run(const Level* level) override {}

    //never delete player, otherwise there will be a segfault
    bool deleteMe() const override { return false; }
    virtual bool preserveBetweenLevels() const { return true; }

private:
    CombatInstance m_instance;
    CombatManager* m_manager;
};
