#include "utils.h"
#include "../game.h"
#include "types.h"

void insertLineBreaks(std::string& str)
{
    auto windowSize = Game::getWindow().getSize();
    // unsigned width = static_cast<unsigned>(str.size() * cCharWidth);

    unsigned start = 0;
    for (unsigned i = start; i < str.size(); ++i) {
        if (str[i] == '\n') {
            start = i + 1;
            continue;
        }
        unsigned width = static_cast<unsigned>((i - start) * cCharWidth);
        if (width > windowSize.x) {
            start = i + 1;
            str.insert(i, 1, '\n');
        }
    }
}
