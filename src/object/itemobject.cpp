#include "itemobject.h"

ItemObject::ItemObject()
{
}

void ItemObject::addItem(int idx, int count)
{
	m_inventory[idx] += count;
}

std::vector<int> ItemObject::getUniqueItemIds() const
{
	std::vector<int> ret;

	for(auto it: m_inventory) {
		ret.push_back(it.first);
	}

	return ret;
}
