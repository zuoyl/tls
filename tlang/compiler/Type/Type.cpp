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
    if (!name.empty() && type)
        if (m_types.find(name) != m_types.end())
            m_types.insert(make_pair(name, type));
    
}
void TypeDomain::getType(const string &name, Type **type)
{
    map<string, Type*>::iterator ite = m_types.find(name);
    if (ite != m_types.end() && type != NULL)
        *type = ite->second;
    else 
        *type = NULL;
}


// helper methods

// type helper methods
bool isTypeCompatible(Type* type1, Type *type2)
{
    return true; //temp 
}
bool isType(Type *type, const string &name)
{
    return true; 
}
Type* getTypeBySpec(TypeSpec *spec)
{
    return NULL; // temp
}


