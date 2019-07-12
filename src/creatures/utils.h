#pragma once

#include <iostream>
#include <string>

#include "types.h"

std::string bodyPartToString(eBodyParts part);

eBodyParts stringToBodyPart(const std::string& str);
