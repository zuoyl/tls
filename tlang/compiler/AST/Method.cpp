//
//  Method.cpp
//  A toyable language compiler (like a simple c++)
//


#include "Method.h"
#include "ASTVistor.h"
#include "Scope.h"

/// @brief Method constructor
Method::Method(const Location &location) 
:Scope("Method", NULL),AST(location),m_paraList(NULL),m_block(NULL) 
{
}

/// @brief Method constructor
Method::Method(TypeSpec * retType, const string &id, 
	MethodParameterList *list, MethodBlock *block, const Location &location)
	:Scope(id, NULL), AST(location), m_retTypeSpec(retType),
	m_name(id), m_paraList(list), m_block(block) 
{
}

/// @brief Method destructor
Method::~Method() 
{
	if (m_paraList) {
		delete m_paraList;
		m_paraList = NULL; 
	}
	
	if (m_block) {
		delete m_block;
		m_block = NULL; 
	}
}

/// @brief Get method parameter count
int  Method::getParameterCount() 
{
    if (m_paraList)
        return m_paraList->getParameterCount();
    return 0;
}

/// @brief Get method parameter by index
MethodParameter* Method::getParameter(int index) 
{
    if (m_paraList)
        return m_paraList->getParameter(index);
    return NULL;
}

/// @brief Get locals count
int Method::getLocalsCount()
{
    // the local size could be found by current scope's symbol table
    int symbolsTotalCount = getSymbolsCount();
    
    // get all parameter's size
    int paraCount = 0;
    if (m_paraList) {
        paraCount = m_paraList->getParameterCount();
    }
    return (symbolsTotalCount - paraCount);
    
}
/// @brief get locals's size
int Method::getLocalsSize()
{
	
}
