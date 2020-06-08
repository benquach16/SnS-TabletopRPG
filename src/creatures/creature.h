#pragma once

#include "../items/armor.h"
#include "../items/types.h"
#include "../items/weapon.h"
#include "manuever.h"
#include "types.h"
#include "wound.h"

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

#include <boost/serialization/strong_typedef.hpp>

static constexpr int cBaseBTN = 3;
static constexpr int cMinBTN = 2;
static constexpr int cMaxBTN = 6;
// different based on race
static constexpr int cBaseBloodLoss = 8;
static constexpr int cBaseConstitution = 2;

static constexpr int cTapFactor = 3;

enum class eCreatureType { Human, Goblin, Elf, Dwarf };

enum class eCreatureState { Idle = 0, Dead = 2, Unconscious = 3 };

enum class eCreatureFatigue { Hunger, Thirst, Sleepiness, Stamina };

static int getTap(int attribute)
{
    constexpr int cTapValue = 3;
    return attribute / cTapValue;
}

class Creature {
public:
    BOOST_STRONG_TYPEDEF(unsigned, CreatureId);

    Creature();
    virtual ~Creature() {}
    virtual eCreatureType getCreatureType() = 0;

    int getStrength() const { return m_strength; }
    int getAgility() const { return m_agility; }
    int getIntuition() const { return m_intuition; }
    int getPerception() const { return m_perception; }
    int getWillpower() const { return m_willpower; }

    int getGrit() const { return (m_strength + m_willpower) / 2; }
    int getShrewdness() const { return (m_intuition + m_perception) / 2; }
    int getReflex() const { return (m_agility + m_intuition) / 2; }
    int getSpeed() const { return (m_agility + m_strength) / 2; }

    int getConstitution() const { return cBaseConstitution; }

    int getBTN() const
    {
        return m_currentStance == eCreatureStance::Prone ? getDisadvantagedBTN() : m_BTN;
    }
    int getAdvantagedBTN() const { return std::max(m_BTN - 1, cMinBTN); }
    int getDisadvantagedBTN() const { return std::min(m_BTN + 1, cMaxBTN); }
    int getBloodLoss() const { return m_bloodLoss; }
    void setBTN(int BTN) { m_BTN = BTN; }

    const Weapon* getPrimaryWeapon() const;
    int getPrimaryWeaponId() const { return m_primaryWeaponId; }
    eLength getCurrentReach() const;
    void setGrip(eGrips grip) { m_currentGrip = grip; }
    eGrips getGrip() const { return m_currentGrip; }
    std::vector<const Armor*> getArmor() const;
    const std::vector<int>& getArmorId() const { return m_armor; }
    void setWeapon(int id);

    void setName(const std::string& name) { m_name = name; }
    std::string getName() const { return m_name; }

    int getProficiency(eWeaponTypes type) { return m_proficiencies[type]; }

    void inflictImpact(int impact);
    void inflictWound(Wound* wound);
    const std::vector<Wound*>& getWounds() const { return m_wounds; }
    int getSuccessRate() const;

    void equipArmor(int id);
    bool canEquipArmor(int id);
    ArmorSegment getArmorAtPart(eBodyParts part) const;
    void removeArmor(int id);
    float getAP() const { return m_AP; }

    // for current weapon
    int getCombatPool() const { return m_combatPool; }
    void resetCombatPool();
    void reduceCombatPool(int num)
    {
        assert(num >= 0);
        m_combatPool -= num;
    }
    void setBonusDice(int num) { m_bonusDice = num; }
    void addAndResetBonusDice();
    void reduceOffenseDie(int num)
    {
        m_currentOffense.dice -= num;
        m_currentOffense.dice = std::max(0, m_currentOffense.dice);
    }
    const std::vector<eHitLocations> getHitLocations() const;
    const std::set<eHitLocations>& getFavoredLocations() const { return m_favoredLocations; }

    // AI functions
    // move these to ai combat controller
    virtual bool isPlayer() { return false; }
    void doPrecombat(const Creature* opponent);
    void doOffense(
        const Creature* target, int reachCost, bool allin = false, bool dualRedThrow = false);

    void doDefense(const Creature* attacker, bool isLastTempo);

    void doStolenInitiative(const Creature* defender, bool allin = false);

