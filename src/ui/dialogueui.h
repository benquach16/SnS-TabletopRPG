#pragma once

#include <SFML/Graphics.hpp>

class PlayerObject;
class CreatureObject;

class DialogueUI {
public:
    void run(sf::Event event, PlayerObject* player, CreatureObject* creature);
};
