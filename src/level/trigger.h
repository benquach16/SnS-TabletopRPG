#pragma once

class Trigger {
public:
    enum eTriggerConditions { factionCleared, itemInInventory, creatureCleared };
    Trigger(eTriggerConditions condition);
    bool active();

private:
    Trigger();

    eTriggerConditions m_condition;
};
