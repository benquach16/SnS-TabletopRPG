#include "game.h"
#include <iostream>
#include <time.h>

#include <SFML/Graphics.hpp>

int main()
{
    Game game;
    game.initialize();
    game.run();
}
