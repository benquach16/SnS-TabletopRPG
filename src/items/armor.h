#pragma once

#include <string>
#include <set>
#include <map>

#include "../creatures/types.h"
#include "types.h"

struct ArmorSegment
{
	int AV = 0;
	int isMetal = false;
	int isRigid = false;
	eArmorTypes type = eArmorTypes::None;
};

class Armor
{
public:

	bool isRigid() const;
	bool isMetal() const;

	std::string getName() const;
	std::set<eBodyParts> getCoverage() const;

	bool isOverlapping(const Armor* armor);
private:
	int m_AV;
	int m_AP;
	std::set<eBodyParts> m_coverage;

	eLayer m_layer;
	eArmorTypes m_type;
};


class ArmorTable
{
public:
	ArmorTable();
	static ArmorTable* getSingleton() {
		if(singleton == nullptr) {
			singleton = new ArmorTable;
		}
		return singleton;
	}
	Armor* get(int id);
private:
	static ArmorTable* singleton;
	//<id, armor>
	std::map<int, Armor*> m_armorList; 
};
