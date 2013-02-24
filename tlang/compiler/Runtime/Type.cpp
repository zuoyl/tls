//
//  Type.cpp
//  A toyable language compiler (like a simple c++)
//

#include "compiler/Type.h"

// TypeDomain implement

TypeDomain::TypeDomain()
{}
TypeDomain::~TypeDomain()
{}

void TypeDomain::addType(const string &name, Type *type)
{
    std::pair<string,Type *> item(name, type);
    m_types.insert(item);
    
}
void TypeDomain::getType(const string &name, Type **type)
{
    map<string, Type*>::iterator ite = m_types.find(name);
    if (ite != m_types.end() && type != NULL)
        *type = ite->second;
    else 
        *type = NULL;
}

// ClassType implement
ClassType::ClassType()
{}

ClassType::ClassType(const string &name, Scope *scope, bool isPublic)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{}

ClassType::~ClassType()
{}

void ClassType::addSlot(const string &name, Type *slot)
{
}
Type* ClassType::getSlot(const string &name) const
{
    return NULL; // dummy
}

Type* ClassType::getSlot(int index)
{
    return NULL; // dummy
}

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


// Interface implement
InterfaceType::InterfaceType()
{}

InterfaceType::InterfaceType(const string &name, Scope *scope, bool isPublic)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{
    
}

void InterfaceType::addSlot(const string &name, Type *slot)
{
}
Type* InterfaceType::getSlot(const string &name) const
{
    return NULL; // dummy
}
int  InterfaceType::getSlotCount() const
{
    int size = 0;
    
    if (m_vtbl)
        size = m_vtbl->getSize();
    return size;
}
Type* InterfaceType::getSlot(int index)
{
    return NULL; // dummy
    
}

bool InterfaceType::operator !=(Type &type)
{
    return true; // dummy
}
bool InterfaceType::operator ==(Type &type)
{
    return true; // dummy
}
Type& InterfaceType::operator =(Type &type)
{
    return *this; // dummy
}

// StructType implement
StructType::StructType()
{}
StructType::StructType(const string &name, Scope *scope, bool isPublic)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{
    
}
StructType::~StructType()
{}


void StructType::addSlot(const string &name, Type *slot)
{
}
Type* StructType::getSlot(const string &name) const
{
    return NULL; // dummy
}

Type* StructType::getSlot(int index)
{
    return NULL; // dummy
}

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


// FunctionType implement
FunctionType::FunctionType()
{}
FunctionType::FunctionType(const string &name, Scope *scope, bool isPublic)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{
    
}
FunctionType::~FunctionType()
{}

void FunctionType::addSlot(const string &name, Type *slot)
{
}

Type* FunctionType::getSlot(const string &name) const
{
    return NULL; // dummy
}

Type* FunctionType::getSlot(int index)
{   
    return NULL; // dummy
    
}

bool FunctionType::operator !=(Type &type)
{
    return true; // dummy
}
bool FunctionType::operator ==(Type &type)
{
    return true; // dummy
}
Type& FunctionType::operator =(Type &type)
{
    return *this; // dummy
}




