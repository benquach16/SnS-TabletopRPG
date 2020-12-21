#pragma once

#include <map>
#include <vector>

#include "object.h"

class ItemObject : public Object {
public:
    friend class boost::serialization::access;
    ItemObject();
    void addItem(int idx, int count = 1);
    int getItemCount(int idx) { return m_inventory[idx]; }
    std::vector<int> getUniqueItemIds() const;

private:
    std::map<int, int> m_inventory;
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<Object>(*this);
    }
};
