//
//  MLBuilder.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_MLBUILDER_H
#define TCC_MLBUILDER_H

#include <Common.h>
#include <AST/ASTVistor.h>
#include "IRNodeBlock.h"


class AST;
class Label;
class Value;
class Frame;

class IRBuilder : public ASTVisitor {
public:
    IRBuilder();
    ~IRBuilder();
    
    void build(AST *ast, IRBlockList *blocks);    
    
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
    void accept(MapItemExpression &expr);
    void accept(SetExpression &expr);
    
private:
    void build(AST *ast);
    void makeGlobalVariables();
    void generateFunction(Function &func);
    void makeFunctionName(Function &func, string& name);
    int  getLinkAddress(Function &func);
    
	
	/// Alloc local in current frame
	/// @param size the size of local
	/// @ret newly alloced local
    Value* allocValue(int size);
	
	/// Alloc local in register or in frame(4bye only)
	/// @param inreg wethere in frame or in regisgter
	/// @reg newly alloced local
	Value* allocValue(bool inreg);
	
	/// Alloc local according to inreg parameter.
	/// @param type Type's of newly alloced local
	/// @param inreg true for local in register, else in frame
	/// @ret newly alloced local
    Value* allocValue(Type *type, bool inreg = false);
	
	/// Enter a new scope
	/// @param name the scope's name
	/// @param scope the new scope
    void enterScope(const string &name, Scope *scope);
	/// Exit current scope
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
    IRBlockList* m_blocks;
    vector<Variable *> m_globalVars;

};





#endif
