//
//  Scope.h - manage the Object scope 
//  A toyable language compiler (like a simple c++)


#ifndef TCC_SCOPE_H
#define TCC_SCOPE_H

#include "Common.h"
#include "Type.h"

using namespace std;

class Object;

/// "class Scope
/// Scope manage all Objects in current life scope, such as method block,class block,etc.
class Scope 
{
public:
	Scope();
	/// Scope constructor
	/// @param name the name of the scope
	/// @param parent the parent scope of the scope
    Scope(const string &name, Scope *parent);
	
	/// Scope destructor
    virtual ~Scope();
	
	/// Get scope's name
	/// @ret the name of scope
    const string& getScopeName() { return m_scopeName; }
	
	/// Get scope's parent scope
	/// @ret the parent scope
    Scope* getParentScope() { return m_parentScope; }
	
	/// Set scope's parent scope
	/// @param parent the parent scope
	void setParentScope(Scope *parent) { m_parentScope = parent; }
	
	/// Define a new Object in the scope, if the Object exist, excpetion will be throwed.
	/// @param Object the new Object 
    void defineObject(Object *object);
	
	/// Check to see wether has a specified Object
	/// @param name the Object's name
	/// @param nested indicate wether to resolve recursively
	/// @ret true indicate that has the Object, else none
	bool hasObject(const string& name, bool nested = false);
	
	/// Resolve a Object by the specified name if the Object exist
	/// @param name the Object's name
	/// @param nested indicate wether to resolve recursively, for example the parent's scope
	/// @ret the Object found by name
    Object* resolveObject(const string &name, bool nested = false);
	
	/// Define a new type in the scope, if the type exist, exception will be throwed
	/// @param type the new type
    void defineType(Type *type);
	
	/// Resolve a type by the specified name if the type exist
	/// @param name the type's name
	/// @param nested indicate wether to resolve recursively, for example the parent's scope
    Type* resolveType(const string &name, bool nested = true);

	/// Resolve a type by the specified name if the type exist
	/// @param name the type's name
	/// @param nested indicate wether to resolve recursively, for example the parent's scope
	/// ret true indicate that has the Object, else none
    bool hasType(const string &name, bool nested = true);

	int getObjectsCount() { return (int)m_objects.size(); }
protected:
	/// Name of the scope
    string m_scopeName;
	/// Parent scope of the scope
    Scope *m_parentScope;
	
	/// Holder for all Objects in the scope
    map<string, Object *> m_objects;
	
	/// Holder for all types in the scope
    TypeDomain m_types;
};

class Object {
public:
enum { LocalStackObject, LocalMemoryObject, GlobalObject };
public:
    string m_name;
    Type *m_type;
    int m_size;
    int m_addr;
    int m_storage;      // indicate wether in stack or in memory
}; 



#endif // TCC_SCOPE_H
