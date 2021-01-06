#pragma once

#include "trigger.h"
#include "../object/object.h"

class ChangeLevelTrigger : public Trigger {
public:
	ChangeLevelTrigger(int timesToFire, int newLevelIdx, vector2d moveToPosition);
	bool run(Scene* scene, Level* level, Object* triggeringObject) override;
	eTriggerConditions getTriggerCondition() const override
	{
		return eTriggerConditions::ChangeLevel;
	}
protected:
	int m_newLevelIdx;
	vector2d m_moveToPosition;
};
 