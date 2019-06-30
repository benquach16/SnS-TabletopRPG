#pragma once

#include <queue>
#include <string>
#include <SFML/Graphics.hpp>

class Log
{
public:
	enum eMessageTypes {
		Announcement,
		Standard,
		Damage,
	};
	struct message {
		std::string text;
		eMessageTypes type;
	};
	static void push(const std::string& str, eMessageTypes type = eMessageTypes::Standard);
	static void run();
private:
	static std::deque<message> m_queue;
};
