#pragma once

#include <SFML/Graphics.hpp>

#include <map>
#include <string>

class TradeUI {
public:
    TradeUI();
    void run(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory,
        std::map<int, int>& container);
    void doInventory();
    void doContainer();

private:
    void displayContainer(
        bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory, std::string& str);

    enum eUiState { Inventory, Container };

    eUiState m_uiState;
};
