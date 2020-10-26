#pragma once

#include <set>
#include <string>
#include <vector>

#include <boost/serialization/strong_typedef.hpp>

#include "combatinstance.h"
#include "creatures/creature.h"
#include "object/creatureobject.h"

enum class eCombatManagerState { RunCombat, PositioningRoll };

class CombatEdge {
public:
    // replace with boost::uuid
    BOOST_STRONG_TYPEDEF(unsigned, EdgeId);
    CombatEdge(CombatInstance* instance, CombatManager* vertex1, CombatManager* vertex2);
    ~CombatEdge();
    CombatInstance* getInstance() const { return m_instance; }
    CombatManager* getVertex1() const { return m_vertex1; }
    CombatManager* getVertex2() const { return m_vertex2; }
    void remove();
    EdgeId getId() { return m_id; }
    void setActive(bool active) { m_active = active; }
    bool getActive() const { return m_active; }
    void setCurrent(bool current) { m_current = current; }
    bool getCurrent() const { return m_current; }
    CombatManager* findOtherVertex(const CombatManager* manager) const;

private:
    EdgeId m_id;
    CombatEdge();
    bool m_active;

    // used primarily for showing the player ui when its ready for ui input
    bool m_current;
    CombatInstance* m_instance;
    CombatManager* m_vertex1;
    CombatManager* m_vertex2;
};

class CombatNode {
public:
    CombatNode(CreatureObject* creature);
    void run();

private:
    CombatNode();
    std::vector<CombatEdge> m_edges;
    CreatureObject* m_creature;
};

// node for a graph. instances act as edges
class CombatManager {
public:
    static constexpr float cTick = 0.9f;
    static constexpr int cMaxEngaged = 4;
    CombatManager(CreatureObject* creature);
    ~CombatManager();
    bool run(float tick);
    CombatEdge* getCurrentEdge() const;
    void startCombatWith(const CreatureObject* creature);
    eCombatManagerState getState() const { return m_currentState; }
    bool isEngaged() const { return m_edges.size() > 0; }
    void addEdge(CombatEdge* edge);
    bool canEngage() const { return m_edges.size() < cMaxEngaged; }
    void remove(CombatEdge::EdgeId id);
    bool isLeaf() const;
    bool isParent() const { return m_isParent; }
    void setLeaf() { m_isParent = false; }
    void setParent() { m_isParent = true; }
    void peel();
    unsigned getEngagementCount() const { return m_edges.size(); }
    bool isInDuel() const;
    bool hasPosition(Creature* creature, Creature* target);

private:
    CombatManager();
    enum class eOutnumberedSide { None, Side1, Side2 };
    void doRunCombat(float tick);
    void doPositionRoll();
    void cleanup();
    void switchInitiative()
    {
        m_currentTempo = (m_currentTempo == eTempo::First) ? eTempo::Second : eTempo::First;
    }
    void refreshInstances();
    void writeMessage(
        const std::string& str, Log::eMessageTypes type = Log::eMessageTypes::Standard);

    bool m_isParent;
    std::vector<CombatEdge*> m_edges;

    // m_edgeId is basically the currently processing edge
    // we go through all of these at a timing defined by the tick
    unsigned m_edgeId;
    bool m_positionDone;
    bool m_doPositionRoll;
    CreatureObject* m_mainCreature;
    bool m_isPlayers;
    eOutnumberedSide m_side;
    eTempo m_currentTempo;
    eCombatManagerState m_currentState;
};
