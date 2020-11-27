#pragma once

#include <SFML/Graphics.hpp>

class PlayerObject;

class InventoryUI {
public:
    InventoryUI();

    void run(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doBackpack(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doEquipped(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doWounds(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doProfile(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void displayDetail(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doPaperdoll(bool hasKeyEvents, sf::Event event, PlayerObject* player);
    void doLevelup(bool hasKeyEvents, sf::Event event, PlayerObject* player);
private:
    enum eUiState { Backpack, Detailed, Wounds, Profile, Paperdoll, Levelup };
    enum eWeaponDetail { Primary, Secondary, Quickdraw };
    // for equipping and unequipping
    bool m_equipped;
    eWeaponDetail m_weaponType;
    eUiState m_uiState;
    int m_id;

    sf::RectangleShape m_bkg;
};
