#pragma once

class Scene;

class Trigger {
public:
    enum eTriggerConditions { factionCleared, itemInInventory, creatureCleared, changeLevel };
    Trigger();
    virtual bool run(Scene* scene) = 0;

	virtual eTriggerConditions getTriggerCondition() = 0;

private:

    eTriggerConditions m_condition;
};
