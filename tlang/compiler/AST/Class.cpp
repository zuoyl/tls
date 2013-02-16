//
//  Class.cpp
//  A toyable language compiler (like a simple c++)
//

#include "AST/Class.h"
#include "AST/Variable.h"
#include "AST/ASTVistor.h"
#include "AST/Function.h"

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

void Class::addFunction(Function *func)
{
    if (m_block)
        m_block->addFunction(func);
    
}
Function* Class::getFunction(const string &name) const
{
    Function *func = NULL;
    
    if (m_block)
        func = m_block->getFunction(name);
    
    return func;
    
}



// ClassBlock
ClassBlock::ClassBlock()
{
}

ClassBlock::~ClassBlock()
{
}

void ClassBlock::addFunction(Function *function)
{
    if (function)
        m_functions.push_back(function);
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
Function* ClassBlock::getFunction(const string &name)
{
    Function *func = NULL;
    
    vector<Function*>::iterator ite = m_functions.begin();
    for (; ite != m_functions.end(); ite++) {
        func = (Function*)*ite;
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

void Interface::addFunction(Function *function)
{
    if (function)
        m_functions.push_back(function);
}


