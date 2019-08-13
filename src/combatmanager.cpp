#include <algorithm>
#include <iostream>

#include "combatmanager.h"
#include "dice.h"
#include "log.h"

using namespace std;

// not remotely thread safe
static CombatEdge::EdgeId ids = static_cast<CombatEdge::EdgeId>(0);

CombatEdge::CombatEdge(CombatInstance* instance, CombatManager* vertex1, CombatManager* vertex2)
    : m_instance(instance)
    , m_vertex1(vertex1)
    , m_vertex2(vertex2)
    , m_active(false)
    , m_id(ids++)
{
}

void CombatEdge::remove() {}

CombatManager::CombatManager(CreatureObject* creature)
    : m_mainCreature(creature)
    , m_side(eOutnumberedSide::None)
    , m_currentTempo(eTempo::First)
    , m_currentState(eCombatManagerState::RunCombat)
    , m_positionDone(false)
    , m_edgeId(0)
    , m_doPositionRoll(false)
{
}

CombatManager::~CombatManager() { cleanup(); }

void CombatManager::cleanup()
{
    for (auto it : m_edges) {
        // remove from both vertices
    }
    m_edges.clear();
    m_currentTempo = eTempo::First;
    m_side = eOutnumberedSide::None;
    m_edgeId = 0;
}

bool CombatManager::run(float tick)
{
    if (m_mainCreature->isConscious() == false) {
        // cleanup, we don't need this anymore
        cleanup();
        return false;
    }
    if (m_edges.size() == 0) {
        m_edgeId = 0;
        cleanup();
        return false;
    }
    switch (m_currentState) {
    case eCombatManagerState::RunCombat:
        doRunCombat(tick);
        break;
    case eCombatManagerState::PositioningRoll:
        if (tick > cTick) {
            doPositionRoll();
        }
        break;
    default:
        assert(true);
        break;
    }

    // second check in case everyone died in previous iteration
    if (m_edges.size() == 0) {
        m_edgeId = 0;
        cleanup();
        return false;
    }
    return true;
}

void CombatManager::doRunCombat(float tick)
{
    if (m_doPositionRoll == true) {
        m_currentState = eCombatManagerState::PositioningRoll;
        return;
    }
    if (m_edges.size() > 1) {
        m_edges[m_edgeId].getInstance()->forceTempo(eTempo::First);
    }

    if (m_edges[m_edgeId].getActive() == false) {
        m_edgeId = m_edgeId < m_edges.size() ? m_edgeId + 1 : 0;
        return;
    }

    // ugly but needed for ui
    if (tick <= cTick) {
        return;
    }

    bool change = m_edges[m_edgeId].getInstance()->getState() == eCombatState::PostResolution;

    m_edges[m_edgeId].getInstance()->run();

    if (m_edges[m_edgeId].getInstance()->getState() == eCombatState::Uninitialized) {
        // remove from both vertices
        delete m_edges[m_edgeId].getInstance();
        CombatEdge::EdgeId id = m_edges[m_edgeId].getId();
        m_edges[m_edgeId].getVertex1()->remove(id);
        m_edges[m_edgeId].getVertex2()->remove(id);
        if (m_edges.size() > 1) {
            writeMessage("Combatant has been killed, refreshing combat pools");
            m_doPositionRoll = true;
        }
        refreshInstances();
        m_edgeId = 0;
        m_currentTempo = eTempo::First;
    }
    // since we just deleted, make sure we clear if we don't have any more
    // combat
    if (change == true) {
        m_positionDone = false;
        if (m_edgeId < static_cast<int>(m_edges.size()) - 1) {
            m_edgeId++;
        } else {
            if (m_currentTempo == eTempo::Second && m_edges.size() > 1) {
                writeMessage("Exchanges have ended, combat pools for all "
                             "combatants have reset");
                refreshInstances();
                m_doPositionRoll = true;
            }
            if (m_edges.size() > 1) {
                switchInitiative();
            }
            m_edgeId = 0;
        }
    }
}

