#include <fstream>
#include <iostream>
#include <assert.h>

#include "armor.h"
#include "../3rdparty/json.hpp"

ArmorTable* ArmorTable::singleton = nullptr;

using namespace std;

const string filepath = "data/armor.json";

bool Armor::isOverlapping(const Armor* armor)
{
	//layered armor is OK
	if(m_layer != armor->m_layer) {
		return false;
	}
	for(auto i : armor->m_coverage) {
		if(m_coverage.find(i) != m_coverage.end()) {
			return false;
		}
	}
	return true; 
}

ArmorTable::ArmorTable()
{
	ifstream file(filepath);

	nlohmann::json parsedArmor;
	
	file >> parsedArmor;

	for(auto &iter : parsedArmor.items()) {
		int id = std::stoi(iter.key());
		auto values = iter.value();

		//asert valid json
		assert(values["name"].is_null() == false);
		assert(values["coverage"].is_null() == false);
		
		string name = values["name"];
		int AV = values["AV"];
		float AP = values["AP"];
		bool rigid = values["rigid"];
		bool metal = values["metal"];

		auto coverage = values["coverage"];
		for(int i = 0; i < coverage.size(); ++i) {
			
		}
	}
}


