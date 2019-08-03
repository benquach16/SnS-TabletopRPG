#include "tradeui.h"
#include "../game.h"
#include "types.h"

using namespace std;

constexpr int cDisplayLines = 28;

TradeUI::TradeUI()
    : m_uiState(eUiState::Inventory)
{
}

void TradeUI::run(sf::Event event, std::map<int, int>& inventory, std::map<int, int>& container)
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
    inventoryTxt.setFont(Game::getDefaultFont());
    inventoryTxt.setCharacterSize(cCharSize);

    string inventoryStr;
    inventoryStr += "Your inventory\n";
    int count = 0;
    for (auto it = inventory.begin(); it != inventory.end();) {
        if (it->second == 0) {
            it = inventory.erase(it);
        } else {
            const Item* item = ItemTable::getSingleton()->get(it->first);
            if (m_uiState == eUiState::Inventory) {
                char idx = ('a' + count);
                inventoryStr += idx;
                inventoryStr += " - ";
                count++;
                bool remove = false;
                //fun nesting
                if (event.type == sf::Event::TextEntered) {
                    char c = event.text.unicode;
                    if (c == idx) {
                        inventory[it->first]--;
                        container[it->first]++;
                    }
                }
            }
            inventoryStr += item->getName() + " x" + to_string(it->second) + '\n';
            it++;
        }
    }

    inventoryTxt.setString(inventoryStr);

    Game::getWindow().draw(inventoryTxt);

    sf::Text containerTxt;
    containerTxt.setFont(Game::getDefaultFont());
    containerTxt.setCharacterSize(cCharSize);
    containerTxt.setPosition(sf::Vector2f(windowSize.x / 2, 0));

    string containerStr;
    containerStr += "Container inventory\n";
    for (auto it = container.begin(); it != container.end();) {
        if (it->second == 0) {
            it = container.erase(it);
        } else {
            char idx = ('a' + count);
            const Item* item = ItemTable::getSingleton()->get(it->first);
            if (m_uiState == eUiState::Container) {
                char idx = ('a' + count);
                containerStr += idx;
                containerStr += " - ";
                count++;

                if (event.type == sf::Event::TextEntered) {
                    char c = event.text.unicode;
                    if (c == idx) {
                        inventory[it->first]++;
                        container[it->first]--;
                    }
                }
            }
            containerStr += item->getName() + " x" + to_string(it->second) + '\n';
            it++;
        }
    }

    containerTxt.setString(containerStr);

    Game::getWindow().draw(containerTxt);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c == '1') {
            m_uiState = eUiState::Inventory;
        }
        if (c == '2') {
            m_uiState = eUiState::Container;
        }
    }
}

void TradeUI::displayContainer(sf::Event, std::map<int, int>& inventory, std::string& str)
{
}
