#pragma once

#include <SFML/Graphics.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/ScrolledWindow.hpp>
#include <SFGUI/Alignment.hpp>
#include <SFGUI/Viewport.hpp>
#include <SFGUI/Adjustment.hpp>
#include <queue>
#include <string>

class Log {
public:
    enum eMessageTypes {
        Announcement,
        Standard,
        Alert,
        Damage,
        Dialogue,
        OtherDialogue,
        Background
    };
    struct message {
        std::string text;
        eMessageTypes type;
    };
	static void initialize();
    static void push(std::string str, eMessageTypes type = eMessageTypes::Standard);
    static void run();
private:
	static sfg::ScrolledWindow::Ptr m_display;
	static sfg::Box::Ptr m_windowBox;
};
