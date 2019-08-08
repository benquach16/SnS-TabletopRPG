#include <algorithm>
#include <iostream>

#include "combatmanager.h"
#include "dice.h"
#include "log.h"

using namespace std;

CombatManager::CombatManager(Creature* creature)
    : m_currentId(0)
    , m_mainCreature(creature)
    , m_side(eOutnumberedSide::None)
    , m_currentTempo(eTempo::First)
    , m_currentState(eCombatManagerState::RunCombat)
    , m_positionDone(false)
    , m_instanceId(0)
    , m_allowNewCreatures(true)
{
    m_isPlayers = creature->isPlayer();
}

CombatManager::~CombatManager() { cleanup(); }

void CombatManager::cleanup()
{
    for (auto it : m_instances) {
        delete it;
    }
    m_instances.clear();
    m_currentTempo = eTempo::First;
    m_side = eOutnumberedSide::None;
    m_currentId = 0;
    m_instanceId = 0;
}

bool CombatManager::run(float tick)
{
    m_allowNewCreatures = true;
    if (m_mainCreature->isConscious() == false) {
        // cleanup, we don't need this anymore
        cleanup();
        return false;
    }
    if (m_instances.size() == 0) {
        m_instanceId = 0;
        m_currentId = 0;
        cleanup();
        for (auto it : m_queuedCreatures) {
            startCombatWith(it);
        }
        // cout << "no more instances" << endl;
        return false;
    }
    m_allowNewCreatures = false;
    switch (m_currentState) {
    case eCombatManagerState::RunCombat:
        doRunCombat(tick);
        break;
    case eCombatManagerState::PositioningRoll:
        if (tick > cTick) {
            doPositionRoll();
        }
        break;
    }

    // second check in case everyone died in previous iteration
    if (m_instances.size() == 0) {
        m_currentId = 0;
        m_instanceId = 0;
        cleanup();
        // cout << "no more instances" << endl;
        return false;
    }
    return true;
}

void CombatManager::doRunCombat(float tick)
{
    if (m_instances.size() > 1) {
        // do positioning roll
        if (m_instanceId == 0 && m_positionDone == false && m_currentTempo == eTempo::First) {
            m_currentState = eCombatManagerState::PositioningRoll;
            return;
        }
        // force tempo
        // activeinstances should be populated at this time
        assert(m_activeInstances.size() > 0);
        m_currentId = m_activeInstances[m_instanceId];
        m_instances[m_currentId]->forceTempo(eTempo::First);

        // force outnumbered side to be defensive
        // player should always be side 1
        if (m_isPlayers == true) {
            for (auto it : m_instances) {
                // bug : this state causes ui skips, make a seperate state
                if (it->getState() == eCombatState::RollInitiative) {
                    // it->forceInitiative(eInitiative::Side2);
                }
            }
        }
    }

    if (tick <= cTick) {
        return;
    }
    // index into array of indexes for active instances
    m_currentId = m_instances.size() > 1 ? m_activeInstances[m_instanceId] : 0;

    bool change = m_instances[m_currentId]->getState() == eCombatState::PostResolution;

    m_instances[m_currentId]->run();

    if (m_instances[m_currentId]->getState() == eCombatState::Uninitialized) {
        delete m_instances[m_currentId];
        m_instances.erase(m_instances.begin() + m_currentId);
        if (m_instanceId == m_activeInstances.size()) {
            m_instanceId = 0;
            m_allowNewCreatures = true;
        }
        if (m_instances.size() > 1) {
            writeMessage("Combatant has been killed, refreshing combat pools");
        }
        refreshInstances();
        m_currentId = 0;
        m_currentTempo = eTempo::First;
    }
    // since we just deleted, make sure we clear if we don't have any more
    // combat
    if (m_activeInstances.size() == 0) {
        m_instanceId = 0;
    }
    if (change == true) {
        m_positionDone = false;
        if (m_instanceId < static_cast<int>(m_activeInstances.size()) - 1) {
            m_instanceId++;
        } else {
            if (m_currentTempo == eTempo::Second && m_instances.size() > 1) {
                writeMessage("Exchanges have ended, combat pools for all "
                             "combatants have reset");
                refreshInstances();
            }
            if (m_currentTempo == eTempo::Second) {
                m_allowNewCreatures = true;
                for (auto it : m_queuedCreatures) {
                    startCombatWith(it);
                }
                m_queuedCreatures.clear();
            }
            m_allowNewCreatures = false;
            switchInitiative();
            m_instanceId = 0;
        }
    }
}

void CombatManager::doPositionRoll()
{
    // do player
    if (m_mainCreature->getHasPosition() == false) {
        m_currentState = eCombatManagerState::PositioningRoll;
        return;
    }
    m_activeInstances.clear();
    for (auto it : m_instances) {
        // do positioning roll
        it->getSide2()->doPositionRoll(m_mainCreature);
    }

    // now roll
    int mainSuccesses = DiceRoller::rollGetSuccess(
        m_mainCreature->getBTN(), m_mainCreature->getQueuedPosition().dice);
    for (unsigned i = 0; i < m_instances.size(); ++i) {
        Creature* creature = m_instances[i]->getSide2();
        int successes
            = DiceRoller::rollGetSuccess(creature->getBTN(), creature->getQueuedPosition().dice);
        if (successes >= mainSuccesses) {
            writeMessage(
                creature->getName() + " kept up with " + m_mainCreature->getName() + "'s footwork");
            m_activeInstances.push_back(i);
        }
        creature->clearCreatureManuevers();
    }
    // have to have at least one
    if (m_activeInstances.size() == 0) {
        writeMessage("No combatants kept up with footwork, initiating duel");
        m_activeInstances.push_back(0);
    } else {
        writeMessage(m_mainCreature->getName() + " is engaged with "
            + to_string(m_activeInstances.size()) + " opponents");
    }
    m_instanceId = 0;
    m_currentId = 0;
    m_currentState = eCombatManagerState::RunCombat;
    m_positionDone = true;
}

void CombatManager::startCombatWith(Creature* creature)
{
    if (m_instances.size() >= cMaxEngaged) {
        return;
    }
    // don't allow a new creature to enter in the middle of an exchange
    if (m_allowNewCreatures == false) {
        m_queuedCreatures.insert(creature);
        return;
    }

    // if a creature initiatives combat against another creature, but is not the
    // main creature then we spin up another combatmanger
    CombatInstance* instance = new CombatInstance;
    if (m_instances.size() == 1) {
        // ptr compares are gross, do ID compares
        if (creature == m_instances[0]->getSide1()) {
            m_side = eOutnumberedSide::Side1;
        } else {
            m_side = eOutnumberedSide::Side2;
        }
        m_currentId = 0;
    }
    writeMessage(
        "Combat started between " + m_mainCreature->getName() + " and " + creature->getName(),
        Log::eMessageTypes::Announcement);

    instance->initCombat(m_mainCreature, creature);
    m_instances.push_back(instance);
}

CombatInstance* CombatManager::getCurrentInstance() const
{
    assert(m_currentId < m_instances.size());
    return m_instances[m_currentId];
}

void CombatManager::refreshInstances()
{
    for (auto it : m_instances) {
        it->forceRefresh();
    }
}

void CombatManager::writeMessage(const std::string& str, Log::eMessageTypes type)
{
    // combat manager is not a singleton, so we can have multiple.
    // we can choose not to display combatmanager messages if we want to.
    Log::push(str, type);
}
