#pragma once

#include <map>

#include "../components/aicontroller.h"
#include "../creatures/creature.h"
#include "object.h"
#include "types.h"

class Level;

class CreatureObject : public Object {
public:
    CreatureObject(Creature* creature);
    virtual ~CreatureObject();
    bool hasCollision() const override { return true; }
    bool deleteMe() const override { return m_creature->getCreatureState() == eCreatureState::Dead; }
    bool isConscious() const { return (!deleteMe() && m_creature->getCreatureState() != eCreatureState::Unconscious); }
    Creature* getCreatureComponent() const { return m_creature; }
    const std::string getName() const { return m_creature->getName(); }
    virtual eCreatureFaction getFaction() const { return m_creatureFaction; }
    virtual eCreatureRace getRace() const = 0;
    virtual eObjectTypes getObjectType() const { return eObjectTypes::Creature; }
    virtual bool isPlayer() const { return false; }
    bool isInCombat() const { return m_creature->getCreatureState() == eCreatureState::InCombat; }
    void kill() const { m_creature->kill(); }
    void run(const Level*) override;

    void addItem(int id, int count) { m_inventory[id] = count; }
    void addItem(int id) { m_inventory[id]++; }
    void removeItem(int id) { m_inventory[id]--; }

    void applyItem(int id);

    int getCombatManagerId() const { return m_combatManagerId; }

protected:
    Creature* m_creature;
    eCreatureFaction m_creatureFaction;
    eCreatureRace m_creatureRace;

    AIController m_controller;

    int m_money;

    int m_thirst;
    int m_hunger;
    int m_exhaustion;

    int m_combatManagerId;
};
