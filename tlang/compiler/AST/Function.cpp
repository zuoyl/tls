//
//  Method.cpp
//  A toyable language compiler (like a simple c++)
//


#include "compiler/Method.h"
#include "compiler/ASTVistor.h"
#include "compiler/Runtime/Scope.h"

/// @brief Method constructor
Method::Method() :Scope("Method", NULL),
	 AST(NULL),m_paraList(NULL),m _blockList(NULL) {
		
}

/// @brief Method constructor
Method::Method(const string &signature, const string &retType, const string &id, 
	MethodParameterList *list, MethodBlock *block)
	:Scope(id, NULL), AST(NULL), m_signature(signature), m_returnType(retType),
	m_name(id), m_paraList(list), m_blockList(block) {
}

/// @brief Method destructor
Method::~Method() {
	if (m_paraList) {
		delete m_paraList;
		m_paraList = NULL; 
	}
	
	if (m_blockList) {
		delete m_blockList;
		m_blockList = NULL; 
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
    int symbolsTotalCount = getScope()->getSymbolsCount();
    
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
