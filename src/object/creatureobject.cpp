#include "creatureobject.h"
#include "../combatmanager.h"
#include "../items/consumable.h"
#include "equipmentmanager.h"

using namespace std;

static CreatureObject::CreatureObjectId ids = static_cast<CreatureObject::CreatureObjectId>(0);

BOOST_CLASS_EXPORT(CreatureObject)

CreatureObject::CreatureObject(Creature* creature)
    : m_creature(creature)
    , m_creatureFaction(eCreatureFaction::Civilian)
    , m_experience(0)
    , m_inCombat(false)
    , m_manager(new CombatManager(this))
    , m_delete(false)
    , m_dialogue("greeting_neutral")
    , m_id(ids++)
{
}

CreatureObject::~CreatureObject()
{
    if (m_creature != nullptr) {
        delete m_creature;
        m_creature = nullptr;
    }
    if (m_manager != nullptr) {
        delete m_manager;
        m_manager = nullptr;
    }
}

void CreatureObject::run(const Level* level)
{
    if (m_creature->getCreatureState() == eCreatureState::Dead && m_manager->isEngaged() == false) {
        m_delete = true;
    }
    if (isInCombat()) {
        if (m_manager->isParent()) {
            // definitely engaged, no need to run ai controller
            m_manager->run(CombatManager::cTick + 1);
        }
        // ai combat controller should take over
        m_combatController.run(m_manager, m_creature);

    } else {
        // m_creature->clearCreatureManuevers();
        m_controller.run(level, this);
    }
}

int CreatureObject::getFatigue() const { return m_creature->getFatigue(eCreatureFatigue::Stamina); }

bool CreatureObject::isInCombat() const { return m_manager->isEngaged(); }

void CreatureObject::startCombatWith(const CreatureObject* creature)
{
    m_manager->startCombatWith(creature);
}

void CreatureObject::setLoadout(eCreatureFaction faction, eRank rank)
{
    int weaponId = EquipmentManager::getSingleton()->getRandomWeapon(faction, rank);
    m_creature->setPrimaryWeapon(weaponId);

    int secondaryId = EquipmentManager::getSingleton()->getRandomSecondary(faction, rank);

    if (secondaryId != -1 && m_creature->canEquip(secondaryId)) {
        m_creature->setSecondaryWeapon(secondaryId);
    }

    std::vector<int> armor = EquipmentManager::getSingleton()->getRandomArmors(faction, rank);
    for (auto i : armor) {
        if (m_creature->canEquipArmor(i)) {
            m_creature->equipArmor(i);
        } else {
            cout << "Failed to equip " << i << endl;
        }
        m_inventory[i]++;
    }

    m_inventory[0] = effolkronium::random_static::get(1, 25);
    m_inventory[weaponId] = 1;

    m_inventory[5] = 3;
}
