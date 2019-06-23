#include <iostream>
#include <time.h>
#include "creatures/wound.h"
#include "combatmanager.h"
#include "weapons/weapon.h"
#include "creatures/human.h"

int main()
{
	srand (time(NULL));
	//WeaponTable table;
	CombatManager manager;
	Human* c1 = new Human;
	Human* c2 = new Human;
	c1->setWeapon(40); //pollax
	c2->setWeapon(41); //arming sword

	manager.setSide1(c1);
	manager.setSide2(c2);
	manager.run();
}
