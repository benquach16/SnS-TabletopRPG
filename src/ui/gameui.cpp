#include "gameui.h"
#include "../game.h"
#include "../object/creatureobject.h"
#include "types.h"

GameUI::GameUI()
{
    m_helpText.setCharacterSize(cCharSize);
    m_helpText.setFont(Game::getDefaultFont());
    m_helpText.setString("1 - Attack\n2 - Inspect\n3 - Use\n4 - Pickup\n5 - Talk\n6 - Rest\nI - "
                         "Inventory\nPg Up - Scroll Up\nPg Dwn - Scroll Down\nArrow Keys - Movement\nWASD - Movement\n");
}

void GameUI::initialize() { m_combatUI.initialize(); }

void GameUI::run()
{
    sf::FloatRect backgroundRect = m_helpText.getLocalBounds();
    backgroundRect.width += 5;
    m_bkg.setSize(sf::Vector2f(backgroundRect.width, backgroundRect.height));
    m_bkg.setFillColor(sf::Color(50, 50, 50, 30));
    Game::getWindow().draw(m_bkg);
    Game::getWindow().draw(m_helpText);
}

void GameUI::initDialog(CreatureObject* creature)
{
    m_dialogueUI.init(creature->getStartingDialogueLabel());
}

bool GameUI::runDialog(
    bool hasKeyEvents, sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    return m_dialogueUI.run(hasKeyEvents, event, player, creature);
}

void GameUI::runCombat(bool hasKeyEvents, sf::Event event, const CombatManager* manager)
{
    m_combatUI.run(hasKeyEvents, event, manager);
}

void GameUI::runInventory(bool hasKeyEvents, sf::Event event, PlayerObject* player)
{
    m_inventoryUI.run(hasKeyEvents, event, player);
}

void GameUI::runTrade(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory,
    std::map<int, int>& container)
{
    m_tradeUI.run(hasKeyEvents, event, inventory, container);
}
