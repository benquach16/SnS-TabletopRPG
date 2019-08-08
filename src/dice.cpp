#include "dice.h"
#include <iostream>
using namespace std;

namespace DiceRoller {
std::vector<int> roll(int number)
{
    std::vector<int> ret;
    for (int i = 0; i < number; ++i) {
        ret.push_back(roll());
    }
    return ret;
}

int rollGetSuccess(int BTN, int number)
{
    int count = 0;
    cout << "roll (btn=" << BTN << "):";
    for (int i = 0; i < number; ++i) {
        int r = roll();
        cout << r << " ";
        if (r >= BTN) {
            count++;
        }
    }
    cout << endl;
    return count;
}
} // namespace DiceRoller
