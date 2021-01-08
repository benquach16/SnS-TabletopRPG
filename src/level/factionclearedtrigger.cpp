#include "factionclearedtrigger.h"
#include "../object/creatureobject.h"
#include "level.h"

BOOST_CLASS_EXPORT(FactionClearedTrigger)

FactionClearedTrigger::FactionClearedTrigger(
    int timesToFire, eCreatureFaction faction, GameEffectManager::eGameEffect effect)
    : Trigger(timesToFire)
    , m_faction(faction)
    , m_effect(effect)
{
}

bool FactionClearedTrigger::run(Scene* scene, Level* level, Object* triggeringObject)
{
    const std::vector<Object*> objects = level->getObjects();
    int count = 0;
    for (unsigned i = 0; i < objects.size(); ++i) {
        Object* object = objects[i];
        if (object->getObjectType() == eObjectTypes::Creature) {
            CreatureObject* creature = static_cast<CreatureObject*>(object);
            if (creature->getFaction() == m_faction) {
                count++;
            }
        }
    }

    // do event
    if (count == 0) {
        // call function pointer
        GameEffectManager::getSingleton()->execute(m_effect, scene, level);
        decrement();
        if (m_timesToFire == 0) {
            return false;
        }
    }
    return true;
}
