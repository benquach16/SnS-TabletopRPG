#include "inventoryui.h"
#include "../creatures/player.h"
#include "../creatures/utils.h"
#include "../game.h"
#include "../items/armor.h"
#include "../items/consumable.h"
#include "../items/utils.h"
#include "../log.h"
#include "../object/creatureobject.h"
#include "../object/playerobject.h"
#include "types.h"

using namespace std;

InventoryUI::InventoryUI()
    : m_uiState(eUiState::Backpack)
    , m_id(-1)
    , m_equipped(false)
{
}
constexpr int cDisplayLines = 28;

void InventoryUI::run(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    switch (m_uiState) {
    case eUiState::Backpack:
        doBackpack(hasKeyEvents, event, player);
        break;
    case eUiState::Equipped:
        doEquipped(hasKeyEvents, event, player);
        break;
    case eUiState::Detailed:
        displayDetail(hasKeyEvents, event, player);
        break;
    case eUiState::Wounds:
        doWounds(hasKeyEvents, event, player);
        break;
    case eUiState::Profile:
        doProfile(hasKeyEvents, event, player);
        break;
    }
}

void InventoryUI::doBackpack(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);

    Player* playerComponent = static_cast<Player*>(player->getCreatureComponent());

    sf::Text txt;
    txt.setFont(Game::getDefaultFont());
    txt.setCharacterSize(cCharSize);

    string str = "Inventory (1 - Backpack, 2 - Equipment, 3 - Wounds, 4 - Profile):\n";

    std::map<int, int> inventory = player->getInventory();

    int count = 0;
    for (auto it = inventory.begin(); it != inventory.end();) {
        if (it->second <= 0) {
            it = inventory.erase(it);
        } else {
            char idx = ('a' + count);
            const Item* item = ItemTable::getSingleton()->get(it->first);
            str += idx;
            str += " - " + item->getName() + " x" + to_string(it->second) + '\n';
            count++;

            if (event.type == sf::Event::TextEntered) {
                char c = event.text.unicode;
                if (c == idx) {
                    m_id = it->first;
                    m_equipped = false;
                    m_uiState = eUiState::Detailed;
                }
            }
            it++;
        }
    }
    txt.setString(str);
    Game::getWindow().draw(txt);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            break;
        case '2':
            m_uiState = eUiState::Equipped;
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        }
    }
}

void InventoryUI::doEquipped(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape bkg(sf::Vector2f(windowSize.x / 2, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);
    sf::RectangleShape bkg2(sf::Vector2f(windowSize.x / 2, cCharSize * cDisplayLines));
    bkg2.setPosition(sf::Vector2f(windowSize.x / 2, 0));
    bkg2.setFillColor(sf::Color(12, 12, 23));
    bkg2.setOutlineThickness(1);
    bkg2.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg2);

    sf::Text txt;
    txt.setFont(Game::getDefaultFont());
    txt.setCharacterSize(cCharSize);
    string str = "Equipped Armor - ";
    str += to_string(player->getCreatureComponent()->getAP()) + "AP\n";
    std::vector<int> armorId = player->getCreatureComponent()->getArmorId();
    unsigned i = 0;
    for (i = 0; i < armorId.size(); ++i) {
        const Armor* armor = ArmorTable::getSingleton()->get(armorId[i]);
        char idx = ('a' + i);
        str += idx;
        str += " - " + armor->getName() + '\n';

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                m_id = armorId[i];
                m_equipped = true;
                m_uiState = eUiState::Detailed;
            }
        }
    }
    txt.setString(str);
    Game::getWindow().draw(txt);

    sf::Text txt2;
    txt2.setPosition(sf::Vector2f(windowSize.x / 2, 0));
    txt2.setFont(Game::getDefaultFont());
    txt2.setCharacterSize(cCharSize);

    string weapontxt = "Equipped Weapons\n";
    const Weapon* weapon = player->getCreatureComponent()->getPrimaryWeapon();
    const char idx = ('a' + i);
    weapontxt += idx;
    weapontxt += " - " + weapon->getName();
    txt2.setString(weapontxt);
    Game::getWindow().draw(txt2);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            m_uiState = eUiState::Backpack;
            break;
        case '2':
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        }
        if (c == idx) {
            m_id = player->getCreatureComponent()->getPrimaryWeaponId();
            m_uiState = eUiState::Detailed;
        }
    }
}

