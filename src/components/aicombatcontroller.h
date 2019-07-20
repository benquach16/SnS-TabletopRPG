#pragma once

enum eAICombatProfiles {
	Aggressive,
	Cautious,
	Neutral,
};

class AICombatController
{
public:
	void doOffense();
	void doDefense();
};
