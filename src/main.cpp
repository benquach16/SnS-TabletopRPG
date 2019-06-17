#include <iostream>
#include <time.h>
#include "creatures/wound.h"

int main()
{
	srand (time(NULL));
	std::cout << (int)WoundTable::getSingleton()->getSwing(eHitLocations::Head) << std::endl;
}