void CombatManager::remove(CombatEdge::EdgeId id)
{
    for (unsigned i = 0; i < m_edges.size(); ++i) {
        if (m_edges[i].getId() == id) {
            m_edges.erase(m_edges.begin() + i);
            return;
        }
    }
}

void CombatManager::doPositionRoll()
{
    // do player, this is hack
    if (m_mainCreature->getCreatureComponent()->getHasPosition() == false) {
        m_currentState = eCombatManagerState::PositioningRoll;
        return;
    }
    Creature::CreatureId id = m_mainCreature->getCreatureComponent()->getId();
    for (auto it : m_edges) {
        // do positioning roll
        // side 2 not guarenteed to be the other side
        // it.getInstance()->getSide2()->doPositionRoll(m_mainCreature->getCreatureComponent());
        Creature* side1 = it.getInstance()->getSide1();
        Creature* side2 = it.getInstance()->getSide2();
        if (side1->getId() != id) {
            side1->doPositionRoll(m_mainCreature->getCreatureComponent());
        }
        if (side2->getId() != id) {
            side2->doPositionRoll(m_mainCreature->getCreatureComponent());
        }
    }
    unsigned count = 0;
    // now roll
    int mainSuccesses = DiceRoller::rollGetSuccess(m_mainCreature->getCreatureComponent()->getBTN(),
        m_mainCreature->getCreatureComponent()->getQueuedPosition().dice);
    for (unsigned i = 0; i < m_edges.size(); ++i) {
        // not always side 2
        Creature* creature = m_edges[i].getInstance()->getSide2();
        int successes
            = DiceRoller::rollGetSuccess(creature->getBTN(), creature->getQueuedPosition().dice);
        if (successes >= mainSuccesses) {
            writeMessage(
                creature->getName() + " kept up with " + m_mainCreature->getName() + "'s footwork");
            m_edges[i].setActive(true);
            count++;
        } else {
            m_edges[i].setActive(false);
        }
        creature->clearCreatureManuevers();
    }
    // have to have at least one
    if (count == 0) {
        writeMessage("No combatants kept up with footwork, initiating duel");
        m_edges[0].setActive(true);
    } else {
        writeMessage(
            m_mainCreature->getName() + " is engaged with " + to_string(count) + " opponents");
    }
    m_edgeId = 0;
    m_currentState = eCombatManagerState::RunCombat;
    m_doPositionRoll = false;
}

void CombatManager::addEdge(CombatEdge edge) { m_edges.push_back(edge); }

void CombatManager::startCombatWith(const CreatureObject* creature)
{
    if (creature->getCombatManager()->canEngage() == false) {
        return;
    }

    // if a creature initiatives combat against another creature, but is not the
    // main creature then we spin up another combatmanger
    CombatInstance* instance = new CombatInstance;

    writeMessage(
        "Combat started between " + m_mainCreature->getName() + " and " + creature->getName(),
        Log::eMessageTypes::Announcement);

    // hack to enforce player being side 1
    if (creature->isPlayer() == true) {
        instance->initCombat(
            creature->getCreatureComponent(), m_mainCreature->getCreatureComponent());
    } else {
        instance->initCombat(
            m_mainCreature->getCreatureComponent(), creature->getCreatureComponent());
    }
    CombatEdge edge(instance, this, creature->getCombatManager());
    if (m_edges.size() == 0) {
        edge.setActive(true);
    }
    m_edges.push_back(edge);

    creature->getCombatManager()->addEdge(edge);
}

CombatInstance* CombatManager::getCurrentInstance() const
{
    if (m_edges.size() == 0) {
        return nullptr;
    }
    assert(m_edgeId < m_edges.size());
    return m_edges.at(m_edgeId).getInstance();
}

void CombatManager::refreshInstances()
{
    for (auto it : m_edges) {
        it.getInstance()->forceRefresh();
    }
}

void CombatManager::writeMessage(const std::string& str, Log::eMessageTypes type)
{
    // combat manager is not a singleton, so we can have multiple.
    // we can choose not to display combatmanager messages if we want to.
    Log::push(str, type);
}
