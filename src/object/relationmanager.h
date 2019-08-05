#pragma once

#include <map>

#include "types.h"

class RelationManager {
public:
    static constexpr int cHostile = -10;
    static constexpr int cFriends = 20;

    static RelationManager* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new RelationManager;
        }
        return singleton;
    }

    int getRelationship(eCreatureFaction faction1, eCreatureFaction faction2) const;
    void setRelationship(eCreatureFaction faction1, eCreatureFaction faction2, int value);
    void setHateAll(eCreatureFaction faction);

private:
    RelationManager();
    static RelationManager* singleton;

    typedef std::pair<eCreatureFaction, eCreatureFaction> factionPair;

    std::map<std::pair<eCreatureFaction, eCreatureFaction>, int> m_relations;
};
