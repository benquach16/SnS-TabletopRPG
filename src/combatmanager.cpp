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
    , m_current(false)
    , m_id(ids++)
{
    assert(m_vertex1 != m_vertex2);
}

CombatEdge::~CombatEdge() { remove(); }

void CombatEdge::remove()
{
    delete m_instance;
    m_vertex1->remove(getId());
    m_vertex2->remove(getId());
}

CombatManager* CombatEdge::findOtherVertex(const CombatManager* manager) const
{
    assert(manager == m_vertex1 || manager == m_vertex2);
    if (manager == m_vertex1) {
        return m_vertex2;
    } else {
        return m_vertex1;
    }
}

CombatManager::CombatManager(CreatureObject* creature)
    : m_mainCreature(creature)
    , m_side(eOutnumberedSide::None)
    , m_currentTempo(eTempo::First)
    , m_currentState(eCombatManagerState::RunCombat)
    , m_positionDone(false)
    , m_edgeId(0)
    , m_doPositionRoll(false)
    , m_isParent(false)
{
}

CombatManager::~CombatManager() { cleanup(); }

void CombatManager::cleanup()
{
    m_isParent = false;
    while (m_edges.empty() == false) {
        delete m_edges[0];
    }
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
    if (m_edgeId >= m_edges.size()) {
        m_edgeId = 0;

        if (m_currentTempo == eTempo::Second && m_edges.size() > 1) {
            m_doPositionRoll = true;
            refreshInstances();
        }
        switchInitiative();
        return;
    }

    if (m_edges[m_edgeId]->getActive() == false) {
        m_edgeId = m_edgeId < m_edges.size() ? m_edgeId + 1 : 0;
        return;
    }

    if (m_edges.size() > 1) {
        m_edges[m_edgeId]->getInstance()->forceTempo(eTempo::First);
    }
    // ugly but needed for ui
    if (tick <= cTick) {
        return;
    }

    m_edges[m_edgeId]->setCurrent(true);

    // if change is true then we will increment m_edgeId
    bool change = m_edges[m_edgeId]->getInstance()->getState() == eCombatState::PostResolution;

    m_edges[m_edgeId]->getInstance()->run();

    if (m_edges[m_edgeId]->getInstance()->getState() == eCombatState::Uninitialized) {
        // remove from both vertices
        if (m_mainCreature->isConscious() == false) {
            // cleanup, we don't need this anymore
            cleanup();
        } else {
            m_edges[m_edgeId]->remove();
        }

        if (m_edges.size() > 1) {
            writeMessage("Combatant has been killed, refreshing combat pools");
            m_doPositionRoll = true;
        }
        refreshInstances();
        m_edgeId = 0;
        m_currentTempo = eTempo::First;

        // make sure we force the last edge to be active if we drop down to 1 edge
        if (m_edges.size() == 1) {
            m_edges[0]->setActive(true);
        }
    }
    if (m_edges[m_edgeId]->getInstance()->getInGrapple()) {
        // if we are in a grapple, turn this into a duel
        // since this function only runs on the parent, that means we disconnect all edges
        // except for grapple
        CombatEdge::EdgeId savedEdgeId = m_edges[m_edgeId]->getId();
        for (unsigned i = 0; i < m_edges.size(); i++) {
            if (m_edges[i]->getId() != savedEdgeId) {
                m_edges[i]->remove();
            }
        }
        m_edgeId = 0;
    }

    // since we just deleted, make sure we clear if we don't have any more
    // combat
    if (change == true) {
        m_positionDone = false;
        m_edges[m_edgeId]->setCurrent(false);
        if (m_edgeId < static_cast<unsigned>(m_edges.size()) - 1) {
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
        if (m_edges[i]->getId() == id) {
            m_edges.erase(m_edges.begin() + i);
            return;
        }
    }
    assert(true);
}

bool CombatManager::hasPosition(Creature* creature, Creature* target)
{
    if (creature->getHasPosition() == false) {
        if (creature->isPlayer() == true) {
            return false;
        } else {
            creature->doPositionRoll(target);
        }
    }
    return true;
}

void CombatManager::doPositionRoll()
{
    assert(m_edges.size() > 1);
    // do main creature positioning first
    if (hasPosition(m_mainCreature->getCreatureComponent(), nullptr) == false) {
        m_currentState = eCombatManagerState::PositioningRoll;
        return;
    }

    Creature::CreatureId id = m_mainCreature->getCreatureComponent()->getId();

    // make sure we also wait for player
    bool allSidesReady = true;
    for (auto it : m_edges) {
        // do positioning roll
        // side 2 not guarenteed to be the other side
        // it.getInstance()->getSide2()->doPositionRoll(m_mainCreature->getCreatureComponent());
        Creature* side1 = it->getInstance()->getSide1();
        Creature* side2 = it->getInstance()->getSide2();
        if (side1->getId() != id) {
            if (hasPosition(side1, m_mainCreature->getCreatureComponent()) == false) {
                m_currentState = eCombatManagerState::PositioningRoll;
                return;
            }
        }
        if (side2->getId() != id) {
            if (hasPosition(side2, m_mainCreature->getCreatureComponent()) == false) {
                m_currentState = eCombatManagerState::PositioningRoll;
                return;
            }
        }
    }
    unsigned count = 0;
    // now roll
    int mainSuccesses = DiceRoller::rollGetSuccess(m_mainCreature->getCreatureComponent()->getBTN(),
        m_mainCreature->getCreatureComponent()->getQueuedPosition().dice);
    for (auto it : m_edges) {
        // not always side 2
        Creature* side1 = it->getInstance()->getSide1();
        Creature* side2 = it->getInstance()->getSide2();
        Creature* creature = nullptr;
        eInitiative forceInitiative = eInitiative::Side1;
        if (side1->getId() != id) {
            creature = side1;
        }
        if (side2->getId() != id) {
            creature = side2;
            forceInitiative = eInitiative::Side2;
        }
        assert(creature != nullptr);
        int successes
            = DiceRoller::rollGetSuccess(creature->getBTN(), creature->getQueuedPosition().dice);
        if (successes >= mainSuccesses) {
            writeMessage(
                creature->getName() + " kept up with " + m_mainCreature->getName() + "'s footwork");
            it->setActive(true);
            count++;
            it->getInstance()->forceInitiative(forceInitiative);
        } else {
            it->setActive(false);
        }
        creature->clearCreatureManuevers();
    }
    // have to have at least one
    if (count == 0) {
        writeMessage("No combatants kept up with footwork, initiating duel");
        m_edges[0]->setActive(true);
    } else {
        writeMessage(
            m_mainCreature->getName() + " is engaged with " + to_string(count) + " opponents");
    }
    m_edgeId = 0;
    m_currentState = eCombatManagerState::RunCombat;
    m_doPositionRoll = false;
}

void CombatManager::addEdge(CombatEdge* edge)
{
    m_edges.push_back(edge);
    if (m_edges.size() > 1) {
        // ensure all edges are connected to leaves
        m_isParent = true;
        // new combat should always be deactivated until position roll
        edge->setActive(false);
        for (auto it : m_edges) {
            CombatManager* otherManager = it->findOtherVertex(this);
            otherManager->setLeaf();
            assert(otherManager->getEngagementCount() == 1);
        }
    }
}

void CombatManager::startCombatWith(const CreatureObject* creature)
{
    // general concept of this - in a duel, parent is assigned to the creature who initiated duel.
    // if another creature enters the engagement, all nodes are set to leaf except for the node with
    // multiple edges.

    if (creature->getCombatManager()->canEngage() == false) {
        return;
    }

    if (m_edges.size() == 1) {
        if (m_edges[0]->getInstance()->getInGrapple() == true) {
            writeMessage("Cannot start combat while target is in a grapple.");
            return;
        }
    }

    // assume creature attempts to stand before attacking someone
    m_mainCreature->getCreatureComponent()->setStand();

    if (creature->getCombatManager()->isLeaf() == true) {
        // peel off creature from previous engagement, unless the engagement is a duel. if we're in
        // a duel, then we can just freely assign parent without peeling
        if (creature->getCombatManager()->isInDuel() == false) {
            creature->getCombatManager()->peel();
        }
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
            creature->getCreatureComponent(), m_mainCreature->getCreatureComponent(), true);
    } else if (m_mainCreature->isPlayer() == true) {
        instance->initCombat(
            m_mainCreature->getCreatureComponent(), creature->getCreatureComponent(), true);
    } else {
        instance->initCombat(
            m_mainCreature->getCreatureComponent(), creature->getCreatureComponent(), false);
    }
    CombatEdge* edge = new CombatEdge(instance, this, creature->getCombatManager());
    if (m_edges.size() == 0) {
        edge->setActive(true);
        m_isParent = false;
    }

    if (creature->getCombatManager()->isParent() == false && m_isParent == false) {
        m_isParent = true;
    }

    m_edges.push_back(edge);

    creature->getCombatManager()->addEdge(edge);
}

