#include "gameeffect.h"
#include "3rdparty/random.hpp"
#include "level/level.h"
#include "object/humanobject.h"
#include "scene.h"

GameEffectManager* GameEffectManager::singleton = nullptr;

using namespace effolkronium;
using namespace std;

GameEffectManager::GameEffectManager()
{
    m_effects[eGameEffect::Arena] = [](Scene* scene, Level* level) {
        HumanObject* object = new HumanObject;
        object->setPosition(5, 5);
        object->setFaction(eCreatureFaction::ArenaFighter);
        // temporary
        object->setLoadout(eCreatureFaction::Confederacy,
            static_cast<eRank>(random_static::get((int)eRank::Recruit, (int)eRank::Count - 1)));
        object->getCreatureComponent()->setAgility(random_static::get(5, 9));
        object->getCreatureComponent()->setIntuition(random_static::get(5, 9));
        object->getCreatureComponent()->setProficiency(
            eWeaponTypes::Polearms, random_static::get(8, 12));
        object->getCreatureComponent()->setProficiency(
            eWeaponTypes::Brawling, random_static::get(8, 12));
        object->getCreatureComponent()->setProficiency(
            eWeaponTypes::Longswords, random_static::get(8, 12));
        object->getCreatureComponent()->setProficiency(
            eWeaponTypes::Swords, random_static::get(8, 12));
        object->getCreatureComponent()->setProficiency(
            eWeaponTypes::Mass, random_static::get(8, 12));
        level->addObject(object);
    };
}
