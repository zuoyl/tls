//
//  ASTBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ASTBUILDER_H
#define TCC_ASTBUILDER_H

#include "Common.h"
#include "ASTVistor.h"
#include "AST.h"
class Node;

class ASTBuilder 
{
public:
    ASTBuilder(const string &path, const string &file);
    ~ASTBuilder();
    AST* build(Node *parseTree);
	
private:
    AST* handleDecl(Node *node); 
    // include 
    AST* handleIncludeDecl(Node *nodet);
    
    // class
    AST* handleClassDecl(Node *node);
    AST* handleClassBlock(Node *node, const string &cls);
    
    // variable 
    AST* handleVarDecl(Node *node);    
   
    // method
    AST* handleMethodDecl(Node *node);
    AST* handleMethodImplementation(Node *node); 
    AST* handleMethodParameterList(Node *node);
    AST* handleMethodBlock(Node *node);
    AST* handleMethodNormalParameter(Node *node);
    AST* handleMethodDefaultParameter(Node *node);
   
    // for typeDecl 
    AST* handleTypeDecl(Node *node);

    // expression
    AST* handleExpr(Node *node);
    AST* handleExprList(Node *node);
    AST* handleCompareExpr(Node *node);
    AST* handleAssignableExpr(Node *node);
    AST* handleConditionalExpr(Node *node);
    AST* handleLogicOrExpr(Node *node);
    AST* handleLogicAndExpr(Node *node);
    AST* handleBitwiseOrExpr(Node *node);
    AST* handleBitwiseXorExpr(Node *node);
    AST* handleBitwiseAndExpr(Node *node);
    AST* handleEqualityExpr(Node *node);
    AST* handleRelationalExpr(Node *node);
    AST* handleShiftExpr(Node *node);
    AST* handleAdditiveExpr(Node *node);
    AST* handleMultiplicativeExpr(Node *node);
    AST* handleUnaryExpr(Node *node);
    AST* handlePrefixExpr(Node *node);
    AST* handlePostfixExpr(Node *node);
    AST* handlePrimary(Node *node);
    AST* handleSelector(Node *node);
    AST* handleNewExpr(Node *node);
    
    /// map & set
    AST* handleMapExpr(Node *node);
    AST* handleSetExpr(Node *node);
    AST* handleMapItemExpr(Node *node);
	AST* handleMapLiteral(Node *node);
	AST* handleSetLiteral(Node *node);
    
    
    // Statement
    AST* handleStatement(Node *node);
    AST* handleBlockStatement(Node *node);
    AST* handleVarDeclStatement(Node *node);
    AST* handleIfStatement(Node *node);
    AST* handleForStatement(Node *node);
    AST* handleForEachStatement(Node *node);
    AST* handleSwitchStatement(Node *node);
    AST* handleDoStatement(Node *node);
    AST* handleWhiletatement(Node *node);
    AST* handleReturnStatement(Node *node);
    AST* handleBreakStatement(Node *node);
    AST* handleContinueStatement(Node *node);
    AST* handleAssertStatement(Node *node);
    
    // try statement
    AST* handleThrowStatement(Node *node);
    AST* handleTryStatement(Node *node);
    AST* handleCatchStatement(Node *node);
    AST* handleFinallyCatchStatement(Node *node);
    AST* handleArgumentList(Node *node);    
    AST* handleExprStatement(Node *node);
    AST* handleMethodCallStatement(Node *node);
private:
    string m_path;
    string m_file;

};

#endif // TCC_ASTBUILDER_H
