//
//  Class.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Declaration.h"
#include "ASTVistor.h"

Class::Class(const string& clsName, 
        QualifiedName& baseClsName, 
        vector<QualifiedName>& abstractClsName,
        const Location& location)
:Declaration(location), m_name(clsName)
{
    m_baseClsName = baseClsName;
    vector<QualifiedName>::iterator ite = abstractClsName.begin();
    for (; ite != abstractClsName.end(); ite++) {
        m_abstractClsList.push_back(*ite); 
    }
}

Class::~Class()
{
    
}

void Class::walk(ASTVistor* vistor)
{
    vistor->accept(*this);
}

Variable* Class::getVariable(const string& name)const
{
    return NULL;
}

void Class::addVariable(Variable* var)
{
}

void Class::addMethod(Method* method)
{
    
}
Method* Class::getMethod(const string& name) const
{
    return NULL;
}

