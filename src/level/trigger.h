#pragma once

class Scene;

class Trigger {
public:
    enum eTriggerConditions { FactionCleared, ItemInInventory, CreatureCleared, ChangeLevel };
    Trigger();
    virtual bool run(Scene* scene) = 0;

    virtual eTriggerConditions getTriggerCondition() const = 0;

private:
    bool m_persistent;
    eTriggerConditions m_condition;
};
