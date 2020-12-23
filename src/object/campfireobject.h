#include "object.h"

class CampfireObject : public Object {
public:
    friend class boost::serialization::access;

    CampfireObject();
    ~CampfireObject() override;
    std::string getDescription() const override;
    eObjectTypes getObjectType() const override { return eObjectTypes::Campfire; }

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar& boost::serialization::base_object<Object>(*this);
    }
};
