#pragma once

#include "../creatures/creature.h"

#include "gfxobject.h"

class GFXCreature
{
public:
	GFXCreature();
	~GFXCreature();

private:
	Creature* m_creature;
};
