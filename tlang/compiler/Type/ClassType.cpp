#include "Type.h"
#include "BuiltinType.h"
// ClassType implement
ClassType::ClassType()
{}

ClassType::ClassType(const string &name, Scope *scope, bool isPublic, bool isFrozen)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{}

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



/// ProtocolType implementation

// Protocol implementation
ProtocolType::ProtocolType()
:Type()
{}

ProtocolType::ProtocolType(const string &name, Scope *scope, bool isPublic)
:Type(), m_name(name), m_scope(scope), m_isPublic(isPublic)
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

/// Struction Implmentations

StructType::StructType()
{}
StructType::StructType(const string &name, Scope *scope, bool isPublic)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{
    
}
StructType::~StructType()
{}


bool StructType::operator !=(Type &type)
{
    return true; // dummy
}
bool StructType::operator ==(Type &type)
{
    return true; // dummy
}
Type& StructType::operator =(Type &type)
{
    return *this; // dummy
}


