#pragma once

#include "uiobject.h"

class KeySelect : public UIObject
{
public:
	
	void run(sf::Event event) override;

private:
	int m_value;
	
};
