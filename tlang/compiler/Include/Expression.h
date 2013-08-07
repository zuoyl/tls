//
//  Expr.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_EXPRESSION_H
#define TCC_EXPRESSION_H

#include "Common.h"
#include "AST.h"
#include "ASTVistor.h"
#include "Type.h"
#include "Value.h"
#include "Label.h"
#include "Location.h"
#include "BuiltinType.h"

class Value;
class ASTVistor;

/// 'class Expr
///  Basic class for all sub expression
class Expr : public AST 
{
public:    
	/// constructor
    Expr(const Location& location):AST(location){}
	
    /// destructor
    virtual ~Expr(){}
	
    /// walker method
    virtual void walk(ASTVistor* visitor) { visitor->accept(*this); }
    
    /// get the type of expression
    virtual Type* getType() = 0;
    
    /// check wether the expression is const 
    virtual bool isConstant() = 0;

	/// checke to see wether the expression has valid result
	virtual bool hasValidValue() { return false; } 
    
    /// get the result for expression evaluaton
    virtual void getValue(Value& val) { val = m_value; }
public:
    Value m_value; // temp
};

class ExprList : public AST 
{
public:
    ExprList(const Location& location):AST(location){}
    ~ExprList(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return NULL; }
    bool isConstant(); 
    void appendExpr(Expr* expr){}
public:
    vector<Expr*> m_exprs;
};

class AssignmentExpr : public Expr 
{
public:
    enum {BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_LSHIFT, BOP_RSHIFT};
    
public:
    AssignmentExpr(const string& op, Expr* left, Expr* right, const Location& location)
        :Expr(location), m_opname(op), m_left(left), m_right(right){}
    ~AssignmentExpr(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return NULL; }
    bool isConstant() { return false; } 
public:
    string m_opname; 
    int m_op;
    Expr* m_left;
    Expr* m_right;
};

class ComparisonExpr : public Expr 
{
public:
    enum {BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_LSHIFT, BOP_RSHIFT};
public:
    ComparisonExpr(Expr* target, const Location& location)
        :Expr(location), m_target(target){}
    ~ComparisonExpr(){}
    void appendElement(const string& op, Expr* expr){ 
        m_elements.push_back(make_pair(op, expr));
    }
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_boolType; }
    bool isConstant(); 
public:
    Expr* m_target;
    vector<pair<string, Expr*> > m_elements;
private:
    BoolType m_boolType;
};

class LogicOrExpr : public Expr 
{
public:
    LogicOrExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target) {}
    ~LogicOrExpr(){}
    void appendElement(Expr* expr){ m_elements.push_back(expr); }
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_boolType; }
    bool isConstant(); 

public:
    Expr* m_target;
    vector<Expr*> m_elements;
private:
    BoolType m_boolType;
};

class LogicAndExpr : public Expr 
{
public:
    LogicAndExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~LogicAndExpr(){}
    void appendElement(Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_boolType; }
    bool isConstant(); 
public:
    Expr* m_target;
    vector<Expr*> m_elements;
private:
    BoolType m_boolType;
};


class BitwiseOrExpr : public Expr 
{
public:
    BitwiseOrExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~BitwiseOrExpr(){}
    void appendElement(Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_intType; }
    bool isConstant(); 
public:
    Expr* m_target;
    vector<Expr*> m_elements;
private:
    IntType m_intType;
};

