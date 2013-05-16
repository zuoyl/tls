//
//  Statement.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_STATEMENT_H
#define TCC_STATEMENT_H

#include "AST.h"
#include "Expression.h"
#include "Label.h"
#include "Type.h"
#include "Location.h"

class ASTVisitor;

/// 'class Statement
/// Basic class for all statement
class Statement : public AST 
{
public:
	/// Constructor
    Statement(const Location &location):AST(location),m_isIterable(false){}
	/// Destructor
    virtual ~Statement(){}
	/// Walker method
    virtual void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	/// Checker wether the statement is iterable statement
	virtual bool isIterable() { return false; }
	
	/// Get Iterable start point
	virtual Label& getIterableStartPoint() { 
		return m_iterableStartPoint; 
	}
	/// Get iterable end point
	virtual Label& getIterableEndPoint() { 
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
	bool  m_isIterable;
};

/// 'class ImportStatment
class ImportStatement:public Statement 
{
public:
    ImportStatement(vector<string> &packages, const Location &location)
        :Statement(location){}
    ~ImportStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    vector<string> m_packages;
};

/// 'class BlockStatement
class BlockStatement : public Statement, public Scope 
{
public:
    BlockStatement(const Location &location)
        :Statement(location), Scope("block", NULL){}
    ~BlockStatement(){}
    void addStatement(Statement *stmt){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    vector<Statement *> m_statements;
};

/// 'class VariableDeclStatement
class VariableDeclStatement : public Statement 
{
public:
    VariableDeclStatement(Variable *var, Expr *expr, const Location &location)
        :Statement(location){}
    ~VariableDeclStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Variable *m_var;
    Expr *m_expr;
};

/// 'class IfStaement
class IfStatement : public Statement 
{
public:
    IfStatement(Expr *condition, Statement *stm1t, Statement *stmt2, const Location &location)
        :Statement(location){}
    ~IfStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_conditExpr;
    Statement *m_ifBlockStmt;
    Statement *m_elseBlockStmt;
};

/// 'class ForStatement
class ForStatement : public Statement 
{
public:
    ForStatement(Expr *expr1, Expr *expr2, ExprList *exprList, Statement *stmt, const Location &location)
        :Statement(location){}
    ~ForStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    Expr *m_expr1;
    Expr *m_expr2;
    ExprList *m_exprList;
    Statement *m_stmt;
    Label m_loopLabel;
};

/// 'class ForEachStatement
// 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' (identifier|mapLiteral|setLitieral) ')' blockStatement
class ForEachStatement : public Statement 
{
public:
    enum {Object, MapObject, SetObject};
public:
    ForEachStatement(const Location &location)
        :Statement(location){}
    ~ForEachStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    TypeSpec *m_typeSpec[2];
    string m_id[2];
    int m_varNumbers;
    
    int m_objectSetType;
    string m_objectSetName;
    
    Expr *m_expr;
    MapType *m_map;
    SetType *m_set;
    
    Statement *m_stmt;
    Label m_loopLabel;
};


/// 'class WhileStatement
class WhileStatement : public Statement 
{
public:
    WhileStatement(Expr *condit, Statement *stmt, const Location &location)
        :Statement(location){}
    ~WhileStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    Label m_loopLabel;
    Expr *m_conditExpr;
    Statement *m_stmt;
};

/// 'class DoStatement
class DoStatement : public Statement 
{
public:
    DoStatement(Expr *expr, Statement *stmt, const Location &location)
        :Statement(location){}
    ~DoStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
	bool isIterable() { return true; }
public:
    Label m_loopLabel;
    Expr *m_conditExpr;
    Statement *m_stmt;    
};
/// 'class ReturnStatement
class ReturnStatement : public Statement 
{
public:
    ReturnStatement(Expr *expr, const Location &location)
        :Statement(location){}
    ~ReturnStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_resultExpr;
};

/// 'class AssertStatement
class AssertStatement : public Statement 
{
public:
    AssertStatement(Expr *expr, const Location &location)
        :Statement(location){}
    ~AssertStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_resultExpr;
};

/// 'class ContinueStatement
class ContinueStatement : public Statement 
{
public:
    ContinueStatement(const Location &location)
        :Statement(location){}
    ~ContinueStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
};

/// 'class SwitchStatement
class SwitchStatement : public Statement 
{
public:
    SwitchStatement(Expr *expr, const Location &location)
        :Statement(location){}
    ~SwitchStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
    void addCaseStatement(vector<Expr*> *exprList, Statement *stmt){}
    void addDefaultStatement(Statement *stmt){}
public:
    vector<std::pair<vector<Expr*>, Statement *> > m_cases;
    Statement *m_defaultStmt;
    Expr *m_conditExpr;  
};

/// 'class BreakStatement
class BreakStatement : public Statement 
{
public:
    BreakStatement(const Location &location)
        :Statement(location){}
    ~BreakStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
};

/// 'class ThrowStatement
class ThrowStatement : public Statement 
{
public:
    ThrowStatement(Expr *expr, const Location &location)
        :Statement(location){}
    ~ThrowStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    Expr *m_resultExpr;
};

/// 'class CatchStatement
class CatchStatement : public Statement
{
public:
    CatchStatement(const string &type, const string &id, 
        BlockStatement *stmt, const Location &location)
        :Statement(location){}
    ~CatchStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    string m_type;
    string m_id;
    BlockStatement *m_block;
};

/// 'class FinallyCatchStatement
class FinallyCatchStatement : public Statement
{
public:
    FinallyCatchStatement(BlockStatement *stmt, const Location &location)
        :Statement(location){}
    ~FinallyCatchStatement(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    BlockStatement *m_block;
    
};

/// 'class TryStatement
class TryStatement : public Statement
{
public:
    TryStatement(BlockStatement *block, const Location &location)
        :Statement(location){}
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
