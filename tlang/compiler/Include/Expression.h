//
//  Expr.h
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

/// 'class Expr
///  Basic class for all sub expression
class Expr : public AST 
{
public:
	/// Constructor
  Expr():m_type(NULL), m_isConst(false){}
	/// Destructor
  virtual ~Expr(){}
	/// Walker method
  virtual void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	
	/// Checker to see wether the expression has valid result
	virtual bool hasValidValue() { return m_value.isValid(); }
    
public:
	/// Wether the expression is constant
  bool m_isConst;
	/// If the expression is constant, the result
  Value m_value;
	/// Type of Expr
  Type *m_type;
};

class ExprList : public AST {
public:
    ExprList(){}
    ~ExprList(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void appendExpr(Expr *expr){}
public:
    vector<Expr *> m_exprs;
};

class BinaryOpExpr : public Expr {
public:
    enum {BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_LSHIFT, BOP_RSHIFT};
    
public:
    BinaryOpExpr(const string &op, Expr *leftExpr, Expr *rightExpr){}
    ~BinaryOpExpr(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expr *m_left;
    Expr *m_right;
};

class ConditionalExpr : public Expr {
public:
    ConditionalExpr(){}
    ~ConditionalExpr(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
};

class LogicOrExpr : public Expr {
public:
    LogicOrExpr(Expr *target){}
    ~LogicOrExpr(){}
    void appendElement(Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}

public:
    Expr *m_target;
    vector<Expr *> m_elements;
};

class LogicAndExpr : public Expr {
public:
    LogicAndExpr(Expr *target){}
    ~LogicAndExpr(){}
    void appendElement(Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_target;
    vector<Expr *> m_elements;
    
};


class BitwiseOrExpr : public Expr {
public:
    BitwiseOrExpr(Expr *target){}
    ~BitwiseOrExpr(){}
    void appendElement(Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_target;
    vector<Expr *> m_elements;
};

class BitwiseXorExpr : public Expr {
public:
    BitwiseXorExpr(Expr *target){}
    ~BitwiseXorExpr(){}
    void appendElement(Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_target;
    vector<Expr *> m_elements;
};

class BitwiseAndExpr : public Expr {
public:
    BitwiseAndExpr(Expr *target){}
    ~BitwiseAndExpr(){}
    void appendElement(Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_target;
    vector<Expr *> m_elements;
};

class EqualityExpr : public Expr {
public:
    enum { OP_EQ, OP_NEQ };
public:
    EqualityExpr(Expr *target){}
    ~EqualityExpr(){}
    void appendElement(int op, Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expr *m_target;
    vector<Expr *> m_elements;
};

class RelationalExpr : public Expr {
public:
    enum { OP_GT, OP_LT, OP_GTEQ, OP_LTEQ };
    
    RelationalExpr(Expr *target){}
    ~RelationalExpr(){}
    void appendElement(int op, Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expr *m_target;
    vector<Expr *> m_elements;
};

class ShiftExpr : public Expr {
public:
    enum { OP_LSHIFT, OP_RSHIFT };
public:
    ShiftExpr(Expr *target){}
    ~ShiftExpr(){}
    void appendElement(int op, Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expr *m_target;
    vector<Expr*> m_elements;
    
};


class AdditiveExpr : public Expr {
public:
    enum { OP_PLUS, OP_SUB };
public:
    AdditiveExpr(Expr *target){}
    ~AdditiveExpr(){}
    void appendElement(int op, Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expr *m_target;
    vector<Expr*> m_elements;
};

class MultiplicativeExpr : public Expr {
public:
    enum { OP_MUL, OP_DIV, OP_MODULO };
public:
    MultiplicativeExpr(Expr *target){}
    MultiplicativeExpr(){}
    void appendElement(int op, Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_op;
    Expr *m_target;
    vector<Expr*> m_elements;    
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Expr *target){}
    ~UnaryExpr(){}
    void appendElement(Expr *expr){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_primary;
    vector<Expr *> m_selectors;
    
};

class SelectorExpr : public Expr {
public:
    enum { DOT_SELECTOR, ARRAY_SELECTOR, METHOD_SELECTOR};
    
    SelectorExpr(const string &id){}
    SelectorExpr(Expr *expr){}
    SelectorExpr(){}
    ~SelectorExpr(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_type;
    string m_identifier; // for .identifier
    Expr *m_arrayExpr;
    MethodCallExpr *m_methodCallExpr;
};

class PrimaryExpr : public Expr {
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
    
    PrimaryExpr(int type):m_type(type){}
    PrimaryExpr(int type, const string &text):m_type(type),m_text(text){}
    PrimaryExpr(int type, Expr *expr):m_type(type),m_expr(expr){}
    ~PrimaryExpr(){}
    void appendSelector(SelectorExpr *sel){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    int m_type;
    string m_text;
    Expr *m_expr;
};

class NewExpr : public Expr {
public:
    NewExpr(const string &type):m_type(type){}
    ~NewExpr(){
        vector<Expr *>::iterator i = m_arguments.begin();
        for (; i != m_arguments.end(); i++) {
            delete *i;
        }
    }
    void appendArgument(Expr *expr){ 
        m_arguments.push_back(expr);
    }
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    string m_type;
    vector<Expr *> m_arguments;
   
};

class TypeExpr: public Expr {
public:
    enum { TE_SET, TE_MAP, TE_USER, TE_BUILTIN};
public:
    TypeExpr(int type, const string &name):m_name1(name){}
    TypeExpr(int type, const string &name1, const string &name2)
        :m_name1(name1),m_name2(name2){}
    ~TypeExpr(){}
public:
    int m_type;
    string m_name1;
    string m_name2; 
    
};

class MapItemExpr;
class MapExpr : public Expr {
public:
		MapExpr(){}
    MapExpr(int type){}
    ~MapExpr(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void appendItem(MapItemExpr *item){ m_items.push_back(item);}
public:
    vector<MapItemExpr*> m_items;
};

class MapItemExpr: public Expr {
public:
    MapItemExpr(Expr *key, Expr *val):m_key(key),m_val(val){}
    ~MapItemExpr(){}  
public:
    Expr *m_key;
    Expr *m_val;
};

class SetExpr : public Expr {
public:
    SetExpr(ExprList *exprList){}
    SetExpr(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    ExprList *m_exprList;
};


#endif // TCC_EXPRESSION_H
