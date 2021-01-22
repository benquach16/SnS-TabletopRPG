#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include "items/types.h"

class Creature;

class Page {
public:
    Page();
    int run(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory,
        sf::Vector2f position, bool readOnly, Creature* creature, bool useFilter, eItemType filter);

private:
    unsigned m_begin;
    unsigned m_end;
};
