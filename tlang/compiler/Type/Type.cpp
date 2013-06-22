//
//  Type.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Type.h"
#include "BuiltinType.h"

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
// object virtual table

/// @brief add a slot to table
void ObjectVirtualTable::addSlot(const string &name, Type *slot) 
{
    vector<pair<string, Type *> >::iterator ite;
    for (ite = m_slots.begin(); ite != m_slots.end(); ite++) {
        pair<string, Type *> &item = *ite;
        if (item.first == name && item.second == slot)
            break;
    }
    if (ite == m_slots.end())
        m_slots.push_back(make_pair(name, slot));
}
/// @brief get a slot by it's name
Type* ObjectVirtualTable::getSlot(const string &name)
{
    vector<pair<string, Type *> >::iterator ite;
    for (ite = m_slots.begin(); ite != m_slots.end(); ite++) {
        pair<string, Type *> &item = *ite;
        if (item.first == name)
            return item.second;
    }
    return NULL;
}
/// @brief get a slot by index
bool ObjectVirtualTable::getSlot(int index, string &name, Type **slot)
{
    if (index <0 || index > (int)m_slots.size())
        return false;
    if (!slot)
        return false;
    name = m_slots[index].first;
    *slot = m_slots[index].second;
    return true;
}



// TypeDomain implement

TypeDomain::TypeDomain()
{
    initializeBuiltinTypes();
}

TypeDomain::~TypeDomain()
{
    // free all types
    map<string, Type*>::iterator ite = m_types.begin();
    for (; ite != m_types.end(); ite++) 
        delete ite->second;
    m_types.clear();
}

void TypeDomain::addType(const string &name, Type *type, const string& fullFileName)
{
    if (!name.empty() && type) {
        if (m_types.find(name) == m_types.end()) {
            m_types.insert(make_pair(name, type));
            m_typeFiles.insert(make_pair(type->getName(), fullFileName));
        }
    }
}
void TypeDomain::getType(const string &name, Type **type)
{
    if ( m_types.find(name) != m_types.end())
        if (type)
            *type = m_types[name];
}

bool TypeDomain::isTypeFileExist(const string &fullName)
{
    bool result = false;
    map<string, string>::iterator ite = m_typeFiles.begin();
    for (; ite != m_typeFiles.end(); ite++){ 
        if (fullName == ite->second) {
            result = true;
            break;
        }
    }
    return result; 
}

void TypeDomain::initializeBuiltinTypes()
{
    m_types["int"] = dynamic_cast<Type *>(new IntType());
    m_types["void"] = dynamic_cast<Type *>(new VoidType());
    m_types["string"] = dynamic_cast<Type *>(new StringType());
    m_types["bool"] = dynamic_cast<Type *>(new BoolType());
    m_types["map"] = dynamic_cast<Type *>(new MapType());
    m_types["set"] = dynamic_cast<Type *>(new SetType());
    m_types["float"] = dynamic_cast<Type *>(new FloatType());
}

// helper methods

// type helper methods
bool isTypeCompatible(Type* type1, Type *type2)
{
    if (!type1 || !type2)
        return false;
    return type1->isCompatibleWithType(*type2);
}
bool isType(Type *type, const string &name)
{
    if (type && type->getName() == name)
        return true;
    else
        return false;
}


