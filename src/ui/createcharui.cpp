#include <fstream>

#include "../3rdparty/json.hpp"
#include "createcharui.h"

using namespace std;

const string filepath = "data/starting.json";

CreateCharUI::CreateCharUI()
{
    ifstream file(filepath);

    nlohmann::json parsedLoadouts;
    file >> parsedLoadouts;

    for(auto& iter : parsedLoadouts.items()) {
        string key = iter.key();
    }
    
}

void CreateCharUI::run() {}

void CreateCharUI::doName() {}
