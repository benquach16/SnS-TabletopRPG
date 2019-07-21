#pragma once

#include <SFML/Graphics.hpp>

class PlayerObject;

class InventoryUI
{
public:
	InventoryUI();

	void run(sf::Event event, PlayerObject* player);
private:
	enum eUiState
	{
		View,
		DetailedDescription
	};

	eUiState m_uiState;
};
