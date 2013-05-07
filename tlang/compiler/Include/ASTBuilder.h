//
//  ASTBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ASTBUILDER_H
#define TCC_ASTBUILDER_H

#include "Common.h"
#include "ASTVistor.h"
#include "AST.h"
class Node;

class ASTBuilder {
public:
    ASTBuilder();
    ~ASTBuilder();
    AST* build(Node *parseTree);
	
private:
    // import
    AST* handleImportDeclaration(Node *nodet);
    
    // type
    AST* handleTypeDeclaration(Node *node);
    
    // struct
    AST* handleStructDeclaration(Node *node);
    
    // global variable
    AST* handleVarDeclaration(Node *node);
    AST* handleGlobalVarDeclaration(Node *node);
    
    // method
    AST* handleMethodDeclaration(Node *node);
    AST* handleMethodParameters(Node *node);
    AST* handleMethodBlock(Node *node);
    AST* handleMethodNormalParameter(Node *node);
    AST* handleMethodDefaultParameter(Node *node);
    
    // class
    AST* handleClassDeclaration(Node *node);
    AST* handleClassBlock(Node *node);
    AST* handleClassVariable(Node *node);
    AST* handleClassMethod(Node *node);
    
    
    // interface
    AST* handleProtocolDeclaration(Node* node);
    
    // expression
    AST* handleExpr(Node *node);
    AST* handleExprList(Node *node);
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
    
    AST* handleExpreStatement(Node *node);
    AST* handleMethodCallStatement(Node *node);
};

#endif // TCC_ASTBUILDER_H