bool CombatManager::isLeaf() const
{
    if (m_edges.size() > 1) {
        return false;
    }
    if (m_edges.size() == 0) {
        return true;
    }
    if (m_edges[0]->findOtherVertex(this)->isParent() == false) {
        return false;
    }
    return true;
}

bool CombatManager::isInDuel() const
{
    if (m_edges.size() != 1) {
        return false;
    }
    if (m_edges[0]->findOtherVertex(this)->getEngagementCount() == 1) {
        return true;
    }
    return false;
}

void CombatManager::peel()
{
    if (m_edges.size() == 0) {
        return;
    }
    Log::push(m_mainCreature->getName() + " has left their combat to fight a different combatant",
        Log::eMessageTypes::Announcement);
    m_edges[0]->remove();
}

CombatEdge* CombatManager::getCurrentEdge() const
{
    if (m_edges.size() == 0 || m_edgeId >= m_edges.size()) {
        return nullptr;
    }
    auto edge = m_edges.at(m_edgeId);
    return edge;
}

void CombatManager::refreshInstances()
{
    for (auto it : m_edges) {
        it->getInstance()->forceRefresh();
    }
}

void CombatManager::writeMessage(const std::string& str, Log::eMessageTypes type)
{
    // combat manager is not a singleton, so we can have multiple.
    // we can choose not to display combatmanager messages if we want to.
    Log::push(str, type);
}
