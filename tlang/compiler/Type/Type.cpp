//
//  Type.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Type.h"

/// Type implementation
Type::~Type()
{
    // free all slots
    map<string, Type*>::iterator ite = m_slots.begin();
    for (; ite != m_slots.end(); ite++)
        delete ite->second;
    m_slots.clear();
    m_slotseqs.clear();
    m_isPublic = false;
}

void Type::addSlot(const string &name, Type *type)
{
    if (!name.empty() && type) {
        if (m_slots.find(name) != m_slots.end()) {
            m_slots.insert(make_pair(name, type));
            m_slotseqs.push_back(type);
        }
    }
}

Type* Type::getSlot(const string &name)
{
    if (m_slots.find(name) != m_slots.end())
        return m_slots[name];
    else
        return NULL;
}

int Type::getSlotCount()
{
    assert(m_slots.size() == m_slotseqs.size());
    return (int)m_slots.size();
}

Type* Type::getSlot(int index)
{
    if (index >= 0 && index < (int)m_slots.size())
        return m_slotseqs[index];
    else
        return NULL;
}


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

ClassType::ClassType(const string &name, Scope *scope, bool isPublic, bool isFrozen)
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
ProtocolType::ProtocolType()
:Type()
{}

ProtocolType::ProtocolType(const string &name, Scope *scope, bool isPublic)
:Type(), m_name(name), m_scope(scope), m_isPublic(isPublic)
{
    
}
ProtocolType::~ProtocolType()
{}

void ProtocolType::addSlot(const string &name, Type *slot)
{
}
Type* ProtocolType::getSlot(const string &name) const
{
    return NULL; // dummy
}
int  ProtocolType::getSlotCount() const
{
    int size = 0;
    
    if (m_vtbl)
        size = m_vtbl->getSize();
    return size;
}
Type* ProtocolType::getSlot(int index)
{
    return NULL; // dummy
    
}

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


// MethodType implement
MethodType::MethodType()
{}
MethodType::MethodType(const string &name, Scope *scope, bool isPublic)
:m_name(name), m_scope(scope), m_isPublic(isPublic)
{
    
}
MethodType::~MethodType()
{}

void MethodType::addSlot(const string &name, Type *slot)
{
}

Type* MethodType::getSlot(const string &name) const
{
    return NULL; // dummy
}

Type* MethodType::getSlot(int index)
{   
    return NULL; // dummy
    
}

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




