//
//  ASTBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ASTBUILDER_H
#define TCC_ASTBUILDER_H

#include "Common.h"
#include "ASTVistor.h"
#include "AST.h"
class Node;
class QualifiedName;

class ASTBuilder 
{
public:
    ASTBuilder(const string& path, const string& file);
    ~ASTBuilder();
    AST* build(Node* parseTree);
	
private:
    // handle type such as classtype etc 
    AST* handleType(Node* node);
    
    // compiliation unit 
    AST* handleDeclarations(Node* node); 
   
    // type declaration, such as class or enum 
    AST* handleTypeDeclaration(Node* node);

    // handle qualified name and qualified name list
    void handleQualifiedName(Node* node, QualifiedName& qualifiedName); 
    void handleQualifiedNameList(Node*node, vector<QualifiedName> &nameList); 
    
    // annotation
    AST* handleAnnotationDeclaration(Node* node);
    void handleAnnotationElementValuePairs(Node* node,
            map<string, ElementValue*>& elementValuePairs);
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
    AST* handleClassDeclaration(Node* node);
    AST* handleClassBody(Node* node, const string& cls);
    AST* handleClassMemberDeclaration(Node* node, const string& cls);    
    
    // method
    AST* handleMethodDecl(Node* node);
    AST* handleMethodParameterList(Node* node);
    AST* handleMethodBlock(Node* node);
    AST* handleMethodNormalParameter(Node* node);
    AST* handleMethodDefaultParameter(Node* node);
   
    // type 
    AST* handleTypeDecl(Node* node);

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
    
    /// map &  set
    AST* handleMapExpr(Node* node);
    AST* handleSetExpr(Node* node);
    AST* handleMapItemExpr(Node* node);
	AST* handleMapLiteral(Node* node);
	AST* handleSetLiteral(Node* node);
    
    
    // Statement
    AST* handleStatement(Node* node);
    AST* handleBlockStatement(Node* node);
    AST* handleVarDeclStatement(Node* node);
    AST* handleIfStatement(Node* node);
    AST* handleForStatement(Node* node);
    AST* handleForEachStatement(Node* node);
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
private:
    string m_path;
    string m_file;

};

class QualifiedName
{
public:
    typedef  vector<string>::iterator iterator;

public:
    QualifiedName(){}
    ~QualifiedName{}
    
    void addElement(const string& name) 
    {
        m_names.push_back(name);
    }
    iterator begin() 
    {
        return m_names.begin();
    }
    iterator end() 
    {
        return m_names.end();
    }
    bool empty() 
    {
        return m_names.empty();
    }
    
    size_t size() 
    { 
        return m_names.size();
    }
    const string& operator [] (int index)
            return m_names[index];
    }
private:
    vector<string> m_names;
}




#endif // TCC_ASTBUILDER_H
