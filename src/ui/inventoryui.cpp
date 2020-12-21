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
#include "common.h"
#include "types.h"
#include "utils.h"

using namespace std;

InventoryUI::InventoryUI()
    : m_uiState(eUiState::Backpack)
    , m_id(-1)
    , m_equipped(false)
    , m_weaponType(eWeaponDetail::Primary)
{
}
constexpr int cDisplayLines = 28;

void InventoryUI::run(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    switch (m_uiState) {
    case eUiState::Backpack:
        doBackpack(hasKeyEvents, event, player);
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
    case eUiState::Paperdoll:
        doPaperdoll(hasKeyEvents, event, player);
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
    std::map<int, int> inventory = player->getInventory();
    string str = "Inventory (1 - Backpack, 2 - Wounds, 3 - Profile, 4 - Armor Coverage):\n\n";
    str += to_string(inventory.size()) + " items, ";
    str += "Equipped Armor - ";
    str += to_string(player->getCreatureComponent()->getAP()) + "AP\n";

    int count = 0;
    for (auto it = inventory.begin(); it != inventory.end();) {
        if (it->second <= 0) {
            it = inventory.erase(it);
        } else {
            char idx = ('a' + count);
            const Item* item = ItemTable::getSingleton()->get(it->first);
            str += idx;
            bool equipped = playerComponent->getNumEquipped(it->first) > 0;

            // str += " - [" + itemTypeToString(item->getItemType()) + "] ";
            str += " - " + item->getName() + " x" + to_string(it->second);
            if (equipped) {
                str += " - Equipped";
                if (playerComponent->getPrimaryWeaponId() == it->first) {
                    str += " in Right Hand";
                }
                if (playerComponent->getSecondaryWeaponId() == it->first) {
                    str += " in Left Hand";
                }
                if (playerComponent->findInQuickdraw(it->first)) {
                    str += " on belt";
                }
            }
            str += '\n';

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
            m_uiState = eUiState::Wounds;
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        case '4':
            m_uiState = eUiState::Paperdoll;
            break;
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
    str += "Selected Item (E to equip/use, W to equip in left hand, Q to add as quickdraw item, U "
           "to unequip, D "
           "to drop)\n\n";
    str += item->getName() + '\n';
    str += item->getDescription() + "\n\n";
    str += "Type: " + itemTypeToString(item->getItemType()) + '\n';
    Player* playerComponent = static_cast<Player*>(player->getCreatureComponent());
    bool equipped = playerComponent->getNumEquipped(m_id) > 0;
    if (item->getItemType() == eItemType::Armor) {
        const Armor* armor = static_cast<const Armor*>(item);
        str += "AV: " + to_string(armor->getAV()) + '\n';
        str += "AP: " + to_string(armor->getAP()) + '\n';
        str += "Covers :";
        for (auto i : armor->getCoverage()) {
            str += bodyPartToString(i) + ',';
        }
        str += '\n';

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            switch (event.text.unicode) {
            case 'e': {
                if (equipped == false) {
                    if (playerComponent->canEquipArmor(m_id)) {
                        playerComponent->equipArmor(m_id);
                        Log::push("You equip the " + item->getName());
                    } else {
                        Log::push(
                            "You cannot equip this armor, there is another piece of armor that "
                            "occupies the same spot");
                    }
                }
                break;
            }
            case 'u': {
                if (equipped == true) {
                    playerComponent->removeArmor(m_id);
                    Log::push("You unequip the " + item->getName());
                } else {
                    Log::push("This armor is not equipped!");
                }
            } break;
            }
        }
    } else if (item->getItemType() == eItemType::Weapon) {
        const Weapon* weapon = static_cast<const Weapon*>(item);
        str += "Length: " + lengthToString(weapon->getLength()) + '\n';
        str += "Proficiency: " + weaponTypeToString(weapon->getType()) + '\n';
        str += "\nWeapon Parts: \n";
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

        const Weapon* secondary = weapon->getSecondaryWeapon();
        if (secondary != nullptr) {
            str += "\n\nSecondary Weapon Part for Alternate Grip: ";
            str += secondary->getName();
            str += "\n";
            std::vector<Component*> components = secondary->getComponents();
            for (unsigned i = 0; i < components.size(); ++i) {
                str += components[i]->getName() + " - ";
                str += "Damage: " + to_string(components[i]->getDamage()) + ' '
                    + damageTypeToString(components[i]->getType()) + " [ ";
                for (auto it : components[i]->getProperties()) {
                    str += weaponPropToString(it) + ' ';
                }
                str += "]\n";
            }
        }

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            switch (event.text.unicode) {
                // allow dupes, but dont allow equipping of more weapons than actually exists
                // which is the complicated part
                // also don't allow quickdraw of certain weapons
            case 'e': {
                if (playerComponent->getNumEquipped(m_id) < player->getInventory().at(m_id)) {
                    playerComponent->setPrimaryWeapon(m_id);
                    Log::push("You equip the " + item->getName());
                } else {
                    Log::push("This weapon is already equipped!");
                }
            } break;
            case 'q': {
                if (playerComponent->getNumEquipped(m_id) < player->getInventory().at(m_id)) {
                    if (weapon->getLength() > eLength::Long) {
                        Log::push("This weapon is too long to fit on your belt");
                    } else if (weapon->getType() == eWeaponTypes::Polearms) {
                        Log::push("Polearms are too big to put on your belt");
                    } else {
                        playerComponent->addQuickdrawItem(m_id);
                        Log::push("You attach the " + item->getName() + " to your belt");
                    }
                } else {
                    Log::push("This weapon is already equipped!");
                }
            } break;
            case 'u': {
                if (equipped == true) {
                    Log::push("You unequip the " + item->getName());
                    if (playerComponent->getPrimaryWeaponId() == m_id) {
                        playerComponent->removePrimaryWeapon();
                    } else if (playerComponent->getSecondaryWeaponId() == m_id) {
                        playerComponent->removeSecondaryWeapon();
                    } else {
                        playerComponent->removeQuickdrawItem(m_id);
                    }
                } else {
                    Log::push("You cannot unequip a weapon you do not have equipped");
                }
            } break;
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

    insertLineBreaks(str);
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
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);

    Creature* creature = player->getCreatureComponent();

    sf::Text ap;
    ap.setCharacterSize(cCharSize);
    ap.setFont(Game::getDefaultFont());
    string str = "Inventory (1 - Backpack, 2 - Wounds, 3 - Profile, 4 - Armor Coverage):\n\n";
    str += "Success rate: " + to_string(creature->getSuccessRate()) + "%" + '\n'
        + "Blood loss: " + to_string(creature->getBloodLoss()) + '\n';

    str += "Thirst: " + to_string(player->getThirst()) + '\n';
    str += "Hunger: " + to_string(player->getHunger()) + '\n';
    str += "Exhaustion: " + to_string(player->getExhaustion()) + '\n';
    str += "Combat Fatigue: " + to_string(player->getFatigue()) + '\n';
    const std::unordered_map<eBodyParts, int> wounds = creature->getWounds();
    for (auto i : wounds) {
        str += "Level " + to_string(i.second) + " wound at " + bodyPartToString(i.first) + '\n';
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
    Game::getWindow().draw(ap);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            m_uiState = eUiState::Backpack;
            break;
        case '2':
            m_uiState = eUiState::Wounds;
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        }
    }
}

void InventoryUI::doProfile(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);

    Creature* creature = player->getCreatureComponent();

    sf::Text stats;
    stats.setCharacterSize(cCharSize);
    stats.setFont(Game::getDefaultFont());

    string statStr = "Inventory (1 - Backpack, 2 - Wounds, 3 - Profile, 4 - Armor Coverage):\n\n";
    statStr += "Primary Attributes\nStrength: " + to_string(creature->getStrength()) + '\n'
        + "Agility: " + to_string(creature->getAgility()) + '\n'
        + "Intuition: " + to_string(creature->getIntuition()) + '\n'
        + "Perception: " + to_string(creature->getPerception()) + '\n'
        + "Willpower: " + to_string(creature->getWillpower()) + "\n\n" + "Derived Attributes\n"
        + "Grit: " + to_string(creature->getGrit()) + '\n'
        + "Shrewdness: " + to_string(creature->getShrewdness()) + '\n'
        + "Reflex: " + to_string(creature->getReflex()) + '\n'
        + "Mobility: " + to_string(creature->getMobility()) + "\n\n";

    statStr += "Proficiencies\n";
    statStr += "Brawling: " + to_string(creature->getProficiency(eWeaponTypes::Brawling)) + '\n';
    statStr += "Polearms: " + to_string(creature->getProficiency(eWeaponTypes::Polearms)) + '\n';
    statStr += "Swords: " + to_string(creature->getProficiency(eWeaponTypes::Swords)) + '\n';
    statStr
        += "Longswords: " + to_string(creature->getProficiency(eWeaponTypes::Longswords)) + '\n';
    stats.setString(statStr);

    Game::getWindow().draw(stats);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            m_uiState = eUiState::Backpack;
            break;
        case '2':
            m_uiState = eUiState::Wounds;
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        case '4':
            m_uiState = eUiState::Paperdoll;
            break;
        }
    }
}

