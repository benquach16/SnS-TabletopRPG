#pragma once

#include "trigger.h"
#include "../object/relationmanager.h"

class FactionClearedTrigger : public Trigger {
public:

	// function pointer callback event because adding lua takes time
	FactionClearedTrigger(int timesToFire, eCreatureFaction faction, void (*PFN_event)(Scene*,Level*));
    bool run(Scene* scene, Level* level, Object* triggeringObject) override;
    eTriggerConditions getTriggerCondition() const override
    {
        return eTriggerConditions::FactionCleared;
    }

private:
	// for boost
	FactionClearedTrigger();
	void(*m_PFN_event)(Scene*, Level*);
	eCreatureFaction m_faction;
};
