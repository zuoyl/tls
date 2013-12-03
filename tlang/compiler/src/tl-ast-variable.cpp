//
//  Variable.cpp
//  A toyable language compiler (like a simple c++)

#include "tl-ast-decl.h"
#include "tl-location.h"

using namespace tl;

ASTVariable::ASTVariable(ASTTypeDecl *variableType, 
        const std::string &variableName, 
        const Location &location):
    ASTDeclaration(location), 
    m_typeDecl(variableType),
    m_name(variableName)
{
    m_isInitialized = false;
    m_isGlobal = false;
    m_isOfClass = false;
    m_expr = NULL;
}
ASTVariable::~ASTVariable()
{
    if (m_typeDecl)
        delete m_typeDecl;
}
