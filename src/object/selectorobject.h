#pragma once

#include "object.h"

class SelectorObject : public Object {
public:
    SelectorObject();
    std::string getDescription() const override { return ""; }
    virtual eObjectTypes getObjectType() const { return eObjectTypes::Misc; }
};
