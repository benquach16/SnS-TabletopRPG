#include "tradeui.h"
#include "common.h"
#include "game.h"
#include "types.h"

using namespace std;

constexpr int cDisplayLines = 28;

TradeUI::TradeUI()
    : m_uiState(eUiState::Inventory)
{
}

void TradeUI::run(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory,
    std::map<int, int>& container)
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

    sf::Text inventoryTxt;
    UiCommon::initializeText(inventoryTxt);
    inventoryTxt.setString("1 - Your inventory\n");
    string inventoryStr;
    inventoryStr += "1 - Your inventory\n";
    inventoryTxt.setString(inventoryStr);
    Game::getWindow().draw(inventoryTxt);
    int id = m_inventoryPage.run(hasKeyEvents, event, inventory, sf::Vector2f(0, cCharSize * 2),
        m_uiState != eUiState::Inventory, nullptr, false, eItemType::Armor);

    if (id != -1) {
        inventory[id]--;
        container[id]++;
    }

    sf::Text containerTxt;
    UiCommon::initializeText(containerTxt);
    containerTxt.setPosition(sf::Vector2f(windowSize.x / 2, 0));

    id = m_containerPage.run(hasKeyEvents, event, container, sf::Vector2f(windowSize.x / 2, cCharSize * 2),
        m_uiState != eUiState::Container, nullptr, false, eItemType::Armor);

    if (id != -1) {
        inventory[id]++;
        container[id]--;
    }
    containerTxt.setString("2 - Container inventory\n");

    Game::getWindow().draw(containerTxt);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c == '1') {
            m_uiState = eUiState::Inventory;
        }
        if (c == '2') {
            m_uiState = eUiState::Container;
        }
    }
}

void TradeUI::displayContainer(
    bool hasKeyEvents, sf::Event, std::map<int, int>& inventory, std::string& str)
{
}
