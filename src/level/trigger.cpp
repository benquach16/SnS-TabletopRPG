#include "trigger.h"

Trigger::Trigger(eTriggerConditions condition)
    : m_condition(condition)
{
}

bool Trigger::active() { return false; }
