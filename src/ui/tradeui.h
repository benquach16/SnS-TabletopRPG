#pragma once

#include <SFML/Graphics.hpp>

#include <map>

class TradeUI
{
public:
	TradeUI();
	void run(sf::Event event, std::map<int, int>& inventory, std::map<int, int>& container);
	void doInventory();
	void doContainer();
private:
	enum eUiState {
		Inventory,
		Container
	};

	eUiState m_uiState;
};