class BitwiseXorExpr : public Expr 
{
public:
    BitwiseXorExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~BitwiseXorExpr(){}
    void appendElement(Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
public:
    Expr* m_target;
    vector<Expr*> m_elements;
    Type* getType() { return &m_intType; }
    bool isConstant(); 
private:
    IntType m_intType;
};

class BitwiseAndExpr : public Expr 
{
public:
    BitwiseAndExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~BitwiseAndExpr(){}
    void appendElement(Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_intType; }
    bool isConstant(); 
public:
    Expr* m_target;
    vector<Expr* > m_elements;
private:
    IntType m_intType;
};

class EqualityExpr : public Expr 
{
public:
    enum { OP_EQ, OP_NEQ };
public:
    EqualityExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~EqualityExpr(){}
    void appendElement(int op, Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_boolType; }
    bool isConstant(); 
public:
    int m_op;
    Expr* m_target;
    vector<Expr* > m_elements;
private:
    BoolType m_boolType;
};

class RelationalExpr : public Expr 
{
public:
    enum { OP_GT, OP_LT, OP_GTEQ, OP_LTEQ };
    
    RelationalExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~RelationalExpr(){}
    void appendElement(int op, Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_boolType; }
    bool isConstant(); 
public:
    int m_op;
    Expr* m_target;
    vector<Expr* > m_elements;
private:
    BoolType m_boolType;
};

class ShiftExpr : public Expr 
{
public:
    enum { OP_LSHIFT, OP_RSHIFT };
public:
    ShiftExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~ShiftExpr(){}
    void appendElement(int op, Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_intType; }
    bool isConstant(); 
public:
    int m_op;
    Expr* m_target;
    vector<Expr*> m_elements;
private:
    IntType m_intType;
};


class AdditiveExpr : public Expr 
{
public:
    enum { OP_ADD, OP_SUB };
public:
    AdditiveExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    ~AdditiveExpr(){}
    void appendElement(int op, Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_intType; }
    bool isConstant(); 
public:
    int m_op;
    Expr* m_target;
    vector<Expr*> m_elements;
private:
    IntType m_intType;
};

class MultiplicativeExpr : public Expr 
{
public:
    enum { OP_MUL, OP_DIV, OP_MODULO };
public:
    MultiplicativeExpr(Expr* target, const Location& location)
        :Expr(location),m_target(target){}
    MultiplicativeExpr(const Location& location):Expr(location){}
    void appendElement(int op, Expr* expr){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return &m_intType; }
    bool isConstant(); 
public:
    int m_op;
    Expr* m_target;
    vector<Expr*> m_elements;    
private:
    IntType m_intType;
};

class SelectorExpr;
class PrimaryExpr;
class UnaryExpr : public Expr 
{
public:
    UnaryExpr(PrimaryExpr* target, const Location& location)
        :Expr(location), m_primary(target){}
    ~UnaryExpr(){}
    void appendElement(SelectorExpr* expr){ m_selectors.push_back(expr); }
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType(); 
    bool isConstant(); 
public:
    PrimaryExpr* m_primary;
    vector<SelectorExpr* > m_selectors;
    
};

class SelectorExpr : public Expr 
{
public:
    enum { DOT_SELECTOR, ARRAY_SELECTOR, METHOD_SELECTOR};
    
