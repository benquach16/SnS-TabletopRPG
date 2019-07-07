#pragma once
#include "component.h"
#include <vector>
#include <string>
#include <map>

class Weapon
{
public:
	Weapon(std::string name, eLength length, std::vector<Component*> components, eWeaponTypes type);
	~Weapon();
	std::string getName() { return m_name; }
	eLength getLength() { return m_length; }
	eWeaponTypes getType() { return m_type; }
	std::vector<Component*> getComponents() { return m_components; }
	std::vector<Component*> getThrustComponents() { return m_thrustComponents; }
	std::vector<Component*> getSwingComponents() { return m_swingComponents; }
	//convinence functions
	Component* getBestAttack();
private:
	Weapon();
	std::string m_name;
	eLength m_length;
	std::vector<Component*> m_components;
	std::vector<Component*> m_thrustComponents;
	std::vector<Component*> m_swingComponents;
	eWeaponTypes m_type;
};

class WeaponTable
{
public:
	WeaponTable();
	~WeaponTable();
	static WeaponTable* getSingleton() {
		if(singleton == nullptr) {
			singleton = new WeaponTable;
		}
		return singleton;
	}
	Weapon* get(int id);
private:
	eLength convertLengthFromStr(const std::string& str);
	eWeaponTypes convertTypeFromStr(const std::string& str);
	eWeaponProperties convertPropertiesFromStr(const std::string& str);
	eDamageTypes convertDamageFromStr(const std::string& str);
	eAttacks convertAttackFromStr(const std::string& str);
	
	static WeaponTable* singleton;

	//<id, weapon>
	std::map<int, Weapon*> m_weaponsList;
};
