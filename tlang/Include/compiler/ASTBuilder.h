//
//  ASTBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ASTBUILDER_H
#define TCC_ASTBUILDER_H

#include "compiler/Common.h"

class AST;
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
    
    // function
    AST* handleFunctionDeclaration(Node *node);
    AST* handleFunctionParameters(Node *node);
    AST* handleFunctionBlock(Node *node);
    AST* handleFunctionNormalParameter(Node *node);
    AST* handleFunctionDefaultParameter(Node *node);
    
    // class
    AST* handleClassDeclaration(Node *node);
    AST* handleClassBlock(Node *node);
    AST* handleClassVariable(Node *node);
    AST* handleClassFunction(Node *node);
    
    
    // interface
    AST* handleProtocolDeclaration(Node* node);
    
    // expression
    AST* handleExpression(Node *node);
    AST* handleExpressionList(Node *node);
    AST* handleAssignableExpression(Node *node);
    AST* handleConditionalExpression(Node *node);
    AST* handleLogicOrExpression(Node *node);
    AST* handleLogicAndExpression(Node *node);
    AST* handleBitwiseOrExpression(Node *node);
    AST* handleBitwiseXorExpression(Node *node);
    AST* handleBitwiseAndExpression(Node *node);
    AST* handleEqualityExpression(Node *node);
    AST* handleRelationalExpression(Node *node);
    AST* handleShiftExpression(Node *node);
    AST* handleAdditiveExpression(Node *node);
    AST* handleMultiplicativeExpression(Node *node);
    AST* handleUnaryExpression(Node *node);
    AST* handlePrefixExpression(Node *node);
    AST* handlePostfixExpression(Node *node);
    AST* handlePrimary(Node *node);
    AST* handleSelector(Node *node);
    AST* handleNewExpression(Node *node);
    
    /// map & set
    AST* handleMapExpression(Node *node);
    AST* handleSetExpression(Node *node);
    AST* handleMapItemExpression(Node *node);
    
    
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
    AST* handleFunctionCallStatement(Node *node);
};

#endif // TCC_ASTBUILDER_H