#pragma once

#include "creature.h"

class Human : public Creature
{
public:
	Human();
	eCreatureType getCreatureType() override { return eCreatureType::Human; }
private:
	
};
