#pragma once

#include <vector>

#include "level.h"

class LevelManager {
public:
    LevelManager();
    enum eLevels { Intro, City, Dungeon };

private:
    void createIntro();
    std::vector<Level*> m_levels;
};
