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
    map<string, Type* >::iterator ite = m_domains.begin();
    for (; ite != m_domains.end(); ite++) 
        delete ite->second;

    // free all types
    map<string, map<string, Type*>* >::iterator i = m_types.begin();
    for (; i != m_types.end(); i++){ 
        // delete class's types 
        map<string, Type* >* types = i->second;
        map<string, Type* >::iterator v = types->begin();
        for (; v != types->end(); v++)
                delete v->second;
        delete i->second; 
    }
    m_domains.clear();
    m_types.clear();
}

void TypeDomain::addDomain(const string& name, Type* type)
{
    if (name.empty() || !type)
        return;
    if (m_domains.find(name) == m_domains.end()) {
        m_domains[name] = type;
        m_types[name] = new map<string, Type* >();
    }
}
Type* TypeDomain::getDomain(const string& name)
{
    if (m_domains.find(name) != m_domains.end())
        return m_domains[name];
    else
        return NULL;
}

void TypeDomain::addType(const string& domain, const string& name, Type* type)
{
    if (!domain.empty() && !name.empty() && type) {
        // check wether the class exist
        if (m_domains.find(domain) == m_domains.end())
            return;
        map<string, Type* >* types = m_types[domain]; 
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
    map<string, Type*>* types = m_types[domain];
    if (types->find(name) != types->end())
        *type = (*types)[name];
}

void TypeDomain::initializeBuiltinTypes()
{
    m_domains["builtin"] = dynamic_cast<Type* >(new BuiltinType());
    map<string, Type*>* types = new map<string, Type*>();
    types->insert(make_pair("int", dynamic_cast<Type* >(new IntType())));
    types->insert(make_pair("void", dynamic_cast<Type* >(new VoidType())));
    types->insert(make_pair("bool", dynamic_cast<Type* >(new BoolType())));
    types->insert(make_pair("string", dynamic_cast<Type* >(new StringType())));
    types->insert(make_pair("float", dynamic_cast<Type* >(new FloatType())));
    types->insert(make_pair("map", dynamic_cast<Type* >(new MapType())));
    types->insert(make_pair("set", dynamic_cast<Type* >(new SetType())));
    m_types["builtin"] = types; 
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


