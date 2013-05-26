//
//  IRBuilder.h -walk ast to generate the IR instructions
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_MLBUILDER_H
#define TCC_MLBUILDER_H

#include "Common.h"
#include "ASTVistor.h"
#include "IRNodeBlock.h"
#include "IREmiter.h"

class AST;
class Label;
class Value;
class Frame;

class IRBuilder : public ASTVisitor 
{
public:
    IRBuilder(const string &path, const string &file);
    ~IRBuilder();
    
    void build(AST *ast, IRBlockList *blocks);    
    
    // struct
    void accept(Struct &type);
    
    // variable 
    void accept(TypeSpec &spec);
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
    void build(AST *ast);
    void makeAllGlobalVariables();
    void generateMethod(Method &func);
    void makeMethodName(Method &func, string& name);
    int  getLinkAddress(Method &func);
    Value* handleSelectorExpr(PrimaryExpr &primExpr, vector<SelectorExpr*> &selectors);    
    void callObjectMethod(const string &objectName, const string &method, vector<Value> &arguments, Value &result){}
	/// Enter a new scope
	/// @param name the scope's name
	/// @param scope the new scope
    void enterScope(const string &name, Scope *scope);
	/// Exit current scope
    void exitScope();
    
    Symbol* getSymbol(const string &name, bool nested = true);
    Type* getType(const string &name, bool nested = true);
    
    /// helper method for iterable statement
    void pushIterablePoint(Statement *stmt);
    void popIterablePoint();
    Statement *getCurIterablePoint();
    void clearIterablePoint();
private:
    Scope *m_rootScope;
    Scope *m_curScope;
	string m_curScopeName;
    IRBlockList* m_blocks;
    vector<Variable *> m_globalVars;
    vector<Statement *> m_iterablePoints;
    IREmiter m_ir;
};





#endif
