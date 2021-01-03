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

#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/strong_typedef.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

static constexpr int cBaseBTN = 7;
static constexpr int cMinBTN = 5;
static constexpr int cMaxBTN = 10;
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
    friend class boost::serialization::access;

    BOOST_STRONG_TYPEDEF(unsigned, CreatureId);

    Creature(int naturalWeaponId);

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
    int getMobility() const { return (m_agility + m_strength) / 2; }

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
    const Weapon* getSecondaryWeapon() const;
    int getSecondaryWeaponId() const { return m_secondaryWeaponId; }
    const Weapon* getNaturalWeapon() const;
	int getNaturalWeaponId() const { return m_naturalWeaponId; }
    std::vector<int> getQuickdrawItems() const { return m_quickDrawItems; }
    bool findInQuickdraw(int id);
    void addQuickdrawItem(int id) { m_quickDrawItems.push_back(id); }
    void removeQuickdrawItem(int id);
    eLength getCurrentReach() const;
    eLength getSecondaryWeaponReach() const;
    void setGrip(eGrips grip) { m_currentGrip = grip; }
    eGrips getGrip() const { return m_currentGrip; }
    std::vector<const Armor*> getArmor() const;
    const std::vector<int>& getArmorId() const { return m_armor; }
    void setPrimaryWeapon(int id);
    void setSecondaryWeapon(int id);
    void removePrimaryWeapon() { m_primaryWeaponId = m_naturalWeaponId; }
    void removeSecondaryWeapon() { m_secondaryWeaponId = m_naturalWeaponId; }
    std::vector<int> getDroppedWeapons() const { return m_droppedWeapons; }
    void clearDroppedWeapons();
    int getNumEquipped(int id);

    void setName(const std::string& name) { m_name = name; }
    std::string getName() const { return m_name; }

    int getProficiency(eWeaponTypes type) { return m_proficiencies[type]; }

    void inflictImpact(int impact);
    void inflictWound(Wound* wound);
    const std::unordered_map<eBodyParts, int>& getWounds() const { return m_wounds; }
    int getSuccessRate() const;

    void equipArmor(int id);
    bool canEquipArmor(int id);
    ArmorSegment getArmorAtPart(eBodyParts part) const;
	std::vector<ArmorSegment> getArmorAtLocation(eHitLocations location) const;
	ArmorSegment getMedianArmor(eHitLocations location) const;
    void removeArmor(int id);
    float getAP() const { return m_AP; }

    // for current weapon
    int getCombatPool() const { return m_combatPool; }
    void resetCombatPool();
    int getMaxCombatPool();
    void reduceCombatPool(int num)
    {
        assert(num >= 0);
        std::cout << "using " << num << "die\n";
        m_combatPool -= num;
    }
    void setBonusDice(int num) { m_bonusDice = num; }
    void addAndResetBonusDice();
    void reduceOffenseDie(int num)
    {
        m_currentOffense.dice -= num;
        m_currentOffense.dice = std::max(0, m_currentOffense.dice);
    }
    void reduceDefenseDie(int num)
    {
        m_currentDefense.dice -= num;
        m_currentDefense.dice = std::max(0, m_currentDefense.dice);
    }
    const std::vector<eHitLocations> getHitLocations() const;
    const std::set<eHitLocations>& getFavoredLocations() const { return m_favoredLocations; }

    // AI functions
    // move these to ai combat controller
    virtual bool isPlayer() { return false; }

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
    bool getHasPreResolution() const { return m_hasPreResolution; }
    bool pollForInitiative()
    {
        bool ret = m_flagInitiative;
        if (ret == true) {
            // m_flagInitiative = false;
        }
        return ret;
    }
    void resetInitiative() { m_flagInitiative = false; }
    void setOffenseWeapon(bool isPrimary) { m_currentOffense.withPrimaryWeapon = isPrimary; }
    void setOffenseManuever(eOffensiveManuevers manuever) { m_currentOffense.manuever = manuever; }
    void setOffenseDice(int dice) { m_currentOffense.dice = dice; }
    void setOffenseComponent(const Component* component) { m_currentOffense.component = component; }
    void setOffenseTarget(eHitLocations location) { m_currentOffense.target = location; }
    void setOffensePinpointTarget(eBodyParts part) { m_currentOffense.pinpointTarget = part; }
    void setOffensePinpoint() { m_currentOffense.pinpoint = true; }
    void setOffenseLinked() { m_currentOffense.linked = true; }
    void setOffenseFeint() { m_currentOffense.feint = true; }
    void setOffenseHeavyDice(int dice) { m_currentOffense.heavyblow = dice; }

    void setDefenseWeapon(bool isPrimary) { m_currentDefense.withPrimaryWeapon = isPrimary; }
    void setDefenseManuever(eDefensiveManuevers manuever) { m_currentDefense.manuever = manuever; }
    void setDefenseDice(int dice) { m_currentDefense.dice = dice; }

    void setPositionManuever(ePositionManuevers manuever) { m_currentPosition.manuever = manuever; }
    void setPositionDice(int dice) { m_currentPosition.dice = dice; }

    void setOffenseReady() { m_hasOffense = true; }
    void setDefenseReady() { m_hasDefense = true; }
    void setPositionReady() { m_hasPosition = true; }
    void setInitiative(eInitiativeRoll initiative);
    eInitiativeRoll getInitiative() { return m_initiative; }
    void addFavored(eHitLocations location) { m_favoredLocations.insert(location); }
    void setPrecombatReady() { m_hasPrecombat = true; }
    void setPreResolutionReady() { m_hasPreResolution = true; }

    virtual void clearCreatureManuevers(bool skipDisable = false);

    bool setCreatureFeint();

    void setProne() { m_currentStance = eCreatureStance::Prone; }
    void setStand() { m_currentStance = eCreatureStance::Standing; }
    void disableWeapon();
    void dropWeapon();
    void dropSecondaryWeapon();
    void enableWeapon();
    void pickupWeapon();

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

    int getPain() const { return std::max(m_pain - getGrit(), 0); }

    bool getBleeding() const { return m_bleeding; }

    bool rollFatigue();

    void resetFatigue();

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

    bool primaryWeaponDisabled() const { return m_primaryWeaponDisabled; }

    bool secondaryWeaponDisabled() const { return m_secondaryWeaponDisabled; }