void InventoryUI::displayDetail(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    assert(m_id != -1);
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    Game::getWindow().draw(bkg);

    sf::Text txt;
    txt.setFont(Game::getDefaultFont());
    txt.setCharacterSize(cCharSize);

    string str;
    const Item* item = ItemTable::getSingleton()->get(m_id);
    str += "Selected Item (E to equip/use, D to drop)\n\n";
    str += item->getName() + '\n';
    str += item->getDescription() + '\n';
    str += "Type: " + itemTypeToString(item->getItemType()) + '\n';

    if (item->getItemType() == eItemType::Armor) {
        const Armor* armor = static_cast<const Armor*>(item);
        str += "AV: " + to_string(armor->getAV()) + '\n';
        str += "AP: " + to_string(armor->getAP()) + '\n';
        str += "Covers :";
        for (auto i : armor->getCoverage()) {

            str += bodyPartToString(i) + ',';
        }
        str += '\n';

        if (hasKeyEvents && event.type == sf::Event::TextEntered && event.text.unicode == 'e') {
            if (m_equipped == false) {
                if (player->getCreatureComponent()->canEquipArmor(m_id)) {
                    player->getCreatureComponent()->equipArmor(m_id);
                    player->removeItem(m_id);
                    m_uiState = eUiState::Equipped;
                    Log::push("You equip the " + item->getName());
                } else {
                    Log::push("You cannot equip this armor, there is another piece of armor that "
                              "occupies the same spot");
                }
            } else {
                player->getCreatureComponent()->removeArmor(m_id);
                player->addItem(m_id);
                m_uiState = eUiState::Equipped;
                Log::push("You unequip the " + item->getName());
            }
        }
    } else if (item->getItemType() == eItemType::Weapon) {
        const Weapon* weapon = static_cast<const Weapon*>(item);
        str += "Length: " + lengthToString(weapon->getLength()) + '\n';
        str += "Proficiency: " + weaponTypeToString(weapon->getType()) + '\n';
        str += "Weapon Parts: \n";
        std::vector<Component*> components = weapon->getComponents();
        for (unsigned i = 0; i < components.size(); ++i) {
            str += components[i]->getName() + " - ";
            str += "Damage: " + to_string(components[i]->getDamage()) + ' '
                + damageTypeToString(components[i]->getType()) + " [ ";
            for (auto it : components[i]->getProperties()) {
                str += weaponPropToString(it) + ' ';
            }
            str += "]\n";
        }

        // don't allow fists since they dont exist yet
        if (hasKeyEvents && event.type == sf::Event::TextEntered && event.text.unicode == 'e') {
            if (m_equipped == false) {
                player->addItem(player->getCreatureComponent()->getPrimaryWeaponId());
                player->getCreatureComponent()->setWeapon(m_id);
                player->removeItem(m_id);
                m_uiState = eUiState::Equipped;
                Log::push("You equip the " + item->getName());
            }
        }
    } else if (item->getItemType() == eItemType::Food) {
        if (event.type == sf::Event::TextEntered && event.text.unicode == 'e') {
            player->applyItem(m_id);
            player->removeItem(m_id);
            Log::push("You consume the " + item->getName());
        }
    }
    str += '\n';
    str += "Worth " + to_string(item->getCost()) + " silvers\n";
    txt.setString(str);

    Game::getWindow().draw(txt);

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape)) {
        m_uiState = eUiState::Backpack;
    }
}

