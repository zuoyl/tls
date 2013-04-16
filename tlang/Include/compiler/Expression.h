//
//  Expression.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_EXPRESSION_H
#define TCC_EXPRESSION_H

#include "compiler/Common.h"
#include "compiler/AST.h"
#include "compiler/ASTVistor.h"
#include "compiler/Type.h"
#include "compiler/Value.h"
#include "compiler/Label.h"

class Value;
class ASTVisitor;

/// 'class Expression
///  Basic class for all sub expression
class Expression : public AST {
public:
	/// Constructor
    Expression():m_result(), m_type(NULL), m_isConst(false){}
	/// Destructor
    virtual ~Expression(){}
	/// Walker method
    virtual void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	
	/// Checker to see wether the expression has valid result
	virtual bool hasValidValue() { return m_value.isValid(); }
    
public:
	/// Wether the expression is constant
    bool m_isConst;
	/// If the expression is constant, the result
    Value m_value;
	/// Type of Expression
    Type *m_type;
};

class ExpressionList : public AST {
public:
    ExpressionList(){}
    ~ExpressionList(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void appendExpression(Expression *expr){}
public:
    vector<Expression *> m_exprs;
};

class BinaryOpExpression : public Expression {
public:
    enum {BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_LSHIFT, BOP_RSHIFT};
    
public:
    BinaryOpExpression(const string &op, Expression *leftExpr, Expression *rightExpr){}
    ~BinaryOpExpression(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expression *m_left;
    Expression *m_right;
};

class ConditionalExpression : public Expression {
public:
    ConditionalExpression(){}
    ~ConditionalExpression(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
};

class LogicOrExpression : public Expression {
public:
    LogicOrExpression(Expression *target){}
    ~LogicOrExpression(){}
    void appendElement(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}

public:
    Expression *m_target;
    vector<Expression *> m_elements;
};

class LogicAndExpression : public Expression {
public:
    LogicAndExpression(Expression *target){}
    ~LogicAndExpression(){}
    void appendElement(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_target;
    vector<Expression *> m_elements;
    
};


class BitwiseOrExpression : public Expression {
public:
    BitwiseOrExpression(Expression *target){}
    ~BitwiseOrExpression(){}
    void appendElement(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_target;
    vector<Expression *> m_elements;
};

class BitwiseXorExpression : public Expression {
public:
    BitwiseXorExpression(Expression *target){}
    ~BitwiseXorExpression(){}
    void appendElement(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_target;
    vector<Expression *> m_elements;
};

class BitwiseAndExpression : public Expression {
public:
    BitwiseAndExpression(Expression *target){}
    ~BitwiseAndExpression(){}
    void appendElement(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_target;
    vector<Expression *> m_elements;
};

class EqualityExpression : public Expression {
public:
    enum { OP_EQ, OP_NEQ };
public:
    EqualityExpression(Expression *target){}
    ~EqualityExpression(){}
    void appendElement(int op, Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expression *m_target;
    vector<Expression *> m_elements;
};

class RelationalExpression : public Expression {
public:
    enum { OP_GT, OP_LT, OP_GTEQ, OP_LTEQ };
    
    RelationalExpression(Expression *target){}
    ~RelationalExpression(){}
    void appendElement(int op, Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expression *m_target;
    vector<Expression *> m_elements;
};

class ShiftExpression : public Expression {
public:
    enum { OP_LSHIFT, OP_RSHIFT };
public:
    ShiftExpression(Expression *target){}
    ~ShiftExpression(){}
    void appendElement(int op, Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expression *m_target;
    vector<Expression*> m_elements;
    
};


class AdditiveExpression : public Expression {
public:
    enum { OP_PLUS, OP_SUB };
public:
    AdditiveExpression(Expression *target){}
    ~AdditiveExpression(){}
    void appendElement(int op, Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expression *m_target;
    vector<Expression*> m_elements;
};

class MultiplicativeExpression : public Expression {
public:
    enum { OP_MUL, OP_DIV, OP_MODULO };
public:
    MultiplicativeExpression(Expression *target){}
    MultiplicativeExpression(){}
    void appendElement(int op, Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expression *m_target;
    vector<Expression*> m_elements;    
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(Expression *target){}
    ~UnaryExpression(){}
    void appendElement(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_primary;
    vector<Expression *> m_selectors;
    
};

class SelectorExpression : public Expression {
public:
    enum { DOT_SELECTOR, ARRAY_SELECTOR, FUNCTION_SELECTOR};
    
    SelectorExpression(const string &id){}
    SelectorExpression(Expression *expr){}
    SelectorExpression(){}
    ~SelectorExpression(){}
    void appendArgument(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_target;
    int m_type;
    string m_id;
    vector<Expression *> m_elements;
};

class PrimaryExpression : public Expression {
public:
    enum {
        T_THIS, 
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
    
    PrimaryExpression(int type):m_type(type){}
    PrimaryExpression(int type, const string &text):m_type(type),m_text(text){}
    PrimayExpression(int type, Expression *expr):m_type(type),m_expr(expr){}
    ~PrimaryExpression(){}
    void appendSelector(SelectorExpression *sel){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_type;
    string m_text;
    Expression *m_expr;
};

class NewExpression : public Expression {
public:
    NewExpression(const string &type):m_type(type){}
    ~NewExpression(){
        vector<Expression *>::iterator i = m_arguments.begin();
        for (; i != m_arguments.end(); i++) {
            delete *i;
        }
    }
    void appendArgument(Expression *expr){ 
        m_arguments.push_back(expr);
    }
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    string m_type;
    vector<Expression *> m_arguments;
   
};

class TypeExpression: public Expression {
public:
    enum { TE_SET, TE_MAP, TE_USER, TE_BUILTIN};
public:
    BasicTypeExpression(int type, const string &name):m_name1(name){}
    BasicTypeExpression(int type, const string &name1, const string &name2)
        :m_name1(name1),m_name2(name2){}
    ~BasicTypeExpression(){}
public:
    int m_type;
    string m_name1;
    string m_name2; 
    
};

class MapExpression : public Expression {
public:
    MapExpression(int type){}
    ~MapExpression(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void appendItem(mapItemExpression *item){ m_items.push_back(item);}
public:
    vector<MaptemExpression*> m_items;
};

class MapItemExpression: public Expression {
public:
    MapItemExpression(Expression *key, Expression *val):m_key(key),m_val(val){}
    ~MapItemExpression()  
public:
    Expression *m_key;
    Expression *m_val;
};

class SetExpression : public Expression {
public:
    ListExpression(ExpressionList *exprList){}
    ~ListExpression(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    ExpressionList *m_exprList;
};


#endif // TCC_EXPRESSION_H
