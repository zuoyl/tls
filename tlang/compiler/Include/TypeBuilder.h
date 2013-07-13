//
//  TypeChecker.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TYPECHECKER_H
#define TCC_TYPECHECKER_H

#include "AST.h"
#include "ASTVistor.h"
#include "Scope.h"
#include "Type.h"
#include "BuiltinType.h"

class TypeBuilder : public ASTVistor {
public:
    TypeBuilder(const string& path, const string& file);
    ~TypeBuilder();
    
    bool isBuildComplete();
    void build(AST* ast, TypeDomain* typeDomain);

    void accept(Declaration& decl);
    void accept(PackageDeclaration& decl);
    void accept(ImportDeclaration& decl);
    void accept(Annotation& annotation);
    void accept(Class& cls);
    void accept(ClassBlock& block);
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
    void enterScope(Scope* scope);
    // get root scope
    Scope* getScope();
    void exitScope();
    
    bool hasObject(const string& name, bool nested = true);
    bool hasType(const string& name, bool nested = true);
    
    Object* getObject(const string& name, bool nested = true);
    Type* getType(const string& name, bool nested = true);
    Type* getType(const string& clsName, const string& name); 
    Type* getType(TypeDecl* spec, bool nested = true);    
    void defineObject(Object* object);
    void defineType(Type* type);
    void walk(AST* node);
    

    // the following methods are for break/continue/return statement* /
    void pushMethod(Method* method);
    void popMethod();
    Method* getCurrentMethod();

    void pushIterableStatement(Statement* stmt);
    void popIterableStatement();
    Statement* getCurrentIterableStatement();
    
    void pushBreakableStatement(Statement* stmt);
    void popBreakableStatement();
    Statement* getCurrentBreakableStatement();
    
    void pushClass(Class* cls);
    void popClass();
    Class* getCurrentClass();
    
    void handleSelectorExpr(PrimaryExpr& primExpr, vector<SelectorExpr* >& elements);
private:
    string m_file;
    string m_path;
    string m_fullName;
    Scope* m_rootScope;
    Scope* m_curScope;
    stack<Method* > m_methods;
    stack<Statement* > m_iterableStmts;
    stack<Statement* > m_breakableStmts;
    stack<Class* > m_classes;
    TypeDomain* m_typeDomain;
    bool m_isIncludedFile;
    map<string, bool> m_headerFiles;
    // for static class data member and method
    vector<Variable *> m_globalVars;
};

#endif // TCC_TYPECHECKER_H
