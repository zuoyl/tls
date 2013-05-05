//
//  Variable.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_VARIABLE_H
#define TCC_VARIABLE_H

#include "Common.h"
#include "Scope.h"
#include "Value.h"
#include "AST.h"
#include "Type.h"
#include "Expression.h"

class ASTVisitor;

/// 'class Variable
class Variable : public AST {
public:
	/// Constructor
    Variable(bool isStatic, bool isConst, TypeSpec *typeSpec, const string &id, Expr *expr);
	/// Destructor
    ~Variable();
	/// walkhelper method
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
    Expr *m_expr;
	/// Type of variable
    TypeSpec * m_typeSpec;
	/// Name of Variable
    string m_name;
	/// Class name of variable if the variable is member of class
    string m_class;
	/// Const initialization value
    Value m_initializedVal;
};

// TypeSpec - the AST node for type declaration
// such as in, map, set, etc
class TypeSpec : public AST 
{
public:
    TypeSpec();
    TypeSpec(const string &name, int typeID):m_name(name),m_typeid(typeID){}
    enum {intType, boolType, stringType, floatType, mapType,setType, idType, customType };
    void walk(ASTVisitor *visitor) 
		{ visitor->accept(*this); }        
public:
    string m_name;
    int m_typeid;
    string m_t1;
    string m_t2;
};

#endif // TCC_VARIABLE_H
