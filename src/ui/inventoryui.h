#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include "page.h"

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
    void drawBkg();
    enum eUiState { Backpack, Detailed, Wounds, Profile, Paperdoll, Levelup };
    enum eWeaponDetail { Primary, Secondary, Quickdraw };
    // for equipping and unequipping
    bool m_equipped;
    eWeaponDetail m_weaponType;
    eUiState m_uiState;
    int m_id;
    unsigned m_beginCount;
    unsigned m_endCount;
    std::map<int, int> m_lowIterator;
    std::map<int, int> m_highIterator;
    sf::RectangleShape m_bkg;

    int m_currentPageIdx;
    static constexpr int cNumPages = 3;
    std::array<Page, cNumPages> m_pages;
};
