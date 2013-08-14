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

class IRBuilder : public ASTVistor 
{
public:
    IRBuilder(const string& path, const string& file);
    ~IRBuilder();
    
    void build(AST* ast, IRBlockList* blocks, TypeDomain* typeDomain);    
    void accept(Declaration& decl);
    void accept(PackageDeclaration& decl);
    void accept(ImportDeclaration& decl);
    void accept(Annotation& annotation);
    void accept(Class& cls);
    void accept(Variable& variable);
    void accept(TypeDecl& type);
    void accept(Method& method);
    void accept(FormalParameterList& list);
    void accept(FormalParameter& para);
    void accept(MethodBlock& block);
    void accept(ArgumentList& arguments);
    void accept(IterableObjectDecl& object);
    void accept(MapInitializer& mapInitializer);
    void accept(MapPairItemInitializer& mapPairItemInitializer);
    void accpet(ArrayInitializer& arrayInitializer);
    
    // statement
    void accept(Block& block); 
    void accept(Statement& stmt);
    void accept(LocalVariableDeclarationStatement& stmt);
    void accept(BlockStatement& stmt);
    void accept(IfStatement& stmt);
    void accept(WhileStatement& stmt);
    void accept(DoStatement& stmt);
    void accept(ForStatement& stmt);
    void accept(SwitchStatement& stmt);
    void accept(ForeachStatement& stmt);
    void accept(ContinueStatement& stmt);
    void accept(BreakStatement& stmt);
    void accept(ReturnStatement& stmt);
    void accept(ThrowStatement& stmt);
    void accept(AssertStatement& stmt);
    void accept(TryStatement& stmt);
    void accept(CatchStatement& stmt);
    void accept(FinallyCatchStatement& stmt);
    void accept(ExprStatement& stmt); 
    
    // expression
    void accept(Expr& expr);
    void accept(ExprList& list);
    void accept(AssignmentExpr& expr);
    void accept(ConditionalExpr& expr);
    void accept(LogicOrExpr& expr);
    void accept(LogicAndExpr& expr);
    void accept(BitwiseOrExpr& expr);
    void accept(BitwiseXorExpr& expr);
    void accept(BitwiseAndExpr& expr);
    void accept(EqualityExpr& expr);
    void accept(RelationalExpr& expr);
    void accept(ShiftExpr& expr);
    void accept(AdditiveExpr& expr);
    void accept(MultiplicativeExpr& expr);
    void accept(UnaryExpr& expr);
    void accept(PrimaryExpr& expr);
    void accept(MethodCallExpr& expr);
    void accept(SelectorExpr& expr);
    void accept(NewExpr& expr);
    
    
private:
    void build(AST* ast);
    void makeAllGlobalVariables();
    void makeMethodName(Method& func, string& name);
    int  getLinkAddress(Method& func);
    Value* handleSelectorExpr(PrimaryExpr& primExpr, vector<SelectorExpr*>& selectors);    
    void callObjectMethod(const string& objectName, 
                const string& method, 
                vector<Value>& arguments, 
                Value& result){}
	
    void constructObject(int storageType, int offset, Type* type){} 
    
    /// Enter a new scope
	/// @param name the scope's name
	/// @param scope the new scope
    void enterScope(const string& name, Scope* scope);
	/// Exit current scope
    void exitScope();
    
    Object* getObject(const string& name, bool nested = true);
    Type* getType(const string& name, bool nested = true);
    Type* getType(const string& clsName, const string& name); 
    /// helper method for iterable statement
    void pushIterablePoint(Statement* stmt);
    void popIterablePoint();
    Statement* getCurIterablePoint();
    void clearIterablePoint();
    Class* getCurrentClass();
    void pushClass(Class* cls);
    void popClass();
    // alloc object in heap
    int allocHeapObject(Object* object);
private:
    Scope* m_rootScope;
    Scope* m_curScope;
	string m_curScopeName;
    IRBlockList* m_blocks;
    vector<Variable* > m_globalVars;
    vector<Statement* > m_iterablePoints;
    IREmiter m_ir;
    TypeDomain* m_typeDomain;
    stack<Class*> m_classStack;
};





#endif
