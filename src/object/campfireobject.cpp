#include "campfireobject.h"

using namespace std;

BOOST_CLASS_EXPORT(CampfireObject)

CampfireObject::CampfireObject() {}

CampfireObject::~CampfireObject() {}

string CampfireObject::getDescription() const
{
    return "A campfire to rest by. You can use them to save.";
}
