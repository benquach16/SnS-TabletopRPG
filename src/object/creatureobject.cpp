#include "creatureobject.h"
#include "../combatmanager.h"
#include "../items/consumable.h"
#include "equipmentmanager.h"

using namespace std;

static CreatureObject::CreatureObjectId ids = static_cast<CreatureObject::CreatureObjectId>(0);

CreatureObject::CreatureObject(Creature* creature)
    : m_creature(creature)
    , m_creatureFaction(eCreatureFaction::None)
    , m_thirst(0)
    , m_hunger(0)
    , m_exhaustion(0)
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
    if (m_manager->inCombat()) {
        if (m_manager->isParent()) {
            // definitely engaged, no need to run ai controller
            m_manager->run(CombatManager::cTick + 1);
        }
        if (isPlayer() == false) {
            // ai combat controller should take over
            m_combatController.run(m_manager, m_creature);
        }
    } else {
        m_controller.run(level, this);
    }
}

void CreatureObject::applyItem(int id)
{
    const Item* item = ItemTable::getSingleton()->get(id);
    assert(item->getItemType() != eItemType::Weapon);
    assert(item->getItemType() != eItemType::Armor);
    const Consumable* consumable = static_cast<const Consumable*>(item);

    for (auto it : consumable->getEffects()) {
        switch (it->getType()) {
        case eItemEffect::Thirst: {
            m_thirst = max(0, m_thirst - it->getValue());
            break;
        }
        case eItemEffect::Hunger:
            m_hunger = max(0, m_hunger - it->getValue());
            break;
        default:
            break;
        }
    }
}

int CreatureObject::getFatigue() const { return m_creature->getFatigue(); }

bool CreatureObject::isInCombat() const { return m_manager->isEngaged(); }

void CreatureObject::startCombatWith(const CreatureObject* creature)
{
    m_manager->startCombatWith(creature);
}

void CreatureObject::setLoadout(eCreatureFaction faction, eRank rank)
{
    int weaponId = EquipmentManager::getSingleton()->getRandomWeapon(faction, rank);
    m_creature->setPrimaryWeapon(weaponId);
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
