//
//  Class.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Class.h"
#include "Variable.h"
#include "ASTVistor.h"
#include "Method.h"

Class::Class(bool isPublic, const string &name, vector<string> &base, ClassBlock *block)
:m_isPublic(isPublic), m_name(name), m_block(block)
{
    vector<string>::iterator ite = base.begin();
    for (; ite != base.end(); ite++) {
        m_base.push_back(*ite);
    }
}

Class::~Class()
{
    
}

void Class::walk(ASTVisitor *visitor)
{
    visitor->accep(*this);
}

Variable* Class::getVariable(const string &name)const
{
    Variable *var = NULL;
    
    if (m_block)
        var = m_block->getVariable(name);
    
    return var;
}

void Class::addVariable(Variable *var)
{
    if (m_block)
        m_block->addVariable(var);
}

void Class::addMethod(Method *func)
{
    if (m_block)
        m_block->addMethod(func);
    
}
Method* Class::getMethod(const string &name) const
{
    Method *func = NULL;
    
    if (m_block)
        func = m_block->getMethod(name);
    
    return func;
    
}



// ClassBlock
ClassBlock::ClassBlock()
{
}

ClassBlock::~ClassBlock()
{
}

void ClassBlock::addMethod(Method *method)
{
    if (method)
        m_methods.push_back(method);
}

void ClassBlock::addVariable(Variable *var)
{
    if (var)
        m_vars.push_back(var);
}

Variable* ClassBlock::getVariable(const string &name)
{
    Variable *var = NULL;
    
    vector<Variable*>::iterator ite = m_vars.begin();
    for (; ite != m_vars.end(); ite++) {
        var = (Variable*)*ite;
        if (var->m_name == name)
            break;
    }
    return var;
    
}
Method* ClassBlock::getMethod(const string &name)
{
    Method *func = NULL;
    
    vector<Method*>::iterator ite = m_methods.begin();
    for (; ite != m_methods.end(); ite++) {
        func = (Method*)*ite;
        if (func->m_name == name)
            break;
    }
    return func;
    
}



Interface::Interface(const string &name)
    : m_name(name)
{
}

Interface::~Interface()
{
}

void Interface::addMethod(Method *method)
{
    if (method)
        m_methods.push_back(method);
}


