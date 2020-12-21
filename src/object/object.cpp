#include "object.h"

static Object::ObjectId ids = static_cast<Object::ObjectId>(0);

BOOST_CLASS_EXPORT(Object)

Object::Object()
    : m_position(0, 0)
    , m_id(ids++)
{
}

void Object::run(const Level* level) {}
