//
//  Class.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_CLASS_H
#define TCC_CLASS_H

#include "compiler/Common.h"
#include "compiler/AST.h"
#include "compiler/ASTVistor.h"
#include "compiler/Type.h"


class Class : public AST {
public:
    Class(bool isPublic, const string &name, vector<string> &base, vector<string> &protocols, ClassBlock *block);
    ~Class();
    void walk(ASTVisitor *visitor);
 
    // for class variable
    void addVariable(Variable *var);
    Variable* getVariable(const string & name) const;
    /// for filed access
    int getVaraibleOffset(const string &name) const;
    
    // for class method
    void addFunction(Function *func);
    Function* getFunction(const string &name) const;
    
public:
    bool m_isPublic;
    string m_name;
    vector<string> m_base;
    vector<string> m_protocols;
    ClassBlock *m_block;
};

class ClassBlock : public AST {
public:
    ClassBlock();
    ~ClassBlock();
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void addFunction(Function *function);
    void addVariable(Variable *var);
    Variable* getVariable(const string &name);
    Function* getFunction(const string &name);
public:
    vector<Variable *> m_vars;
    vector<Function *> m_functions;
};

//
// protocol
//
class Protocol : public AST {
public:
    Protocol(const string &name);
    ~Protocol();
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void addFunction(Function *function);
    Function* getFunction(const string& name)const { return NULL; }
    
public:
    string m_name;
    bool m_isPublic;
    vector<Function *> m_functions;
};

#endif // TCC_CLASS_H
