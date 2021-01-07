#pragma once

#include "../object/relationmanager.h"
#include "trigger.h"

class FactionClearedTrigger : public Trigger {
public:
	friend class boost::serialization::access;
    // function pointer callback event because adding lua takes time
    FactionClearedTrigger(
        int timesToFire, eCreatureFaction faction, void (*PFN_event)(Scene*, Level*));
    bool run(Scene* scene, Level* level, Object* triggeringObject) override;
    eTriggerConditions getTriggerCondition() const override
    {
        return eTriggerConditions::FactionCleared;
    }

private:
    // for boost
	FactionClearedTrigger() : Trigger(Trigger::cPersistentTrigger) {}

    // TODO: instead of using a function pointer, point to a table of effects. We can't serialize
    // functions
    void (*m_PFN_event)(Scene*, Level*);
	int m_effectId;
    eCreatureFaction m_faction;

	template <class Archive> void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<Trigger>(*this);
		ar& m_faction;
	}
};
