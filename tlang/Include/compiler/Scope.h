//
//  Scope.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_SCOPE_H
#define TCC_SCOPE_H

#include "tcc.h"
#include "Runtime/Type.h"

using namespace std;

class Symbol;

/// "class Scope
/// Scope manage all symbols in current life scope, such as function block,class block,etc.
class Scope {
public:
	/// Scope constructor
	/// @param name the name of the scope
	/// @param parent the parent scope of the scope
    Scope(const string &name, Scope *parent):m_scopeName(name), m_parentScope(parent){}
	
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
	
	/// Define a new symbol in the scope, if the symbol exist, excpetion will be throwed.
	/// @param symbol the new symbol 
    void defineSymbol(Symbol *symbol);
	
	/// Check to see wether has a specified symbol
	/// @param name the symbol's name
	/// @param nested indicate wether to resolve recursively
	/// @ret true indicate that has the symbol, else none
	bool hasSymbol(const string& name, bool nested = false);
	
	/// Resolve a symbol by the specified name if the symbol exist
	/// @param name the symbol's name
	/// @param nested indicate wether to resolve recursively, for example the parent's scope
	/// @ret the symbol found by name
    Symbol* resolveSymbol(const string &name, bool nested = false);
	
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
	/// ret true indicate that has the symbol, else none
    bool hasType(const string &name, bool nested = true);
	    
protected:
	/// Name of the scope
    string m_scopeName;
	/// Parent scope of the scope
    Scope *m_parentScope;
	
	/// Holder for all symbols in the scope
    map<string, Symbol *> m_symbols;
	
	/// Holder for all types in the scope
    TypeDomain m_types;
};

struct Symbol {
    string m_name;
    Type *m_type;
    int m_size;
    int m_addr;

} /// namespace tcc

#endif // TCC_SCOPE_H
