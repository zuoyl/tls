//
//  ASTBuilder.h
//  A toyable language compiler (like a simple c++)

#ifndef TCC_ASTBUILDER_H
#define TCC_ASTBUILDER_H

#include "AST.h"
#include "ASTVistor.h"
#include "Declaration.h"
#include "Expression.h"
class Node;

class ASTBuilder 
{
public:
    ASTBuilder(const string& path, const string& file);
    ~ASTBuilder();
    AST* build(Node* parseTree);
	
private:
    // compiliation unit 
    AST* handleDeclarations(Node* node); 
    // type declaration, such as class or enum 
    AST* handleTypeDeclaration(Node* node);
    // handle qualified name and qualified name list
    void handleQualifiedName(Node* node, QualifiedName& qualifiedName); 
    void handleQualifiedNameList(Node*node, vector<QualifiedName> &nameList); 
    // annotation
    AST* handleAnnotation(Node* node);
    void handleAnnotationElementValuePairs(Node* node,
            map<string, Annotation::ElementValue*>& elementValuePairs);
    void handleAnnotationElementValue(Node* node,
            Annotation::ElementValue& elementValue);
    // package 
    AST* handlePackageDeclaration(Node* node);
    // import 
    AST* handleImportDeclaration(Node* nodet);
    // constant
    AST* handleConstantDeclaration(Node* node);
    // enum
    AST* handleEnumDeclaration(Node* node);
   
    // class
    void handleClassModifier(Node* node, int&attribute, Annotation** annotation);
    AST* handleClassDeclaration(Node* node);
    AST* handleClassBodyDeclaration(Node* node, const string& cls);
    AST* handleClassMemberDeclaration(Node* node, const string& cls);    
   
    // method
    AST* handleMethodDeclaration(Node* node, const string& clsName);
    AST* handleMethodBlock(Node* node);
    AST* handleFormalParameters(Node* node); 
    AST* handleFormalParameterList(Node* node); 
    AST* handleFormalParameter(Node* node);
    
    // field
    AST* handleFieldDeclaration(Node* node, const string& clsName);
    AST* handleVariableDeclarators(Node* node, TypeDecl* typeDecl);
    AST* handleVariableInitializer(Node* node);
    void handleVariableModifier(Node* node, int& attribute,
            Annotation** annotation);
    void handleVariableDeclaratorId(Node* node, string& variableName, int& scalars);
    AST* handleLocalVariableDeclaration(Node* node); 
    // type 
    AST* handleType(Node* node);
    AST* handlePrimitiveType(Node* node);
    AST* handleClassTypeName(Node* node); 
    AST* handleMapType(Node* node);
    AST* handleArrayType(Node* node);
    AST* handleIterableObject(Node* node);
    AST* handleArrayInitializer(Node* node);
    AST* handleMapInitializer(Node* node);
    AST* handleMapPairItemInitializer(Node* node);
    
    // Statement
    AST* handleBlock(Node* node);
    AST* handleStatement(Node* node);
    AST* handleBlockStatement(Node* node);
    AST* handleLocalVariableDeclarationStatement(Node* node);
    AST* handleIfStatement(Node* node);
    AST* handleForInitializer(Node* node);
    AST* handleForStatement(Node* node);
    AST* handleForeachStatement(Node* node);
    AST* handleForeachVariable(Node* node);
    AST* handleSwitchStatement(Node* node);
    AST* handleDoStatement(Node* node);
    AST* handleWhileStatement(Node* node);
    AST* handleReturnStatement(Node* node);
    AST* handleBreakStatement(Node* node);
    AST* handleContinueStatement(Node* node);
    AST* handleAssertStatement(Node* node);
    
    // try statement
    AST* handleThrowStatement(Node* node);
    AST* handleTryStatement(Node* node);
    AST* handleCatchStatement(Node* node);
    AST* handleFinallyCatchStatement(Node* node);
    AST* handleArgumentList(Node* node);    
    AST* handleExprStatement(Node* node);
    AST* handleMethodCallStatement(Node* node);
    
    // expression
    AST* handleExpr(Node* node);
    AST* handleExprList(Node* node);
    AST* handleCompareExpr(Node* node);
    AST* handleAssignableExpr(Node* node);
    AST* handleConditionalExpr(Node* node);
    AST* handleLogicOrExpr(Node* node);
    AST* handleLogicAndExpr(Node* node);
    AST* handleBitwiseOrExpr(Node* node);
    AST* handleBitwiseXorExpr(Node* node);
    AST* handleBitwiseAndExpr(Node* node);
    AST* handleEqualityExpr(Node* node);
    AST* handleRelationalExpr(Node* node);
    AST* handleShiftExpr(Node* node);
    AST* handleAdditiveExpr(Node* node);
    AST* handleMultiplicativeExpr(Node* node);
    AST* handleUnaryExpr(Node* node);
    AST* handlePrefixExpr(Node* node);
    AST* handlePostfixExpr(Node* node);
    AST* handlePrimary(Node* node);
    AST* handleSelector(Node* node);
    AST* handleNewExpr(Node* node);
    
private:
    string m_path;
    string m_file;
};

#endif // TCC_ASTBUILDER_H
