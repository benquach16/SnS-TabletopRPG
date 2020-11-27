#pragma once

#include <queue>

#include "../object/object.h"
#include "gfxobject.h"
#include "gfxqueue.h"

class Level;

class GFXLevel {
public:
    GFXLevel();
    void run(const Level* level, vector2d center);
    void renderText();

private:
    sf::Texture m_texture;
    sf::Texture m_grass;
    sf::Texture m_stone;
    std::queue<sf::Text> m_texts;
    std::queue<sf::RectangleShape> m_ground;
    std::queue<sf::RectangleShape> m_top;

    GFXQueue m_queue;
};
