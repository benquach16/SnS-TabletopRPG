#pragma once

#include "combatui.h"
#include "createcharui.h"
#include "dialogueui.h"
#include "inventoryui.h"
#include "tradeui.h"

class CombatManager;
class PlayerObject;
class CreatureObject;

class GameUI {
public:
    GameUI();
    void initialize();
    void run();
    void initDialog(CreatureObject* creature);
    bool runDialog(
        bool hasKeyEvent, sf::Event event, PlayerObject* player, CreatureObject* creature);
    void runCombat(bool hasKeyEvents, sf::Event event, const CombatManager* manager);
    void runInventory(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void runTrade(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory,
        std::map<int, int>& container);


private:
    sf::Text m_helpText;
    sf::RectangleShape m_bkg;

    CombatUI m_combatUI;
    InventoryUI m_inventoryUI;
    TradeUI m_tradeUI;
    DialogueUI m_dialogueUI;
};
