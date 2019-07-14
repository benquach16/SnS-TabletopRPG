#pragma once

#include "../creatures/creature.h"

#include "gfxobject.h"

class GFXCreature : public GFXObject
{
public:
	GFXCreature();
	~GFXCreature();
	void run() override;
private:
	Creature* m_creature;
};
