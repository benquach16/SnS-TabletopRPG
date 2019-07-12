#include <fstream>
#include <iostream>
#include <assert.h>

#include "armor.h"
#include "../3rdparty/json.hpp"
#include "../creatures/utils.h"
#include "utils.h"

ArmorTable* ArmorTable::singleton = nullptr;

using namespace std;

const string filepath = "data/armor.json";

Armor::Armor(const std::string &name, int AV, int AP, eLayer layer,
			 bool rigid, bool metal, std::set<eBodyParts> coverage) :
	Nameable(name), m_AV(AV), m_AP(AP), m_rigid(rigid), m_metal(metal), m_coverage(coverage), m_layer(layer)
{
}

bool Armor::isOverlapping(const Armor* armor) const
{
	//layered armor is OK
	if(m_layer != armor->m_layer) {
		return false;
	}
	for(eBodyParts i : armor->m_coverage) {
		if(m_coverage.find(i) != m_coverage.end()) {
			return true;
		}
	}
	return false; 
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
		assert(values["AV"].is_null() == false);
		assert(values["AP"].is_null() == false);
		assert(values["rigid"].is_null() == false);
		assert(values["metal"].is_null() == false);
		assert(values["layer"].is_null() == false);
		
		string name = values["name"];
		int AV = values["AV"];
		float AP = values["AP"];
		bool rigid = values["rigid"];
		bool metal = values["metal"];
		eLayer layer = stringToArmorLayer(values["layer"]);
		
		set<eBodyParts> coverage;
		
		auto coverageJson = values["coverage"];
		for(int i = 0; i < coverageJson.size(); ++i) {
			eBodyParts part = stringToBodyPart(coverageJson[i]);
			coverage.insert(part);
		}

		Armor *armor = new Armor(name, AV, AP, layer, rigid, metal, coverage);
		m_armorList[id] = armor;
	}
}