void InventoryUI::doWounds(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x / 2, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);
    sf::RectangleShape bkg2(sf::Vector2f(windowSize.x / 2, cCharSize * cDisplayLines));
    bkg2.setPosition(sf::Vector2f(windowSize.x / 2, 0));
    bkg2.setFillColor(sf::Color(12, 12, 23));
    bkg2.setOutlineThickness(1);
    bkg2.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg2);
}

void InventoryUI::doProfile(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape bkg(sf::Vector2f(windowSize.x / 2, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);
    sf::RectangleShape bkg2(sf::Vector2f(windowSize.x / 2, cCharSize * cDisplayLines));
    bkg2.setPosition(sf::Vector2f(windowSize.x / 2, 0));
    bkg2.setFillColor(sf::Color(12, 12, 23));
    bkg2.setOutlineThickness(1);
    bkg2.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg2);

    Creature* creature = player->getCreatureComponent();

    sf::Text ap;
    ap.setCharacterSize(cCharSize);
    ap.setFont(Game::getDefaultFont());
    string str = "Success rate: " + to_string(creature->getSuccessRate()) + "%" + '\n'
        + "Blood loss: " + to_string(creature->getBloodLoss()) + '\n';

    str += "Thirst: " + to_string(player->getThirst()) + '\n';
    str += "Hunger: " + to_string(player->getHunger()) + '\n';
    str += "Exhaustion: " + to_string(player->getExhaustion()) + '\n';
    str += "Combat Fatigue: " + to_string(player->getFatigue()) + '\n';
    const std::vector<Wound*> wounds = creature->getWounds();
    for (auto i : wounds) {
        str += "Level " + to_string(i->getLevel()) + " wound at "
            + bodyPartToString(i->getLocation()) + '\n';
    }
    ap.setString(str);

    if (player->getBleeding() == true) {
        sf::Text bleeding;
        bleeding.setPosition(sf::Vector2f(ap.getLocalBounds().width, cCharSize));
        bleeding.setString("Bleeding!");
        bleeding.setCharacterSize(cCharSize);
        bleeding.setFont(Game::getDefaultFont());
        bleeding.setFillColor(sf::Color::Red);
        bleeding.setStyle(sf::Text::Bold);
        Game::getWindow().draw(bleeding);
    }

    sf::Text stats;
    stats.setCharacterSize(cCharSize);
    stats.setFont(Game::getDefaultFont());
    stats.setPosition(sf::Vector2f(windowSize.x / 2, 0));
    string statStr = "Primary Attributes\nBrawn: " + to_string(creature->getBrawn()) + '\n'
        + "Agility: " + to_string(creature->getAgility()) + '\n'
        + "Cunning: " + to_string(creature->getCunning()) + '\n' + "Perception: "
        + to_string(creature->getPerception()) + '\n' + "Will: " + to_string(creature->getWill())
        + '\n' + "Derived Attributes\n" + "Grit: " + to_string(creature->getGrit()) + '\n'
        + "Keen: " + to_string(creature->getKeen()) + '\n'
        + "Reflex: " + to_string(creature->getReflex()) + '\n'
        + "Speed: " + to_string(creature->getSpeed()) + '\n';

    statStr += "Proficiencies\n";
    statStr += "Brawling: " + to_string(creature->getProficiency(eWeaponTypes::Brawling)) + '\n';
    statStr += "Polearms: " + to_string(creature->getProficiency(eWeaponTypes::Polearms)) + '\n';
    statStr += "Swords: " + to_string(creature->getProficiency(eWeaponTypes::Swords)) + '\n';
    statStr
        += "Longswords: " + to_string(creature->getProficiency(eWeaponTypes::Longswords)) + '\n';
    stats.setString(statStr);
    Game::getWindow().draw(ap);
    Game::getWindow().draw(stats);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            m_uiState = eUiState::Backpack;
            break;
        case '2':
            m_uiState = eUiState::Equipped;
            break;
        }
    }
}
