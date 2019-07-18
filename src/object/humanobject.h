#pragma once

#include "creatureobject.h"
#include "../creatures/human.h"

class HumanObject : public CreatureObject
{
public:
	HumanObject();
	~HumanObject() override;
	eCreatureRace getRace() const override { return eCreatureRace::Human; }
	std::string getDescription() const override;

private:
};
