//
//  Variable.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_VARIABLE_H
#define TCC_VARIABLE_H

#include "Common.h"
#include "Runtime/Scope.h"
#include "Runtime/Local.h"
#include "AST/AST.h"
#include "AST/Expression.h"

class ASTVisitor;

/// 'class Variable
class Variable : public AST {
public:
	/// Constructor
    Variable(bool isStatic, bool isConst, const string &type, const string &id, Expression *expr);
	/// Destructor
    ~Variable();
	/// walkhelper function
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    
public:
	/// Wether the variable is public
    bool m_isPublic;
	/// Wether the variable is static
    bool m_isStatic;
	/// Wether the vairalbe is const
    bool m_isConst;
	/// Wether the vraible is been initialized
    bool m_isInitialized;
	/// Wether the variable is member of class
    bool m_isOfClass;
	/// Wether the variable is global variable
    bool m_isGlobal;
    
	/// Initialization expression if the vaible is initialized
    Expression *m_expr;
	/// Type of variable
    string m_type;
	/// Name of Variable
    string m_name;
	/// Class name of variable if the variable is member of class
    string m_class;
	/// Const initialization value
    Local m_initializedVal;
};

#endif // TCC_VARIABLE_H
