#pragma once

#include <queue>
#include <string>
#include <SFML/Graphics.hpp>

class Log
{
public:	
	static void push(const std::string& str);
	static void run();
private:

	static std::deque<std::string> m_queue;
};
