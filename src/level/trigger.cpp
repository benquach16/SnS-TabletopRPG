#include "trigger.h"
#include "../scene.h"

Trigger::Trigger(int timesToFire) : m_timesToFire(timesToFire) {}

void Trigger::decrement()
{
	if (m_timesToFire > 0 && m_timesToFire != cPersistentTrigger) {
		m_timesToFire--;
	}
}