#pragma once

#include <map>

class EquipmentManager
{
public:
	static EquipmentManager* getSingleton() {
		if(singleton == nullptr) {
			singleton = new EquipmentManager;
		}
		return singleton;
	}
private:
	EquipmentManager();

	static EquipmentManager *singleton;
};
