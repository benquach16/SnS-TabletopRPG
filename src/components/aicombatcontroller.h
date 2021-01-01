#pragma once

#include "../items/types.h"

enum eAICombatProfiles {
    Aggressive,
    Cautious,
    Neutral,
};

class CombatManager;
class Creature;
class CombatInstance;

class AICombatController {
public:
    void run(const CombatManager* manager, Creature* controlledCreature);

private:
    void doOffense(Creature* controlledCreature, const Creature* target, int reachCost,
        const CombatInstance* instance, bool allin = false, bool dualRedThrow = false,
        bool payCosts = true);
    void doDefense(Creature* controlledCreature, const Creature* attacker, const CombatInstance* instance, bool isLastTempo);

    void doStolenInitiative(Creature* controlledCreature, const Creature* defender, bool allin);

    void doPrecombat(Creature* controlledCreature, const Creature* opponent);

    void doInitiative(Creature* controlledCreature, const Creature* opponent);

    void doPositionRoll(Creature* controlledCreature, const Creature* opponent);

    void doPreresolution(Creature* controlledCreature, const Creature* opponent);

    bool setCreatureOffenseManuever(
        Creature* controlledCreature, eOffensiveManuevers manuever, eLength currentReach);
    bool setCreatureDefenseManuever(Creature* controlledCreature, eDefensiveManuevers manuever, eLength currentReach);

    bool stealInitiative(Creature* controlledCreature, const Creature* attacker, int& outDie);
};
