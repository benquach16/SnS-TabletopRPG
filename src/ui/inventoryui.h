#pragma once

#include <SFML/Graphics.hpp>

class PlayerObject;

class InventoryUI
{
public:
	InventoryUI();

	void run(sf::Event event, PlayerObject* player);
	void doBackpack(sf::Event event, PlayerObject *player);
	void doEquipped(sf::Event event, PlayerObject *player);
	void displayDetail(sf::Event event, int id);

private:
	enum eUiState
	{
		Backpack,
		Equipped,
		DetailedDescription
	};

	eUiState m_uiState;
};
