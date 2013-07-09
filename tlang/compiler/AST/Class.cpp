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
// ClassBlock
ClassBlock::ClassBlock(const Location& location)
    :AST(location)
{
}

ClassBlock::~ClassBlock()
{
}

void ClassBlock::addMethod(Method* method)
{
    if (method)
        m_methods.push_back(method);
}

void ClassBlock::addVariable(Variable* var)
{
    if (var)
        m_vars.push_back(var);
}

Variable* ClassBlock::getVariable(const string& name)
{
    Variable* var = NULL;
    
    vector<Variable*>::iterator ite = m_vars.begin();
    for (; ite != m_vars.end(); ite++) {
        var = (Variable*)*ite;
        if (var->m_name == name)
            break;
    }
    return var;
    
}
Method* ClassBlock::getMethod(const string& name)
{
    Method* method = NULL;
    
    vector<Method*>::iterator ite = m_methods.begin();
    for (; ite != m_methods.end(); ite++) {
        method = (Method*)*ite;
        if (method->m_name == name)
            break;
    }
    return method;
    
}

