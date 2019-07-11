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
	std::string getName() const { return m_name; }
	eLength getLength() const { return m_length; }
	eWeaponTypes getType() const { return m_type; }
	std::vector<Component*> getComponents() const { return m_components; }
	std::vector<Component*> getThrustComponents() const { return m_thrustComponents; }
	std::vector<Component*> getSwingComponents() const { return m_swingComponents; }
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
