//
//  Variable.cpp
//  A toyable language compiler (like a simple c++)

#include "Declaration.h"

Variable::Variable(TypeDecl* variableType, 
        const string& variableName, 
        const Location& location)
    :Declaration(location), 
    m_typeDecl(variableType),
    m_name(variableName)
{
    m_isInitialized = false;
    m_isGlobal = false;
    m_isOfClass = false;
}
Variable::~Variable()
{
    if (m_typeDecl)
        delete m_typeDecl;
}
