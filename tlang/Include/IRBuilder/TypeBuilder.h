//
//  TypeChecker.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TYPECHECKER_H
#define TCC_TYPECHECKER_H

#include "AST.h"
#include "ASTVistor.h"
#include <Runtime/Scope.h>

class TypeBuilder : public ASTVisitor {
public:
    TypeBuilder();
    ~TypeBuilder();
    
    void build(AST* ast);
    
    // struct
    void accept(Struct &type);
    // variable 
    void accept(Variable &var);
    
    // function
    void accept(Function &function);
    void accept(FunctionParameterList &list);
    void accept(FunctionParameter &para);
    void accept(FunctionBlock &block);
    
    // class & interface
    void accep(Class &cls);
    void accept(ClassBlock &block);
    void accept(Interface &interface);
    
    // statement
    void accept(Statement &stmt);
    void accept(ImportStatement &stmt);
    void accept(BlockStatement &stmt);
    void accept(VariableDeclStatement &stmt);
    void accept(IfStatement &stmt);
    void accept(WhileStatement &stmt);
    void accept(DoStatement &stmt);
    void accept(ForStatement &stmt);
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
    void accept(Expression &expr);
    void accept(ExpressionList &list);
    void accept(BinaryOpExpression &expr);
    void accept(ConditionalExpression &expr);
    void accept(LogicOrExpression &expr);
    void accept(LogicAndExpression &expr);
    void accept(BitwiseOrExpression &expr);
    void accept(BitwiseXorExpression &expr);
    void accept(BitwiseAndExpression &expr);
    void accept(EqualityExpression &expr);
    void accept(RelationalExpression &expr);
    void accept(ShiftExpression &expr);
    void accept(AdditiveExpression &expr);
    void accept(MultiplicativeExpression &expr);
    void accept(UnaryExpression &expr);
    void accept(PrimaryExpression &expr);
    void accept(SelectorExpression &expr);
    
    // new
    void accept(NewExpression &expr);
    
    // map & list
    void accept(MapExpression &expr);
    void accept(ListExpression &expr);

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
    
private:
    Scope *m_rootScope;
    Scope *m_curScope;
	string m_curScopeName;
    
};

#endif // TCC_TYPECHECKER_H
