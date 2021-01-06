#include "dice.h"
#include <algorithm>
#include <iostream>

using namespace std;

namespace DiceRoller {
constexpr int cMaxSuccesses = 7;
std::vector<int> roll(int number, int sides)
{
    std::vector<int> ret;
    for (int i = 0; i < number; ++i) {
        ret.push_back(rollSides(sides));
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
    count = std::min(count, cMaxSuccesses);
    cout << "successes: " << count << endl;
    return count;
}
} // namespace DiceRoller
