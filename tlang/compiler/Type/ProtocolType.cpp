//
//  ProtocolType.cpp
//  A toyable language compiler (like a simple c++)
//
#include "BuiltinType.h"
// Protocol implementation
ProtocolType::ProtocolType():Type()
{
    m_isPublic = false;
}

ProtocolType::ProtocolType(const string &name,bool isPublic)
    :Type(name, isPublic)
{
}
ProtocolType::~ProtocolType()
{}


bool ProtocolType::operator !=(Type &type)
{
    return true; // dummy
}
bool ProtocolType::operator ==(Type &type)
{
    return true; // dummy
}
Type& ProtocolType::operator =(Type &type)
{
    return *this; // dummy
}


