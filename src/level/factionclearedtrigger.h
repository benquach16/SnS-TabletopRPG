#pragma once

#include "object/relationmanager.h"
#include "gameeffects/gameeffect.h"
#include "trigger.h"

class FactionClearedTrigger : public Trigger {
public:
	friend class boost::serialization::access;
    // function pointer callback event because adding lua takes time
    FactionClearedTrigger(
        int timesToFire, eCreatureFaction faction, GameEffectManager::eGameEffect effect);
    bool run(Scene* scene, Level* level, Object* triggeringObject) override;
    eTriggerConditions getTriggerCondition() const override
    {
        return eTriggerConditions::FactionCleared;
    }

private:
    // for boost
	FactionClearedTrigger() : Trigger(Trigger::cPersistentTrigger) {}

	GameEffectManager::eGameEffect m_effect;
	int m_effectId;
    eCreatureFaction m_faction;

	template <class Archive> void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<Trigger>(*this);
		ar& m_faction;
		ar& m_effect;
	}
};
