#pragma once

#include "object.h"

class SelectorObject : public Object {
public:
    SelectorObject();
    std::string getDescription() const override { return ""; }
    eObjectTypes getObjectType() const override { return eObjectTypes::Misc; }
};
