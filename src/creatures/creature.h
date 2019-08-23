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
static constexpr int cTapFactor = 3;

enum class eCreatureType { Human };

enum class eCreatureState { Idle = 0, Dead = 2, Unconscious = 3 };

enum class eCreatureFatigue { Hunger, Thirst, Sleepiness, Stamina };

class Creature {
public:
    BOOST_STRONG_TYPEDEF(unsigned, CreatureId);

    Creature();
    virtual ~Creature() {}
    virtual eCreatureType getCreatureType() = 0;

    int getBrawn() const { return m_brawn; }
    int getAgility() const { return m_agility; }
    int getCunning() const { return m_cunning; }
    int getPerception() const { return m_perception; }
    int getWill() const { return m_will; }

    int getGrit() const { return (m_brawn + m_will) / 2; }
    int getKeen() const { return (m_cunning + m_perception) / 2; }
    int getReflex() const { return (m_agility + m_cunning) / 2; }
    int getSpeed() const { return (m_agility + m_brawn) / 2; }

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
    void inflictWound(Wound* wound, bool manueverFirst = false);
    const std::vector<Wound*>& getWounds() const { return m_wounds; }
    int getSuccessRate() const;

    void equipArmor(int id);
    bool canEquipArmor(int id);
    ArmorSegment getArmorAtPart(eBodyParts part);
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
    void doPrecombat();
    void doOffense(
        const Creature* target, int reachCost, bool allin = false, bool dualRedThrow = false);

    void doDefense(const Creature* attacker, bool isLastTempo);

    void doStolenInitiative(const Creature* defender, bool allin = false);

    bool stealInitiative(const Creature* attacker, int& outDie);

    eInitiativeRoll doInitiative(const Creature* opponent);

    void doPositionRoll(const Creature* opponent);

    void doStand();

    bool canPerformManuever(eOffensiveManuevers manuever);

    Offense getQueuedOffense() const { return m_currentOffense; }
    Defense getQueuedDefense() const { return m_currentDefense; }
    Position getQueuedPosition() const { return m_currentPosition; }

    bool getHasOffense() const { return m_hasOffense; }
    bool getHasDefense() const { return m_hasDefense; }
    bool getHasPosition() const { return m_hasPosition; }
    bool getHasPrecombat() const { return m_hasPrecombat; }

    virtual void clearCreatureManuevers();

    void setProne() { m_currentStance = eCreatureStance::Prone; }
    void setStand() { m_currentStance = eCreatureStance::Standing; }

    eCreatureStance getStance() const { return m_currentStance; }

    eCreatureState getCreatureState() { return m_currentState; }
    bool isConscious()
    {
        return (m_currentState != eCreatureState::Unconscious)
            && (m_currentState != eCreatureState::Dead);
    }
    void setIdle() { m_currentState = eCreatureState::Idle; }
    void kill() { m_currentState = eCreatureState::Dead; }

    int getFatigue() const { return m_fatigue.at(eCreatureFatigue::Stamina); }

    bool getBleeding() const { return m_bleeding; }

    bool rollFatigue();

    CreatureId getId() const { return m_id; }

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

    std::vector<Manuever*> m_secondaryManuevers;

    // index
    int m_primaryWeaponId;
    int m_disableWeaponId;
    eGrips m_currentGrip;

    int m_bloodLoss;
    bool m_bleeding;
    int m_BTN;
    float m_AP;

    std::unordered_map<eCreatureFatigue, int> m_fatigue;

    bool m_isPlayer;

    // stats
    int m_brawn;
    int m_agility;
    int m_cunning;
    int m_perception;
    int m_will;

    int m_combatPool;
    int m_bonusDice;
    int m_disarm;

    std::unordered_map<eWeaponTypes, int> m_proficiencies;
};
