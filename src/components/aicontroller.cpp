#include "aicontroller.h"
#include "../level/level.h"
#include "../object/creatureobject.h"

AIController::AIController()
{
}

void AIController::run(const Level* level, CreatureObject* controlledCreature)
{
	std::vector<Object*> rLevelObjs = level->getObjects();
	for(int i = 0; i < rLevelObjs.size(); ++i) {
		Object* object = rLevelObjs[i];
		if(object == controlledCreature) {
			continue;
		}

		
	}
}
