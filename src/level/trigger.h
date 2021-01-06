#pragma once

class Scene;
class Level;
class Object;

class Trigger {
public:
	static const int cPersistentTrigger = -1;
    enum eTriggerConditions { FactionCleared, ItemInInventory, CreatureCleared, ChangeLevel };
    Trigger(int timesToFire);
	// triggers might need a ton of state
    virtual bool run(Scene* scene, Level* level, Object* triggeringObject) = 0;

    virtual eTriggerConditions getTriggerCondition() const = 0;
	void decrement();
private:
	// for boost
	Trigger();

protected:
	int m_timesToFire;
};
