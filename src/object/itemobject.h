#pragma once

#include <map>
#include <vector>

#include "object.h"

class ItemObject : public Object
{
public:
	ItemObject();
	void addItem(int idx, int count = 1);
	int getItemCount(int idx) { return m_inventory[idx]; }
	std::vector<int> getUniqueItemIds() const;
private:
	std::map<int, int> m_inventory;
};
