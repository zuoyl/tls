//
//  Type.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Type.h"
#include "BuiltinType.h"

/// Type implementation

Type::Type()
{
    m_vtbl = new ObjectVirtualTable();
}
Type::Type(const string name, bool isPublic)
        :m_name(name), m_isPublic(isPublic)
{
    m_vtbl = new ObjectVirtualTable();
}

Type::~Type()
{
    m_isPublic = false;
    delete m_vtbl;
}

void Type::addSlot(const string& name, Type* type)
{
    if (!name.empty() && type) {
       if (!m_vtbl->getSlot(name)) 
           m_vtbl->addSlot(name, type);
    }
}

Type* Type::getSlot(const string& name)
{
    return m_vtbl->getSlot(name);
}

int Type::getSlotCount()
{
    return m_vtbl->getSlotCount();
}

Type* Type::getSlot(int index)
{
    string name;
    Type* type;
    if (m_vtbl->getSlot(index, name,& type))
        return type;
    return NULL;
}
// object virtual table

/// @brief add a slot to table
void ObjectVirtualTable::addSlot(const string& name, Type* slot) 
{
    vector<pair<string, Type* > >::iterator ite;
    for (ite = m_slots.begin(); ite != m_slots.end(); ite++) {
        pair<string, Type* >& item =* ite;
        if (item.first == name && item.second == slot)
            break;
    }
    if (ite == m_slots.end())
        m_slots.push_back(make_pair(name, slot));
}
/// @brief get a slot by it's name
Type* ObjectVirtualTable::getSlot(const string& name)
{
    vector<pair<string, Type* > >::iterator ite;
    for (ite = m_slots.begin(); ite != m_slots.end(); ite++) {
        pair<string, Type* >& item =* ite;
        if (item.first == name)
            return item.second;
    }
    return NULL;
}
/// @brief get a slot by index
bool ObjectVirtualTable::getSlot(int index, string& name, Type**slot)
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
    // free class types
    map<string, map<string, Type*>* >::iterator ite = m_domains.begin();
    for (; ite != m_domains.end(); ite++) { 
        map<string, Type*>* types = ite->second;
        map<string, Type*>::iterator i = types->begin();
        for (; i != types->end(); i++) {
            if (i->second)
                delete i->second;
        }
        delete types;
    }
    m_domains.clear();
}

void TypeDomain::addType(const string& domain, const string& name, Type* type)
{
    if (!domain.empty() && !name.empty() && type) {
        map<string, Type* >* types = m_domains[domain]; 
        if (types->find(name) == types->end()) 
            types->insert(make_pair(name, type));
    }
}
void TypeDomain::getType(const string& domain, const string& name, Type** type)
{
    if (domain.empty() || name.empty() || !type)
        return;
    if (m_domains.find(domain) == m_domains.end())
        return;
    map<string, Type*>* types = m_domains[domain];
    if (types->find(name) != types->end())
        *type = (*types)[name];
}

void TypeDomain::initializeBuiltinTypes()
{
    // initialize the builtin types  
    map<string, Type*>* types = new map<string, Type*>();
    types->insert(make_pair("int", dynamic_cast<Type* >(new IntType())));
    types->insert(make_pair("void", dynamic_cast<Type* >(new VoidType())));
    types->insert(make_pair("bool", dynamic_cast<Type* >(new BoolType())));
    types->insert(make_pair("string", dynamic_cast<Type* >(new StringType())));
    types->insert(make_pair("float", dynamic_cast<Type* >(new FloatType())));
    types->insert(make_pair("map", dynamic_cast<Type* >(new MapType())));
    types->insert(make_pair("set", dynamic_cast<Type* >(new SetType())));
    m_domains["builtin"] = types; 
    // initialize the class type domain 
    map<string, Type*> *classTypeDomain = new map<string, Type*>();
    m_domains["classType"] = classTypeDomain;
}

// helper methods

// type helper methods
bool isTypeCompatible(Type* type1, Type* type2)
{
    if (!type1 || !type2)
        return false;
    return type1->isCompatibleWithType(*type2);
}
bool isType(Type* type, const string& name)
{
    if (type && type->getName() == name)
        return true;
    else
        return false;
}


