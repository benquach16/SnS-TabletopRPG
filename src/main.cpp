#include "game.h"
#include <iostream>
#include <time.h>

#include <SFML/Graphics.hpp>

int main()
{
    // initialize singletons
    WeaponTable::getSingleton();
    ArmorTable::getSingleton();
    WoundTable::getSingleton();
    ItemTable::getSingleton();

    Game game;
    game.initialize();
    game.run();

    WoundTable::cleanupSingleton();
    // item table is a superset of armor and weapons
    ItemTable::cleanupSingleton();
}
