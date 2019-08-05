#include "gameui.h"
#include "../game.h"
#include "types.h"

GameUI::GameUI() {}

void GameUI::run(sf::Event event)
{
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("I - Inventory\nP - Pickup\nD - Look at object\nA - "
                   "Attack\nT - Talk\nR - Rest\n");
    sf::FloatRect backgroundRect = text.getLocalBounds();
    backgroundRect.width += 5;
    sf::RectangleShape background(sf::Vector2f(backgroundRect.width, backgroundRect.height));
    background.setFillColor(sf::Color(50, 50, 50, 30));
    Game::getWindow().draw(background);
    Game::getWindow().draw(text);
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
