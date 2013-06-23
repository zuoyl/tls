//
//  Class.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_CLASS_H
#define TCC_CLASS_H

#include "Common.h"
#include "AST.h"
#include "ASTVistor.h"
#include "Type.h"
#include "Location.h"
#include "Scope.h"

class Class : public AST, public Scope {
public:
    Class(bool isPublic, 
        bool isFinal, 
        bool isAbstract,
        const string &name, 
        vector<string> &base, 
        vector<string> &abstractClass,
        ClassBlock *block,
        const Location &location);
    ~Class();
    void walk(ASTVisitor *visitor);
 
    // for class variable
    void addVariable(Variable *var);
    Variable* getVariable(const string & name) const;
    /// for filed access
    int getVaraibleOffset(const string &name) const;
    
    // for class method
    void addMethod(Method *func);
    Method* getMethod(const string &name) const;
    
    // helper methods
    bool isPublic() { return m_isPublic; }
    bool isFinal() { return m_isFinal; }
    bool isInheritClass() { return (m_base.size() > 0); }
    bool isImplementedAbstractClass() { return (m_abstractCls.size() > 0); }
    
public:
    bool m_isPublic;
    bool m_isAbstract; 
    bool m_isFinal;
    string m_name;
    vector<string> m_base;
    vector<string> m_abstractCls;
    ClassBlock *m_block;
};

class ClassBlock : public AST {
public:
    ClassBlock(const Location &location);
    ~ClassBlock();
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void addMethod(Method *method);
    void addVariable(Variable *var);
    Variable* getVariable(const string &name);
    Method* getMethod(const string &name);
public:
    vector<Variable *> m_vars;
    vector<Method *> m_methods;
};

#endif // TCC_CLASS_H
