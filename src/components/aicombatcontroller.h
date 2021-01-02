#pragma once

#include <unordered_map>
#include "../items/types.h"
#include "../creatures/types.h"

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

	eHitLocations getBestHitLocation(const Creature* target, const Component* component, int& outDamage);

	// <hit location, priority>
	std::unordered_map<eHitLocations, int> m_hitPriorities;
};
