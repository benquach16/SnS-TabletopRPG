// This is a complex class, since this combat in TROS-likes is complicated
// This flow specifically follows the combat system in the rulebook.
// Without reading the rulebook, this will not make sense to you.

#pragma once

#include <queue>
#include <unordered_map>

#include "creatures/creature.h"
#include "dice.h"
#include "items/types.h"
#include "log.h"

enum class eTempo : unsigned { First, Second };

enum class eInitiative : unsigned { Side1, Side2 };

enum class eCombatState : unsigned {
    Uninitialized = 0,
    Initialized = 1,
    RollInitiative = 2,
    PreexchangeActions = 3,
    PositionActions = 4,
    ResetState = 5, // used only for ui to observe combat manager state
    // These enums are confusingly named.
    // They are meant to be used on a red/red throw
    DualOffenseStealInitiative
    = 6, // on a red/red throw, if first attacker decides to steal initiative
    DualOffense1 = 7,
    DualOffense2 = 8,
    DualOffenseSecondInitiative
    = 9, // if first attacker didn't chose to steal initiative and second does

    Offense = 10,
    StolenOffense = 11, // defender tries to steal initiative, attacker now allocates
    Defense = 12,
    ParryLinked = 13,
    StealInitiative = 14,
    PostDefense = 15,
    PreResolution = 16, // for feinting
    Resolution = 17,
    DualOffenseResolve = 18,
    PostResolution = 19,
    FinishedCombat = 20,
    AttackFromDefense = 21,
    BetweenExchangeActions = 22,
    FeintAttack = 23,
};

class CombatInstance {
public:
    CombatInstance();
    void run();

    void initCombat(Creature* side1, Creature* side2, bool showAllMessages);
    void forceInitiative(eInitiative initiative);
    eCombatState getState() const { return m_currentState; }
    Creature* getSide1() const { return m_side1; }
    Creature* getSide2() const { return m_side2; }
    Creature* getAttacker() const;
    Creature* getDefender() const;
    eLength getCurrentReach() const { return m_currentReach; }
    bool isAttackerPlayer() const;
    bool isDefenderPlayer() const;
    void forceRefresh();
    void forceTempo(eTempo tempo) { m_currentTempo = tempo; }

    bool getInGrapple() const { return m_inGrapple; }
    bool getInWind() const { return m_inWind; }
    bool getLastTempo() const { return m_currentTempo == eTempo::Second; }
    bool getDualRedThrow() const { return m_dualRedThrow; }

private:
    int calcReachCost(Creature* creature, bool attacker);
    void doInitialization();
    void doRollInitiative();
    void doPreexchangeActions();
    void doPosition();
    void doResetState();
    void doDualOffense1();
    void doDualOffenseStealInitiative();
    void doDualOffense2();
    void doDualOffenseSecondInitiative();
    void doAttackFromDefense();
    void doStolenOffense();
    // doOffense has a return type to poll player input for dual offense
    bool doOffense();
    void doDefense();
    void doParryLinked();
    void doStealInitiative();
    void doPostDefense();
    void doPreResolution();
    void doFeintAttack();
    void doResolution();
    void doBetweenExchange();
    // this is a special case because if both sides roll to attack since both of
    // their attacks resolve at the same time
    void doDualOffenseResolve();
    void resolvePosition(Creature* creature);
    void doPostResolution();
    void doEndCombat();
    void resolveStealInit();
    void startGrapple(Creature* attacker, Creature* defender);
    void setSides(Creature*& attacker, Creature*& defender) const;
    bool switchToStaffGrip(Creature* creature);
    const Weapon* getAttackingWeapon(const Creature* creature);
    const Weapon* getDefendingWeapon(const Creature* creature);
    eLength getEffectiveReach(const Creature* creature);
    void outputOffense(const Creature* creature);
    void outputDefense(const Creature* creature);
    void changeReachTo(const Creature* creature);
    void applyAttackGuardBonuses(Creature* attacker);
    void applyDefendGuardBonuses(Creature* defender);
    int getAttackTN(const Creature* creature);
    int getDefendTN(const Creature* creature);
    void alignReach();

    void switchInitiative()
    {
        m_initiative = m_initiative == eInitiative::Side1 ? eInitiative::Side2 : eInitiative::Side1;
    }

    void switchTempo();

    // returns true if the target was killed
    bool inflictWound(Creature* attacker, int MoS, Offense attack, Creature* target);
    void writeMessage(const std::string& str,
        Log::eMessageTypes type = Log::eMessageTypes::Standard, bool important = false);
    void outputReachCost(int cost, Creature* creature, bool attacker);

    eInitiative m_initiative;
    Creature* m_side1;
    Creature* m_side2;
    eTempo m_currentTempo;
    eCombatState m_currentState;
    eLength m_currentReach;
    eHitLocations m_originalLocation;

    int m_dualWhiteTimes;
    bool m_dualRedThrow;
    int m_numTempos;

    bool m_inWind;
    bool m_inGrapple;

    bool m_sendAllMessages;
};
