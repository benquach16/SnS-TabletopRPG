#pragma once

#include "../object/object.h"
#include "gfxobject.h"
#include "gfxqueue.h"

class Level;

class GFXLevel {
public:
    GFXLevel();
    void run(const Level* level, vector2d center);

private:
    sf::Texture m_texture;
    GFXQueue m_queue;
};
