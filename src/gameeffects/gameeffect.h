#pragma once

#include <unordered_map>

class Scene;
class Level;

class GameEffectManager {
public:
    enum class eGameEffect { Arena };
    GameEffectManager();
    static const GameEffectManager* getSingleton()
    {
        if (singleton == nullptr) {
            singleton = new GameEffectManager;
        }
        return singleton;
    }
    void execute(eGameEffect effect, Scene* scene, Level* level) const
    {
        m_effects.at(effect)(scene, level);
    }

private:
    static GameEffectManager* singleton;
    std::unordered_map<eGameEffect, void (*)(Scene*, Level*)> m_effects;
};
