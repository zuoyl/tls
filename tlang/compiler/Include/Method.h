//
//  Method.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_FUNCTION_H
#define TCC_FUNCTION_H

#include "Common.h"
#include "AST.h"
#include "ASTVistor.h"
#include "Scope.h"
#include "Type.h"
#include "Expression.h"

class MethodParameter;
class MethodParameterList;
class MethodBlock;
class TypeSpecifier;
class Expr;

/// 'class Method
/// Method class to manage all semantics of method, the Method are both AST node and Scope
/// @see AST
/// @see Scope
class Method : public AST , public Scope {
public:
	/// Constructor
    Method();
	
	/// Constructor
    Method(const string &signature, 
             TypeSpecifier *typeSpec, 
             const string &id, 
             MethodParameterList *list,
             MethodBlock *block);
	
	/// Destructor
    ~Method();
	
	/// Walkhelper which access the method node
    void walk(ASTVisitor *visitor)    { visitor->accept(*this); }
	
	/// Check to see wether has parameter
	/// @ret true indicate that has paremeter else none
    bool hasParamter() { return ( m_paraList != NULL ); }
	
	/// Get parameter's count
	/// @ret the count of all parameters
    int  getParameterCount();
	
	/// Get specified parameter by index
	/// @param the parameter's index
    MethodParameter* getParameter(int index);
    
	/// Get locals's count for the method
	/// @ret the locals' count
    int  getLocalsCount();
	
	/// Get locals' total size for the method
	/// @ret the local's total size
	int  getLocalsSize();
    
public:
	/// Wether the method is a virtual method
    bool   m_isVirtual;
	/// Wether the method is static method
    bool   m_isStatic;
	///  Wether the method is public method
    bool   m_isPublic;
	/// Wethre the method has constant attribute
    bool   m_isConst;
	/// Wether the method is member method of class
    bool   m_isOfClass;
	/// Wether the method is memeber method of interface
    bool   m_isOfProtocol;
	
	/// If the funciton is member of class, the class name
    string m_class;
	/// The interface name if the method is a member of interface
    string m_protocol;
	/// Return type's name
    TypeSpec *m_retTypeSpec;
	/// Method's name
    string m_name;
	/// Signature 
    string   m_signature;
	/// Parameter list
    MethodParameterList *m_paraList;
	/// Method Block
    MethodBlock *m_block;
};


class MethodParameter : public AST {
public:
    MethodParameter(bool isConst, TypeSpec *typeSpec, const string &id, bool hasDefault, Expr *deft){}
    ~MethodParameter(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    bool m_isConst;
    bool m_hasDefault;
    int m_index;    // indicate the parameter's index
    TypeSpec *m_typeSpec;
    string m_name;
    Expr *m_default;
	Method *m_method;
};

class MethodParameterList : public AST {
public:
    MethodParameterList(){}
    ~MethodParameterList(){}
    void addParameter(MethodParameter *para){
        if (para)
            m_parameters.push_back(para);
    }
    
    int  getParameterCount() {
        return (int)m_parameters.size();
    }
    MethodParameter* getParameter(int index) {
        if (index >= 0 && index < m_parameters.size()) {
            return m_parameters.at(index);
        }
        return NULL;
    }
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    std::vector<MethodParameter *> m_parameters;
};

class MethodBlock : public AST {
public:
    MethodBlock(){}
    ~MethodBlock(){}
    void addStatement(Statement *stmt){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    vector<Statement *> m_stmts;
};

class MethodCallExpr : public Expr {
public:
		MethodCallExpr(){}
    MethodCallExpr(const string &methodName):m_methodName(methodName){}
    ~MethodCallExpr() {
        vector<Expr *>::iterator ite;
        for (ite = m_arguments.begin(); ite != m_arguments.end(); ite++)
            delete *ite;
    }
    void walk(ASTVisitor *visitor) { 
        visitor->accept(*this);
    }
    
    void setMethodName(const string &name) {
        m_methodName = name;
    }
    void appendArgument(Expr *expr) {
        if (expr)
            m_arguments.push_back(expr);
    }
    int getArgumentCount() {
        return (int)m_arguments.size();
    }
    Expr* getArgumentExpr(int index) {
        if (index >= 0 && index < (int)m_arguments.size())
            return m_arguments[index];
        else
            return NULL;
    }
public:
    string m_methodName;
    vector<Expr *> m_arguments;    
};


#endif // TCC_FUNCTION_H