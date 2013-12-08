#include "tl-type.h"
#include "tl-type-builtin.h"

using namespace tlang;

ClassType::ClassType()
{
    m_isPublic = false;
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






