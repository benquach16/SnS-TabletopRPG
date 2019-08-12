#include "gameui.h"
#include "../game.h"
#include "../object/creatureobject.h"
#include "types.h"

GameUI::GameUI()
{
    m_helpText.setCharacterSize(cCharSize);
    m_helpText.setFont(Game::getDefaultFont());
    m_helpText.setString("I - Inventory\nP - Pickup\nD - Look at object\nA - "
                         "Attack\nK - Talk\nR - Rest\n");
}

void GameUI::run(sf::Event event)
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

bool GameUI::runDialog(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    return m_dialogueUI.run(event, player, creature);
}

void GameUI::runCombat(sf::Event event, const CombatManager* manager)
{
    m_combatUI.run(event, manager);
}

void GameUI::runInventory(sf::Event event, PlayerObject* player)
{
    m_inventoryUI.run(event, player);
}

void GameUI::runTrade(sf::Event event, std::map<int, int>& inventory, std::map<int, int>& container)
{
    m_tradeUI.run(event, inventory, container);
}