    SelectorExpr(int type, const string& id, const Location& location)
        :Expr(location),m_type(type), m_id(id){}
    SelectorExpr(int type, Expr* expr, const Location& location)
        :Expr(location),m_type(type){
        if (type == ARRAY_SELECTOR)
            m_arrayExpr = expr;
        else
            m_methodCallExpr = (MethodCallExpr* )expr;
        }
    SelectorExpr(const Location& location):Expr(location){}
    ~SelectorExpr(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType(); 
    bool isConstant(); 
public:
    int m_type;
    string m_id; // for .identifier
    Expr* m_arrayExpr;
    MethodCallExpr* m_methodCallExpr;
};

class PrimaryExpr : public Expr 
{
public:
    enum {
        T_SELF, 
        T_SUPER, 
        T_NULL, 
        T_TRUE,
        T_FALSE, 
        T_NUMBER, 
        T_HEX_NUMBER, 
        T_STRING,
        T_MAP, 
        T_LIST,
        T_IDENTIFIER,
        T_COMPOUND
    };
public: 
    PrimaryExpr(int type, const Location& location)
        :Expr(location), m_type(type), m_resultType(NULL){}
    PrimaryExpr(int type, const string& text, const Location& location)
        :Expr(location), m_type(type),m_text(text),m_resultType(NULL){}
    PrimaryExpr(int type, Expr* expr, const Location& location)
        :Expr(location),m_type(type),m_expr(expr),m_resultType(NULL){}
    ~PrimaryExpr(){ 
        if (m_resultType)
            delete m_resultType;
    }
    void appendSelector(SelectorExpr* sel){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType(); 
    bool isConstant(); 
public:
    int m_type;
    string m_text;
    Expr* m_expr;
    Type* m_resultType;
};

class ArgumentList : public Expr 
{
public:
    ArgumentList(const Location& location):Expr(location){}
    ~ArgumentList(){}
    void walk(ASTVistor* visitor) { visitor->accept(*this); }
    void appendArgument(Expr* expr) { m_arguments.push_back(expr); }
    Type* getType() { return NULL; }
    bool isConstant() { return false; }
public:
    vector<Expr* > m_arguments;

};

class MethodCallExpr : public Expr 
{
public:
	MethodCallExpr(const Location& location):Expr(location){}
    MethodCallExpr(const string& methodName, const Location& location)
        :Expr(location), m_methodName(methodName){}
    ~MethodCallExpr() {
        vector<Expr* >::iterator ite;
        for (ite = m_arguments.begin(); ite != m_arguments.end(); ite++)
            delete* ite;
    }
    void walk(ASTVistor* visitor) { 
        visitor->accept(*this);
    }
    Type* getType() { return NULL; }
    bool isConstant() { return false; }
    void setMethodName(const string& name) {
        m_methodName = name;
    }
    string& getMethodName() { return m_methodName; }
    
    void appendArgument(Expr* expr) {
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
    vector<Expr* > m_arguments;    
};

/// 'class IterableObject 
/// the iterable object can be iterated by foreach statement
class NewExpr : public Expr 
{
public:
    NewExpr(const string& type, ArgumentList* list, const Location& location)
        :Expr(location), m_type(type), m_arguments(list){}
    ~NewExpr(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return NULL; }
    bool isConstant() { return false; } 
public:
    string m_type;
    ArgumentList* m_arguments; 
};

class TypeExpr: public Expr 
{
public:
    enum { TE_SET, TE_MAP, TE_USER, TE_BUILTIN};
public:
    TypeExpr(int type, const string& name, const Location& location)
        :Expr(location), m_name1(name){}
    TypeExpr(int type, const string& name1, const string& name2, const Location& location)
        :Expr(location), m_name1(name1),m_name2(name2){}
    ~TypeExpr(){}
    Type* getType() { return NULL; }
    bool isConstant(){ return false; } 
public:
    int m_type;
    string m_name1;
    string m_name2; 
    
};

class MapItemExpr;
class MapExpr : public Expr 
{
public:
	MapExpr(const Location& location):Expr(location){}
    MapExpr(int type, const Location& location):Expr(location){}
    ~MapExpr(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    void appendItem(MapItemExpr* item){ m_items.push_back(item);}
    Type* getType() { return NULL; }
    bool isConstant(); 
public:
    vector<MapItemExpr*> m_items;
};

class MapItemExpr: public Expr 
{
public:
    MapItemExpr(Expr* key, Expr* val, const Location& location)
        :Expr(location), m_key(key),m_val(val){}
    ~MapItemExpr(){}  
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return NULL; }
    bool isConstant(); 
public:
    Expr* m_key;
    Expr* m_val;
};

class SetExpr : public Expr 
{
public:
    SetExpr(ExprList* exprList, const Location& location)
        :Expr(location){}
    SetExpr(const Location& location):Expr(location){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    Type* getType() { return NULL; }
    bool isConstant(); 
public:
    ExprList* m_exprList;
};


#endif // TCC_EXPRESSION_H
