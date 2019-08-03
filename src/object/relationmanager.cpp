#include "relationmanager.h"

using namespace std;

RelationManager* RelationManager::singleton = nullptr;

RelationManager::RelationManager()
{
    setRelationship(eCreatureFaction::Player, eCreatureFaction::Bandit, cHostile);
}

int RelationManager::getRelationship(eCreatureFaction faction1, eCreatureFaction faction2) const
{
    if (faction1 == faction2) {
        return cFriends;
    }
    return m_relations.at(factionPair(faction1, faction2));
}

void RelationManager::setRelationship(eCreatureFaction faction1, eCreatureFaction faction2,
    int value)
{
    // do it both ways since there is no unique pair class
    m_relations[factionPair(faction1, faction2)] = value;
    m_relations[factionPair(faction2, faction1)] = value;
}

void RelationManager::setHateAll(eCreatureFaction faction)
{
    for (int i = 0; i < static_cast<int>(eCreatureFaction::Count); ++i) {
        eCreatureFaction faction2 = static_cast<eCreatureFaction>(i);
        if (faction != faction2) {
            setRelationship(faction, faction2, cHostile);
        }
    }
}
