#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Creature;
namespace UiCommon {
void drawTopPanel();
void initializeText(sf::Text& text);
std::string drawPaperdoll(const Creature* creature);
}
