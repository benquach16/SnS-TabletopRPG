#pragma once

#include <SFML/Graphics.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Window.hpp>

class Game;

class MainMenuUI {
public:
    MainMenuUI();
	void initialize();
    void show();
    void hide();
    void run(bool hasKeyEvents, sf::Event event);
private:
    void cleanup();
	sfg::Window::Ptr m_window;

};
