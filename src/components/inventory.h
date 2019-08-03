#pragma once

#include <unordered_map>

class Inventory {
public:
    Inventory();

private:
    std::unordered_map<int, int> m_inventory;
};
