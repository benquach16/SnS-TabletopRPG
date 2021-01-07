#pragma once

#include "trigger.h"
#include "../object/object.h"

class ChangeLevelTrigger : public Trigger {
public:
	friend class boost::serialization::access;

	ChangeLevelTrigger(int timesToFire, int newLevelIdx, vector2d moveToPosition);
	bool run(Scene* scene, Level* level, Object* triggeringObject) override;
	eTriggerConditions getTriggerCondition() const override
	{
		return eTriggerConditions::ChangeLevel;
	}
private:
	// for boost
	ChangeLevelTrigger() : Trigger(Trigger::cPersistentTrigger) {}

protected:

	int m_newLevelIdx;
	vector2d m_moveToPosition;

	template <class Archive> void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<Trigger>(*this);
		ar& m_newLevelIdx;
		ar& m_moveToPosition;
	}
};
 