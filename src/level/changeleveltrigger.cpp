#include "changeleveltrigger.h"
#include "../scene.h"

BOOST_CLASS_EXPORT(ChangeLevelTrigger)

ChangeLevelTrigger::ChangeLevelTrigger(int timesToFire, int newLevelIdx, vector2d moveToPosition)
    : Trigger(timesToFire)
    , m_newLevelIdx(newLevelIdx)
    , m_moveToPosition(moveToPosition)
{
}

bool ChangeLevelTrigger::run(Scene* scene, Level* level, Object* triggeringObject)
{
    assert(triggeringObject != nullptr);
    scene->changeToLevel(m_newLevelIdx, triggeringObject, m_moveToPosition.x, m_moveToPosition.y);

    decrement();
    if (m_timesToFire == 0) {
        return false;
    }

    return true;
}
