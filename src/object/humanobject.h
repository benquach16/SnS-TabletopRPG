#pragma once

#include "creatureobject.h"
#include "../creatures/human.h"

class HumanObject : public CreatureObject
{
public:
	HumanObject();
	~HumanObject() override;
};
