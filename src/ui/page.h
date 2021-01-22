#pragma once

#include <map>
#include <SFML/Graphics.hpp>

class Creature;

class Page
{
public:
    Page();
    int run(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory, sf::Vector2f position, bool readOnly, Creature* creature);

private:
    unsigned m_begin;
    unsigned m_end;
};