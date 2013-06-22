#include "Type.h"
#include "BuiltinType.h"


ClassType::ClassType()
{
    m_isPublic = false;
    m_vtbl = NULL;
}

ClassType::ClassType(const string &name, bool isPublic, bool isFrozen)
    :Type(name, isPublic)
{
    m_isPublic = false;
}

ClassType::~ClassType()
{}


bool ClassType::operator !=(Type &type)
{
    return true; // dummy
}
bool ClassType::operator ==(Type &type)
{
    return true; // dummy
}
Type& ClassType::operator =(Type &type)
{
    return *this; // dummy
}






