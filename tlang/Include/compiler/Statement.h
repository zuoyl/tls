//
//  Statement.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_STATEMENT_H
#define TCC_STATEMENT_H

#include "compiler/AST.h"
#include "compiler/Expression.h"
#include "compiler/Label.h"


class ASTVisitor;

/// 'class Statement
/// Basic class for all statement
class Statement : public AST {
public:
	/// Constructor
    Statement():m_isIterable(false){}
	/// Destructor
    virtual ~Statement(){}
	/// Walker method
    virtual void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	/// Checker wether the statement is iterable statement
	virtual bool isIterable() { return false; }
	
	/// Get Iterable start point
	virtual Label getIterablStartPoint() { 
		return m_iterableStartPoint; 
	}
	/// Get iterable end point
	virtual Label getIterableEndPoint() { 
		return m_iterableEndPoint; 
	}
	/// Set Iteralbe start point
	virtual void setIterableStartPoint(Label &label) {
		m_iterableStartPoint = label;
	}
	/// Set Iterable end point
	virtual void setIterableEndPoint(Label &label) {
		m_iterableEndPoint = label;
	}
	
protected:
	Label m_iterableStartPoint;
	Label m_iterableEndPoint;
};

/// 'class ImportStatment
class ImportStatement:public Statement {
public:
    ImportStatement(vector<string> &packages){}
    ~ImportStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    vector<string> m_packages;
};

/// 'class BlockStatement
class BlockStatement : public Statement, public Scope {
public:
    BlockStatement(){}
    ~BlockStatement(){}
    void addStatement(Statement *stmt){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    vector<Statement *> m_statements;
};

/// 'class VariableDeclStatement
class VariableDeclStatement : public Statement {
public:
    VariableDeclStatement(Variable *var, Expression *expr){}
    ~VariableDeclStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Variable *m_var;
    Expression *m_expr;
};

/// 'class IfStaement
class IfStatement : public Statement {
public:
    IfStatement(Expression *condition, Statement *stm1t, Statement *stmt2){}
    ~IfStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_conditExpr;
    Statement *m_ifBlockStmt;
    Statement *m_elseBlockStmt;
};

/// 'class ForStatement
class ForStatement : public Statement {
public:
    ForStatement(Expression *expr1, Expression *expr2, ExpressionList *exprList, Statement *stmt){}
    ~ForStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    Expression *m_expr1;
    Expression *m_expr2;
    ExpressionList *m_exprList;
    Statement *m_stmt;
    Label m_loopLabel;
};

/// 'class ForEachStatement
// 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' (identifier|mapLiteral|setLitieral) ')' blockStatement
class ForEachStatement : public Statement {
public:
    enum {Object, MapObject, SetObject,}
public:
    ForEachStatement(){}
    ~ForEachStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    TypeSpec *m_typeSpec[2];
    string m_id[2];
    int m_varNumbers;
    
    int m_objectSetType;
    string m_objectSetName;
    
    Expression *m_expr;
    MapType *m_map;
    SetType *m_set;
    
    Statement *m_stmt;
    Label m_loopLabel;
};


/// 'class WhileStatement
class WhileStatement : public Statement {
public:
    WhileStatement(Expression *condit, Statement *stmt){}
    ~WhileStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    Label m_loopLabel;
    Expression *m_conditExpr;
    Statement *m_stmt;
};

/// 'class DoStatement
class DoStatement : public Statement {
public:
    DoStatement(Expression *expr, Statement *stmt){}
    ~DoStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    Label m_loopLabel;
    Expression *m_conditExpr;
    Statement *m_stmt;    
};
/// 'class ReturnStatement
class ReturnStatement : public Statement {
public:
    ReturnStatement(Expression *expr){}
    ~ReturnStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_resultExpr;
};

/// 'class AssertStatement
class AssertStatement : public Statement {
    
public:
    AssertStatement(Expression *expr){}
    ~AssertStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_resultExpr;
};

/// 'class ContinueStatement
class ContinueStatement : public Statement {
public:
    ContinueStatement(){}
    ~ContinueStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
};

/// 'class SwitchStatement
class SwitchStatement : public Statement {
public:
    SwitchStatement(Expression *expr){}
    ~SwitchStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void addCaseStatement(vector<Expression*> *exprList, Statement *stmt){}
    void addDefaultStatement(Statement *stmt){}
public:
    vector<std::pair<vector<Expression*>, Statement *> > m_cases;
    Statement *m_defaultStmt;
    Expression *m_conditExpr;  
};

/// 'class BreakStatement
class BreakStatement : public Statement {
public:
    BreakStatement(){}
    ~BreakStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
};

/// 'class ThrowStatement
class ThrowStatement : public Statement {
public:
    ThrowStatement(Expression *expr){}
    ~ThrowStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expression *m_resultExpr;
};

/// 'class CatchStatement
class CatchStatement : public Statement {
public:
    CatchStatement(const string &type, const string &id, BlockStatement *stmt){}
    ~CatchStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    string m_type;
    string m_id;
    BlockStatement *m_block;
};

/// 'class FinallyCatchStatement
class FinallyCatchStatement : public Statement {
public:
    FinallyCatchStatement(BlockStatement *stmt){}
    ~FinallyCatchStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    BlockStatement *m_block;
    
};

/// 'class TryStatement
class TryStatement : public Statement {
public:
    TryStatement(BlockStatement *block){}
    ~TryStatement(){}
    void addCatchPart(CatchStatement *stmt){}
    void setFinallyCatchPart(FinallyCatchStatement *finallyStmt){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    BlockStatement *m_blockStmt;
    vector<CatchStatement *> m_catchStmts;
    FinallyCatchStatement *m_finallyStmt;
};
    

#endif // TCC_STATEMENT_H
