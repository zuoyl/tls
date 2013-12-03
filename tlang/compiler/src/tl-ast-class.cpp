//
//  ASTClass.cpp
//  A toyable language compiler (like a simple c++)
//

#include "tl-ast-decl.h"
#include "tl-ast-vistor.h"

using namespace tl;

ASTClass::ASTClass(const string &clsName, 
        QualifiedName &baseClsName, 
        vector<QualifiedName> &abstractClsName,
        const Location &location) : ASTDeclaration(location), m_name(clsName)
{
    m_baseClsName = baseClsName;
    std::vector<QualifiedName>::iterator ite = abstractClsName.begin();
    for (; ite != abstractClsName.end(); ite++) {
        m_abstractClsList.push_back(*ite); 
    }
}

ASTClass::~ASTClass()
{
    
}

void 
ASTClass::walk(ASTVistor *vistor)
{
    vistor->accept(*this);
}

ASTVariable* 
ASTClass::getVariable(const string &name)const
{
    return NULL;
}

void 
ASTClass::addVariable(ASTVariable *var)
{
}

void 
ASTClass::addMethod(ASTMethod *method)
{
    
}

ASTMethod* 
ASTClass::getMethod(const string &name) const
{
    return NULL;
}

