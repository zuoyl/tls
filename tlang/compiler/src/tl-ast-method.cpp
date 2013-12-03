//
//  ASTMethod.cpp
//  A toyable language compiler (like a simple c++)
//

#include "tl-ast-decl.h"
#include "tl-ast-vistor.h"
#include "tl-scope.h"

using namespace tl;

/// @brief ASTMethod constructor
ASTMethod::ASTMethod(const Location& location) 
    :ASTDeclaration(location), Scope("ASTMethod", NULL),m_paraList(NULL),m_block(NULL) 
{
}

/// @brief ASTMethod constructor
ASTMethod::ASTMethod(ASTTypeDecl *retType, const string &ASTMethodName, 
        const string &clsName, ASTFormalParameterList *list, 
        const Location &location)
        :ASTDeclaration(location), Scope(ASTMethodName, NULL), 
         m_class(clsName), 
         m_retTypeDecl(retType),
         m_name(ASTMethodName), 
         m_paraList(list), 
         m_block(NULL)
{ }

/// @brief ASTMethod destructor
ASTMethod::~ASTMethod() 
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

/// @brief Get ASTMethod parameter count
int  
ASTMethod::getParameterCount() 
{
    if (m_paraList)
        return m_paraList->getParameterCount();
    return 0;
}

/// @brief Get ASTMethod parameter by index
ASTFormalParameter* 
ASTMethod::getParameter(int index) 
{
    if (m_paraList)
        return m_paraList->getParameter(index);
    return NULL;
}

/// @brief Get locals count
int 
ASTMethod::getLocalsCount()
{
    // the local size could be found by current scope's symbol table
    int objectsTotalCount = getObjectsCount();
    
    // get all parameter's size
    int paraCount = 0;
    if (m_paraList) {
        paraCount = m_paraList->getParameterCount();
    }
    return (objectsTotalCount - paraCount);
    
}
/// @brief get locals's size
int 
ASTMethod::getLocalsSize()
{
    return 0; //temp	
}
