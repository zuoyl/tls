#include "Type.h"
#include "BuiltinType.h"

// MethodType implementations

MethodType::MethodType()
{}
MethodType::MethodType(const string &name, bool isPublic)
:m_name(name), m_isPublic(isPublic)
{
    
}
MethodType::~MethodType()
{}


bool MethodType::operator !=(Type &type)
{
    return true; // dummy
}
bool MethodType::operator ==(Type &type)
{
    return true; // dummy
}
Type& MethodType::operator =(Type &type)
{
    return *this; // dummy
}