protected:
    CreatureId m_id;

    eCreatureState m_currentState;
    eCreatureStance m_currentStance;

    void clearArmor();
    void applyArmor();

    std::vector<eHitLocations> m_hitLocations;
    std::set<eHitLocations> m_favoredLocations;
    std::map<eBodyParts, ArmorSegment> m_armorValues;
    // int == wound level
    std::unordered_map<eBodyParts, int> m_wounds;
    std::vector<int> m_armor;
    bool m_hasLeftHand;
    bool m_hasRightHand;

    std::string m_name;

    Offense m_currentOffense;
    Defense m_currentDefense;
    Position m_currentPosition;
    eInitiativeRoll m_initiative;

    bool m_flagInitiative;
    bool m_hasOffense;
    bool m_hasDefense;
    bool m_hasPosition;
    bool m_hasPrecombat;
    bool m_hasPreResolution;

    std::vector<Manuever*> m_secondaryManuevers;

    // index
    int m_primaryWeaponId;
    int m_secondaryWeaponId;
    bool m_primaryWeaponDisabled;
    bool m_secondaryWeaponDisabled;
    int m_naturalWeaponId;
    int m_disableWeaponId;
    std::vector<int> m_quickDrawItems;
    std::vector<int> m_droppedWeapons;
    eGrips m_currentGrip;

    unsigned m_bloodLoss;
    bool m_bleeding;
    int m_BTN;
    int m_pain;
    float m_AP;

    std::unordered_map<eCreatureFatigue, unsigned> m_fatigue;

    // stats
    int m_strength;
    int m_agility;
    int m_intuition;
    int m_perception;
    int m_willpower;

    int m_combatPool;
    int m_bonusDice;

    // turns the character is disarmed or disabled
    int m_disarm;

    std::unordered_map<eWeaponTypes, int> m_proficiencies;

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        // apply armor before and after to ensure everything is updated
        applyArmor();

        ar& m_strength;
        ar& m_agility;
        ar& m_intuition;
        ar& m_perception;
        ar& m_willpower;

        ar& m_proficiencies;
        ar& m_primaryWeaponId;
        ar& m_secondaryWeaponId;
        ar& m_primaryWeaponDisabled;
        ar& m_secondaryWeaponDisabled;
        ar& m_naturalWeaponId;
        ar& m_disableWeaponId;
        ar& m_quickDrawItems;

        ar& m_currentGrip;
        ar& m_name;
        ar& m_currentState;
        ar& m_currentStance;

        ar& m_bleeding;
        ar& m_BTN;
        ar& m_pain;

        ar& m_hitLocations;
        ar& m_armor;
        ar& m_wounds;

        applyArmor();
    }
};
