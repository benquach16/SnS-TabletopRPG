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

    Game::getSingleton()->initialize();
    Game::getSingleton()->run();

    WoundTable::cleanupSingleton();
    // item table is a superset of armor and weapons
    ItemTable::cleanupSingleton();
}