void InventoryUI::doPaperdoll(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);

    std::string str = "Inventory (1 - Backpack, 2 - Wounds, 3 - Profile, 4 - Armor Coverage):\n\n";
    str += UiCommon::drawPaperdoll(player->getCreatureComponent());
    sf::Text txt;
    txt.setFont(Game::getDefaultFont());
    txt.setCharacterSize(cCharSize);
    txt.setString(str);

    Game::getWindow().draw(txt);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            m_uiState = eUiState::Backpack;
            break;
        case '2':
            m_uiState = eUiState::Wounds;
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        case '4':
            m_uiState = eUiState::Paperdoll;
            break;
        }
    }
}

void InventoryUI::doLevelup(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(1);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    Game::getWindow().draw(bkg);

    std::string str = "Inventory (1 - Backpack, 2 - Wounds, 3 - Profile, 4 - Armor Coverage, 5 - "
                      "Allocate Experience):\n\n";
    str += "";
    sf::Text txt;
    txt.setFont(Game::getDefaultFont());
    txt.setCharacterSize(cCharSize);
    txt.setString(str);

    Game::getWindow().draw(txt);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case '1':
            m_uiState = eUiState::Backpack;
            break;
        case '2':
            m_uiState = eUiState::Wounds;
            break;
        case '3':
            m_uiState = eUiState::Profile;
            break;
        case '4':
            m_uiState = eUiState::Paperdoll;
            break;
        }
    }
}
