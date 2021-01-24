#pragma once

#include "../creatures/types.h"
#include "../items/types.h"
#include <unordered_map>

enum eAICombatProfiles {
    Aggressive,
    Cautious,
    Neutral,
};

class CombatManager;
class Creature;
class CombatInstance;
class Component;

class AICombatController {
public:
    AICombatController();

    void run(const CombatManager* manager, Creature* controlledCreature);

private:
    void doOffense(Creature* controlledCreature, const Creature* target, int reachCost,
        const CombatInstance* instance, bool allin = false, bool dualRedThrow = false,
        bool payCosts = true);

    void chooseOffenseManuever(Creature* controlledCreature, const Creature* target,
        const CombatInstance* instance, bool allin, bool payReach, bool feint);
    void doDefense(Creature* controlledCreature, const Creature* attacker,
        const CombatInstance* instance, bool isLastTempo);

    void doStolenInitiative(Creature* controlledCreature, const Creature* defender, bool allin);

    void doPrecombat(
        Creature* controlledCreature, const Creature* opponent, const CombatInstance* instance);

    void doInitiative(Creature* controlledCreature, const Creature* opponent);

    void doPositionRoll(Creature* controlledCreature, const Creature* opponent);

    void doPreresolution(
        Creature* controlledCreature, const Creature* opponent, const CombatInstance* instance);

    bool setCreatureOffenseManuever(
        Creature* controlledCreature, eOffensiveManuevers manuever, eLength currentReach, int cost);
    bool setCreatureDefenseManuever(
        Creature* controlledCreature, eDefensiveManuevers manuever, eLength currentReach, int cost);

    void normalizeGrip(Creature* controlledCreature, bool isLastTempo);
    void shortenGrip(Creature* controlledCreature, bool isLastTempo);

    bool stealInitiative(
        Creature* controlledCreature, const Creature* attacker, int cost, int& outDie);

    eHitLocations getBestHitLocation(
        const Creature* target, const Component* component, bool feint, eHitLocations originalLocation, int& outDamage);

    // <hit location, priority>
    std::unordered_map<eHitLocations, int> m_hitPriorities;

    bool m_doFeint;
    eHitLocations m_realAttack;
};
