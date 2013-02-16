//
//  Function.cpp
//  A toyable language compiler (like a simple c++)
//


#include "Function.h"
#include "ASTVistor.h"
#include <Runtime/Scope.h>

/// @brief Function constructor
Function::Function() :Scope("Function", NULL),
	 AST(NULL),m_paraList(NULL),m _blockList(NULL) {
		
}

/// @brief Function constructor
Function::Function(const string &signature, const string &retType, const string &id, 
	FunctionParameterList *list, FunctionBlock *block)
	:Scope(id, NULL), AST(NULL), m_signature(signature), m_returnType(retType),
	m_name(id), m_paraList(list), m_blockList(block) {
}

/// @brief Function destructor
Function::~Function() {
	if (m_paraList) {
		delete m_paraList;
		m_paraList = NULL; 
	}
	
	if (m_blockList) {
		delete m_blockList;
		m_blockList = NULL; 
	}
}

/// @brief Get function parameter count
int  Function::getParameterCount() 
{
    if (m_paraList)
        return m_paraList->getParameterCount();
    return 0;
}

/// @brief Get function parameter by index
FunctionParameter* Function::getParameter(int index) 
{
    if (m_paraList)
        return m_paraList->getParameter(index);
    return NULL;
}

/// @brief Get locals count
int Function::getLocalsCount()
{
    // the local size could be found by current scope's symbol table
    int symbolsTotalCount = getScope()->getSymbolsCount();
    
    // get all parameter's size
    int paraCount = 0;
    if (m_paraList) {
        paraCount = m_paraList->getParameterCount();
    }
    return (symbolsTotalCount - paraCount);
    
}
/// @brief get locals's size
int Function::getLocalsSize()
{
	
}
