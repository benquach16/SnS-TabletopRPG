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
    DualOffenseStealInitiative = 6,
    DualOffense1 = 7,
    DualOffense2 = 8,
    DualOffenseSecondInitiative = 9,
    Offense = 10,
    StolenOffense = 11,
    Defense = 12,
    ParryLinked = 13,
    StealInitiative = 14,
    PostDefense = 15,
    Resolution = 16,
    DualOffenseResolve = 17,
    PostResolution = 18,
    FinishedCombat = 19
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
    eLength getCurrentReach() const { return m_currentReach; }
    bool isAttackerPlayer();
    bool isDefenderPlayer();
    void forceRefresh();
    void forceTempo(eTempo tempo) { m_currentTempo = tempo; }

    bool getInGrapple() const { return m_inGrapple; }
    bool getInWind() const { return m_inWind; }

private:
    void doInitialization();
    void doRollInitiative();
    void doPreexchangeActions();
    void doPosition();
    void doResetState();
    void doDualOffense1();
    void doDualOffenseStealInitiative();
    void doDualOffense2();
    void doDualOffenseSecondInitiative();
    void doStolenOffense();
    // doOffense has a return type to poll player input for dual offense
    bool doOffense();
    void doDefense();
    void doParryLinked();
    void doStealInitiative();
    void doPostDefense();
    void doResolution();
    // this is a special case because if both sides roll to attack since both of
    // their attacks resolve at the same time
    void doDualOffenseResolve();
    void resolvePosition(Creature* creature);
    void doPostResolution();
    void doEndCombat();
    void setSides(Creature*& attacker, Creature*& defender);
    bool switchToStaffGrip(Creature* creature);
    void outputOffense(Creature* creature);
    void outputDefense(Creature* creature);

    void switchInitiative()
    {
        m_initiative = m_initiative == eInitiative::Side1 ? eInitiative::Side2 : eInitiative::Side1;
    }
    void switchTempo();

    bool inflictWound(
        Creature* attacker, int MoS, Offense attack, Creature* target, bool manueverFirst = false);
    void writeMessage(const std::string& str,
        Log::eMessageTypes type = Log::eMessageTypes::Standard, bool important = false);
    void outputReachCost(int cost, Creature* attacker);

    eInitiative m_initiative;
    Creature* m_side1;
    Creature* m_side2;
    eTempo m_currentTempo;
    eCombatState m_currentState;
    eLength m_currentReach;

    int m_dualWhiteTimes;
    bool m_dualRedThrow;
    int m_numTempos;

    bool m_inWind;
    bool m_inGrapple;

    bool m_sendAllMessages;
};
