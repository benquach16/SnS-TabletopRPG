#pragma once

#include <string>
#include <set>
#include <map>

#include "../creatures/types.h"
#include "types.h"
#include "../object/nameable.h"

struct ArmorSegment
{
	int AV = 0;
	int isMetal = false;
	int isRigid = false;
	eArmorTypes type = eArmorTypes::None;
};

class Armor : public Nameable
{
public:
	Armor(const std::string &name, int AV, int AP,
		  bool rigid, bool metal, std::set<eBodyParts> coverage);
	int getAV() const { return m_AV; }
	int getAP() const { return m_AP; }
	
	bool isRigid() const { return m_rigid; }
	bool isMetal() const { return m_metal; }

	std::set<eBodyParts> getCoverage() const { return m_coverage; }

	eArmorTypes getType() const { return m_type; }

	bool isOverlapping(const Armor* armor);
private:
	Armor();
	int m_AV;
	int m_AP;
	bool m_rigid;
	bool m_metal;
	
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
	const Armor* get(int id) { return m_armorList[id]; }
private:
	static ArmorTable* singleton;
	//<id, armor>
	std::map<int, Armor*> m_armorList; 
};
