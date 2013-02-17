//
//  Expression.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_EXPRESSION_H
#define TCC_EXPRESSION_H

#include "Common.h"
#include "AST/AST.h"
#include "AST/ASTVistor.h"
#include "Runtime/Type.h"
#include "Runtime/Value.h"
#include "Runtime/Label.h"

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
    
    PrimaryExpression(int type){}
    PrimaryExpression(int type, const string &text){}
    ~PrimaryExpression(){}
    void appendSelector(SelectorExpression *sel){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_type;
    string m_text;
};

class NewExpression : public Expression {
public:
    NewExpression(const string &type){}
    ~NewExpression(){}
    void appendArgument(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
   
};

class MapExpression : public Expression {
public:
    enum { MAP_STRING, MAP_ID, MAP_NUMBER, MAP_HEX_NUMBER};
    
    MapExpression(int type){}
    ~MapExpression(){}
    void appendItem(const string &key, Expression *value){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    map<string, Expression*> m_maps;
};

class ListExpression : public Expression {
public:
    ListExpression(ExpressionList *exprList){}
    ~ListExpression(){}
    void appendItem(Expression *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    
public:
    vector<Expression *> m_items;
};


#endif // TCC_EXPRESSION_H
