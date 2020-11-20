#pragma once

#include "../items/types.h"

enum eAICombatProfiles {
    Aggressive,
    Cautious,
    Neutral,
};

class CombatManager;
class Creature;

class AICombatController {
public:
    void run(const CombatManager* manager, Creature* controlledCreature);

private:
    void doOffense(Creature* controlledCreature, const Creature* target, int reachCost,
        eLength currentReach, bool allin = false, bool dualRedThrow = false, bool payCosts = true);
    void doDefense(Creature* controlledCreature, const Creature* attacker, bool isLastTempo);

    bool setCreatureOffenseManuever(
        Creature* controlledCreature, eOffensiveManuevers manuever, eLength currentReach);
    bool setCreatureDefenseManuever(Creature* controlledCreature, eDefensiveManuevers manuever);

    bool stealInitiative(const Creature* attacker, int& outDie);
};
