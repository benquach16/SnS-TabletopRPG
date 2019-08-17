#include "aicontroller.h"
#include "../3rdparty/random.hpp"
#include "../level/level.h"
#include "../object/creatureobject.h"
#include "../object/playerobject.h"
#include "../object/relationmanager.h"

AIController::AIController()
    : m_role(eAIRoles::Patrolling)
{
}

void AIController::run(const Level* level, CreatureObject* controlledCreature)
{
    // dont run world ai for creatures in combat or are dead
    if (controlledCreature->isInCombat() == true || controlledCreature->isConscious() == false)
        return;

    std::vector<Object*> rLevelObjs = level->getObjects();
    constexpr int cMinDist = 64;
    int minDist = cMinDist;
    CreatureObject* target = nullptr;
    for (int i = 0; i < rLevelObjs.size(); ++i) {
        Object* object = rLevelObjs[i];
        if (object == controlledCreature) {
            continue;
        }

        if (object->getObjectType() == eObjectTypes::Creature) {
            CreatureObject* creature = static_cast<CreatureObject*>(object);
            int relationship = RelationManager::getSingleton()->getRelationship(
                controlledCreature->getFaction(), creature->getFaction());

            // dont attack dead people
            if (relationship <= RelationManager::cHostile && creature->isConscious() == true) {
                // get distance
                int dist = getDistance(object->getPosition(), controlledCreature->getPosition());
                if (dist < minDist) {
                    minDist = dist;
                    target = creature;
                }
            }
        }
    }

    if (target != nullptr) {
        if (minDist > 2) {
            moveToward(target->getPosition(), level, controlledCreature);
        } else {
            controlledCreature->startCombatWith(target);
        }
    } else {
        switch (m_role) {
        case eAIRoles::Standing:
            break;
        case eAIRoles::Patrolling:
            patrol(level, controlledCreature);
            break;
        }
    }
}

void AIController::patrol(const Level* level, CreatureObject* controlledCreature)
{
    vector2d position = controlledCreature->getPosition();
    int rand = effolkronium::random_static::get(1, 4);
    if (rand == 1) {
        if (level->isFreeSpace(position.x, position.y - 1)) {
            controlledCreature->moveUp();
        }
    }

    if (rand == 2) {
        if (level->isFreeSpace(position.x, position.y + 1)) {
            controlledCreature->moveDown();
        }
    }

    if (rand == 3) {
        if (level->isFreeSpace(position.x - 1, position.y)) {
            controlledCreature->moveLeft();
        }
    }

    if (rand == 4) {
        if (level->isFreeSpace(position.x + 1, position.y)) {
            controlledCreature->moveRight();
        }
    }
}

int AIController::getDistance(vector2d position1, vector2d position2)
{
    int ret = 0;
    int a = position1.x - position2.x;
    int b = position1.y - position2.y;

    ret = (a * a) + (b * b);
    return ret;
}

void AIController::moveToward(
    vector2d target, const Level* level, CreatureObject* controlledCreature)
{
    vector2d position = controlledCreature->getPosition();
    int diffX = position.x - target.x;
    int diffY = position.y - target.y;

    int rand = effolkronium::random_static::get(1, 2);

    if (rand == 1) {
        if (target.x < position.x) {
            if (level->isFreeSpace(position.x - 1, position.y) == true) {
                controlledCreature->moveLeft();
            }
        } else if (target.x > position.x) {
            if (level->isFreeSpace(position.x + 1, position.y) == true) {
                controlledCreature->moveRight();
            }
        }
    } else {
        if (target.y < position.y) {
            if (level->isFreeSpace(position.x, position.y - 1) == true) {
                controlledCreature->moveUp();
            }
        } else if (target.y > position.y) {
            if (level->isFreeSpace(position.x, position.y + 1) == true) {
                controlledCreature->moveDown();
            }
        }
    }
}
