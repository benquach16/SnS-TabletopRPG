#pragma once

#include <unordered_map>
#include <vector>

#include "combatmanager.h"

class CombatManagerFactory {
public:
    CombatManager* get(int id);

private:
    std::vector<CombatManager*> m_managers;
};
