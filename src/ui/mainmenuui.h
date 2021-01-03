#pragma once

#include <SFML/Graphics.hpp>

class Game;

class MainMenuUI {
public:
    MainMenuUI();
    void run(bool hasKeyEvents, sf::Event event);
};
