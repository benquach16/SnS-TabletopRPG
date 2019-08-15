#pragma once

#include <map>

#include "../components/aicontroller.h"
#include "../creatures/creature.h"
#include "object.h"
#include "types.h"

class Level;
class CombatManager;

class CreatureObject : public Object {
public:
    CreatureObject(Creature* creature);
    virtual ~CreatureObject();
    bool hasCollision() const override { return true; }
    bool deleteMe() const override { return m_delete; }
    bool isConscious() const
    {
        return (!deleteMe() && m_creature->getCreatureState() != eCreatureState::Unconscious);
    }
    Creature* getCreatureComponent() const { return m_creature; }
    const std::string getName() const { return m_creature->getName(); }
    virtual eCreatureFaction getFaction() const { return m_creatureFaction; }
    virtual eCreatureRace getRace() const = 0;
    eObjectTypes getObjectType() const override { return eObjectTypes::Creature; }
    virtual bool isPlayer() const { return false; }
    void kill() const { m_creature->kill(); }
    void run(const Level*) override;

    void addItem(int id, int count) { m_inventory[id] = count; }
    void addItem(int id) { m_inventory[id]++; }
    void removeItem(int id) { m_inventory[id]--; }

    void applyItem(int id);

    int getThirst() const { return m_thirst; }
    int getHunger() const { return m_hunger; }
    int getExhaustion() const { return m_exhaustion; }
    int getFatigue() const;

    bool getBleeding() const { return m_creature->getBleeding(); }

    std::string getStartingDialogueLabel() const { return "greeting_hostile"; }

    bool isInCombat() const;
    void setInCombat() { m_inCombat = true; }
    void setOutofCombat() { m_inCombat = false; }

    void startCombatWith(const CreatureObject* creature);
    CombatManager* getCombatManager() const { return m_manager; }

protected:
    Creature* m_creature;
    eCreatureFaction m_creatureFaction;
    eCreatureRace m_creatureRace;

    AIController m_controller;

    int m_money;

    int m_thirst;
    int m_hunger;
    int m_exhaustion;

    CombatManager* m_manager;

    bool m_inCombat;
    bool m_delete;
};
