//
//  TypeChecker.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TYPECHECKER_H
#define TCC_TYPECHECKER_H

#include "AST.h"
#include "ASTVistor.h"
#include "Scope.h"

class TypeBuilder : public ASTVisitor {
public:
    TypeBuilder();
    ~TypeBuilder();
    
    void build(AST* ast);
    bool isBuildComplete();
    
    // type
    void accept(TypeSpec &type);
    // struct
    void accept(Struct &st);
    // 
    // variable 
    void accept(Variable &var);
    
    // method
    void accept(Method &method);
    void accept(MethodParameterList &list);
    void accept(MethodParameter &para);
    void accept(MethodBlock &block);
    
    // class
    void accep(Class &cls);
    void accept(ClassBlock &block);
    
    // protocol
    void accept(Protocol &protocol);
    
    // statement
    void accept(Statement &stmt);
    void accept(ImportStatement &stmt);
    void accept(BlockStatement &stmt);
    void accept(VariableDeclStatement &stmt);
    void accept(IfStatement &stmt);
    void accept(WhileStatement &stmt);
    void accept(DoStatement &stmt);
    void accept(ForStatement &stmt);
    void accept(ForEachStatement &stmt);
    void accept(SwitchStatement &stmt);
    void accept(ContinueStatement &stmt);
    void accept(BreakStatement &stmt);
    void accept(ReturnStatement &stmt);
    void accept(ThrowStatement &stmt);
    void accept(AssertStatement &stmt);
    void accept(TryStatement &stmt);
    void accept(CatchStatement &stmt);
    void accept(FinallyCatchStatement &stmt);
    
    
    // expression
    void accept(Expr &expr);
    void accept(ExprList &list);
    void accept(BinaryOpExpr &expr);
    void accept(ConditionalExpr &expr);
    void accept(LogicOrExpr &expr);
    void accept(LogicAndExpr &expr);
    void accept(BitwiseOrExpr &expr);
    void accept(BitwiseXorExpr &expr);
    void accept(BitwiseAndExpr &expr);
    void accept(EqualityExpr &expr);
    void accept(RelationalExpr &expr);
    void accept(ShiftExpr &expr);
    void accept(AdditiveExpr &expr);
    void accept(MultiplicativeExpr &expr);
    void accept(UnaryExpr &expr);
    void accept(PrimaryExpr &expr);
    void accept(SelectorExpr &expr);
    void accept(MethodCallExpr &expr);
    
    // new
    void accept(NewExpr &expr);
    
    // map & list
    void accept(MapExpr &expr);
    void accept(MapItemExpr &expr);
    void accept(SetExpr &expr);

private:
    void enterScope(const string &name, Scope *scope);
    // get root scope
    Scope *getScope();
    void exitScope();
    
    bool hasSymbol(const string &name, bool nested = true);
    bool hasType(const string &name, bool nested = true);
    
    Symbol* getSymbol(const string &name, bool nested = true);
    Type* getType(const string &name, bool nested = true);
    
    void defineSymbol(Symbol *symbol);
    void defineType(Type *type);
    void walk(AST *node);
    

    // the following methods are for break/continue/return statement */
    void pushMethod(Method *func);
    void popCurrentMethod();
    Method* getCurrentMethod();

    void pushIterableStatement(Statement *stmt);
    void popIterableStatement();
    Statement* getCurrentIterableStatement();
    
    void pushBreakableStatement(Statement *stmt);
    void popBreakableStatement();
    Statement* getBreakableStatement();
    
    void pushClass(Class *cls);
    void popClass();
    Class* getCurrentClass();
    
    void handleSelectorExpr(Type *curType, string &curID, std::vector<SelectorExpr *> elements);
    
private:
    Scope *m_rootScope;
    Scope *m_curScope;
	string m_curScopeName;   
};

#endif // TCC_TYPECHECKER_H