    bool stealInitiative(const Creature* attacker, int& outDie);

    eInitiativeRoll doInitiative(const Creature* opponent);

    void doPositionRoll(const Creature* opponent);

    void doPreresolution(const Creature* opponent);

    void doStand();

    bool canPerformManuever(eOffensiveManuevers manuever);

    Offense getQueuedOffense() const { return m_currentOffense; }
    Defense getQueuedDefense() const { return m_currentDefense; }
    Position getQueuedPosition() const { return m_currentPosition; }

    bool getHasOffense() const { return m_hasOffense; }
    bool getHasDefense() const { return m_hasDefense; }
    bool getHasPosition() const { return m_hasPosition; }
    bool getHasPrecombat() const { return m_hasPrecombat; }
    bool getHasPreResolution() const { return m_hasPreResolution; }

    virtual void clearCreatureManuevers();
    int getOffenseManueverCost(eOffensiveManuevers manuever);
    bool setCreatureOffenseManuever(eOffensiveManuevers manuever);
    int getDefenseManueverCost(eDefensiveManuevers manuever);
    bool setCreatureDefenseManuever(eDefensiveManuevers manuever);
    bool setCreatureFeint();

    void setProne() { m_currentStance = eCreatureStance::Prone; }
    void setStand() { m_currentStance = eCreatureStance::Standing; }
    void disableWeapon(bool drop = false);
    void enableWeapon();

    eCreatureStance getStance() const { return m_currentStance; }

    eCreatureState getCreatureState() { return m_currentState; }
    bool isConscious()
    {
        return (m_currentState != eCreatureState::Unconscious)
            && (m_currentState != eCreatureState::Dead);
    }
    void setIdle() { m_currentState = eCreatureState::Idle; }
    void kill() { m_currentState = eCreatureState::Dead; }

    int getFatigue() const;

    bool getBleeding() const { return m_bleeding; }

    bool rollFatigue();

    bool hasEnoughMetalArmor() const;

    void getLowestArmorPart(eBodyParts* pPartOut, eHitLocations* pHitOut) const;

    CreatureId getId() const { return m_id; }

    void setStrength(int strength) { m_strength = strength; }
    void setAgility(int agility) { m_agility = agility; }
    void setIntuition(int intuition) { m_intuition = intuition; }
    void setPerception(int perception) { m_perception = perception; }
    void setWillpower(int willpower) { m_willpower = willpower; }
    void setProficiency(eWeaponTypes type, int value) { m_proficiencies[type] = value; }

    void attemptStand();
    void attemptPickup();

    bool droppedWeapon() const
    {
        return m_disableWeaponId != cFistsId && m_primaryWeaponId == cFistsId && m_disarm < 0;
    }

    bool isWeaponDisabled() const
    {
        return m_disableWeaponId != cFistsId && m_primaryWeaponId == cFistsId && m_disarm > 0;
    }

protected:
    CreatureId m_id;

    eCreatureState m_currentState;
    eCreatureStance m_currentStance;

    void clearArmor();
    void applyArmor();

    std::vector<eHitLocations> m_hitLocations;
    std::set<eHitLocations> m_favoredLocations;
    std::map<eBodyParts, ArmorSegment> m_armorValues;
    std::vector<Wound*> m_wounds;
    std::vector<int> m_armor;

    std::string m_name;

    Offense m_currentOffense;
    Defense m_currentDefense;
    Position m_currentPosition;

    bool m_hasOffense;
    bool m_hasDefense;
    bool m_hasPosition;
    bool m_hasPrecombat;
    bool m_hasPreResolution;

    std::vector<Manuever*> m_secondaryManuevers;

    // index
    int m_primaryWeaponId;
    int m_disableWeaponId;
    std::vector<int> m_quickDrawItems;
    eGrips m_currentGrip;

    int m_bloodLoss;
    bool m_bleeding;
    int m_BTN;
    float m_AP;

    std::unordered_map<eCreatureFatigue, int> m_fatigue;

    bool m_isPlayer;

    // stats
    int m_strength;
    int m_agility;
    int m_intuition;
    int m_perception;
    int m_willpower;

    int m_combatPool;
    int m_bonusDice;
    int m_disarm;

    std::unordered_map<eWeaponTypes, int> m_proficiencies;
};
