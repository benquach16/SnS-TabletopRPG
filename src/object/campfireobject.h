#include "object.h"

class CampfireObject : public Object {
public:
    CampfireObject();
    ~CampfireObject() override;
    std::string getDescription() const override;
    eObjectTypes getObjectType() const override { return eObjectTypes::Usable; }
};
