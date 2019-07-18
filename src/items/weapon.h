#pragma once
#include "component.h"
#include "item.h"

#include <vector>
#include <string>
#include <map>

class Weapon : public Item
{
public:
	Weapon(std::string name, eLength length, std::vector<Component*> components, eWeaponTypes type);
	~Weapon();
	eLength getLength() const { return m_length; }
	eWeaponTypes getType() const { return m_type; }
	std::vector<Component*> getComponents() const { return m_components; }
	std::vector<Component*> getThrustComponents() const { return m_thrustComponents; }
	std::vector<Component*> getSwingComponents() const { return m_swingComponents; }
	//convinence functions
	Component* getBestAttack() const;
private:
	Weapon();
	eLength m_length;
	std::vector<Component*> m_components;
	std::vector<Component*> m_thrustComponents;
	std::vector<Component*> m_swingComponents;
	eWeaponTypes m_type;
};

class WeaponTable
{
public:

	static const WeaponTable* getSingleton() {
		if(singleton == nullptr) {
			singleton = new WeaponTable;
		}
		return singleton;
	}
	const Weapon* get(int id) const;
private:
	WeaponTable();
	~WeaponTable();
	
	eLength convertLengthFromStr(const std::string& str);
	eWeaponTypes convertTypeFromStr(const std::string& str);
	eWeaponProperties convertPropertiesFromStr(const std::string& str);
	eDamageTypes convertDamageFromStr(const std::string& str);
	eAttacks convertAttackFromStr(const std::string& str);
	
	static WeaponTable* singleton;

	//<id, weapon>
	std::map<int, Weapon*> m_weaponsList;
};
