//
//  Scope.cpp
//  A toyable language compiler (like a simple c++)


#include "Scope.h"
#include "Type.h"

/// @brief Scope destructor
Scope::~Scope() {
	/// free all symbols	
    map<const string, Symbol*>::iterator ite = m_symbols.begin();
	while (ite != m_symbols.end()) {
		if (ite->second) 
			delete ite->second;
		ite++;
	}
 
	/// remove scope from parent's scope
	m_parentScope = NULL;
	
}

/// @brief Define a new symbol in the scope 
void Scope::defineSymbol(Symbol *symbol) {
    if (symbol) {
        std::pair<const string, Symbol*> item(symbol->m_name, symbol);
        m_symbols.insert(item);
    }
}

/// @brief Define a new type in the scope
void Scope::defineType(Type *type) {
    if (type) {
        m_types.addType(type->getName(), type);
    }
}


/// @brief Resolve a type by it's name 
Type* Scope::resolveType(const string &name, bool nested) {
    Type *type = m_types.getType(name);
	
	if (!type && nested) {
        Scope *parent = getParentScope();
        if (parent) {
            type = parent->resolveType(name, &type);
        }
    }
    return type;
}

/// @brief Resolve a symbol by it's name
Symbol* Scope::resolveSymbol(const string &name, bool nested) {
    Symbol *symbol = NULL;
    
    map<const string, Symbol*>::iterator ite = m_symbols.find(name);
    if (ite != m_symbols.end()) {
        symbol = ite->second;
		return symbol;
    }
	
	if (nested && !symbol) {
		Scope *parent = getParentScope();
		if (parent)
			symbol = parent->resolveSymbol(name, nested);
	}
    return symbol;
}

#if 0
int Scope::getSymbolsCount()
{
    return (int)m_symbols.size();
}

#endif 