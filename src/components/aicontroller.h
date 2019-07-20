#pragma once

enum eAIRoles {
	Standing,
	Patrolling,
};

class Level;
class CreatureObject;

class AIController
{
public:
	AIController();
	void run(const Level* level, CreatureObject* controlledCreature);

private:
	eAIRoles m_role;
};
