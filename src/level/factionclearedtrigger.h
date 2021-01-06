#pragma once

#include "trigger.h"

class FactionClearedTrigger : public Trigger {
    bool run(Scene* scene) override;
    eTriggerConditions getTriggerCondition() const override
    {
        return eTriggerConditions::FactionCleared;
    }
};
