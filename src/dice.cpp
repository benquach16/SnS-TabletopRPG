#include "dice.h"
#include <time.h>


namespace DiceRoller {

	std::vector<int> roll(int number)
	{
		std::vector<int> ret;
		for(int i = 0; i < number; ++i) {
			ret.push_back(roll());
		}
		return ret;
	}

	int rollGetSuccess(int BTN, int number)
	{
		int count = 0;
		for(int i = 0; i < number; ++i) {
			if(roll() >= BTN) {
				count++;
			}
		}
		return count;
	}

};
