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
	void doProfile(sf::Event event, PlayerObject* player);
	void displayDetail(sf::Event event, PlayerObject* player);

private:
	enum eUiState
	{
		Backpack,
		Equipped,
		Detailed,
		Profile
	};

	eUiState m_uiState;
	int m_id;
};
