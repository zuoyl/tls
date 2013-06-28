//
//  Scope.cpp
//  A toyable language compiler (like a simple c++)


#include "Scope.h"
#include "Type.h"

/// @brief Scope constructor
Scope::Scope()
{
    m_parentScope = NULL;
}

Scope::Scope(const string &name, Scope *parent)
        :m_scopeName(name), m_parentScope(parent)
{

}
/// @brief Scope destructor
Scope::~Scope() 
{
	/// free all Objects	
    map<const string, Object*>::iterator ite = m_objects.begin();
	while (ite != m_objects.end()) {
		if (ite->second) 
			delete ite->second;
		ite++;
	}
 
	/// remove scope from parent's scope
	m_parentScope = NULL;
	
}

/// @brief Define a new Object in the scope 
void Scope::defineObject(Object *object) 
{
    if (object) {
        pair<const string, Object*> item(object->getName(), object);
        m_objects.insert(item);
    }
}

/// @brief Define a new type in the scope
void Scope::defineType(Type *type) 
{
    if (type) {
  //      m_types.addType(type->getName(), type);
    }
}


/// @brief Resolve a type by it's name 
Type* Scope::resolveType(const string &name, bool nested) 
{
    Type *type = NULL;
#if 0 
    m_types.getType(name, &type);
	
	if (!type && nested) {
        Scope *parent = getParentScope();
        if (parent) {
            type = parent->resolveType(name, &type);
        }
    }
#endif
    return type;
}

/// @brief Resolve a Object by it's name
Object* Scope::resolveObject(const string &name, bool nested) 
{
    Object *object = NULL;
    
    map<const string, Object*>::iterator ite = m_objects.find(name);
    if (ite != m_objects.end()) {
        object = ite->second;
		return object;
    }
	
	if (nested && !object) {
		Scope *parent = getParentScope();
		if (parent)
			object = parent->resolveObject(name, nested);
	}
    return object;
}

#if 0
int Scope::getObjectsCount()
{
    return (int)m_objects.size();
}

#endif 
