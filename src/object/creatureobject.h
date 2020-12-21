#pragma once

#include <iostream>
#include <map>
#include <unordered_map>

#include "../components/aicombatcontroller.h"
#include "../components/aicontroller.h"
#include "../creatures/creature.h"
#include "object.h"
#include "types.h"

#include <boost/serialization/strong_typedef.hpp>

class Level;
class CombatManager;

class CreatureObject : public Object {
public:
    friend class boost::serialization::access;

    BOOST_STRONG_TYPEDEF(unsigned, CreatureObjectId);

    CreatureObject(Creature* creature);

    virtual ~CreatureObject();
    bool hasCollision() const override { return true; }
    bool deleteMe() const override { return m_delete; }
    bool isConscious() const { return (!deleteMe() && m_creature->isConscious()); }
    Creature* getCreatureComponent() const { return m_creature; }
    const std::string getName() const { return m_creature->getName(); }
    void setName(const std::string& name) { m_creature->setName(name); }
    virtual eCreatureFaction getFaction() const { return m_creatureFaction; }
    void setFaction(eCreatureFaction faction) { m_creatureFaction = faction; }
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

    std::string getStartingDialogueLabel() const { return m_dialogue; }
    void setStartingDialogueLabel(const std::string& str) { m_dialogue = str; }

    bool isInCombat() const;
    void setInCombat() { m_inCombat = true; }
    void setOutofCombat() { m_inCombat = false; }

    void startCombatWith(const CreatureObject* creature);
    CombatManager* getCombatManager() const { return m_manager; }

    void setAIRole(eAIRoles role) { m_controller.setRole(role); }

    void setLoadout(eCreatureFaction faction, eRank rank);

protected:
    CreatureObjectId m_id;

    Creature* m_creature;
    eCreatureFaction m_creatureFaction;

    AICombatController m_combatController;
    AIController m_controller;

    int m_experience;

    int m_thirst;
    int m_hunger;
    int m_exhaustion;

    CombatManager* m_manager;

    bool m_inCombat;
    bool m_delete;

    std::string m_dialogue;

    std::unordered_map<eSkills, int> m_skills;

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        std::cout << "creatureobject: " << getName() << std::endl;
        ar& boost::serialization::base_object<Object>(*this);
        ar& m_creature;
        ar& m_creatureFaction;

        ar& m_experience;
        ar& m_thirst;
        ar& m_hunger;
        ar& m_dialogue;
        ar& m_skills;
    }
};
