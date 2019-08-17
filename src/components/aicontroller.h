#pragma once

enum eAIRoles {
    Standing,
    Patrolling,
};

class Level;
class CreatureObject;
struct vector2d;

class AIController {
public:
    AIController();
    void run(const Level* level, CreatureObject* controlledCreature);
    void patrol(const Level* level, CreatureObject* controlledCreature);
    void moveToward(vector2d target, const Level* level, CreatureObject* controlledCreature);
    int getDistance(vector2d position1, vector2d position2);

    void setRole(eAIRoles role) { m_role = role; }

private:
    eAIRoles m_role;
};
