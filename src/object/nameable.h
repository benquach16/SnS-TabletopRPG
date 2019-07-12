#pragma once

#include <string>

class Nameable
{
public:
	Nameable(const std::string &name) : m_name(name) {}

	std::string getName() const { return m_name; }
	
protected:
	std::string m_name;
	std::string m_description;
};
