//
//  ASTXml.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ASTXML_H
#define TCC_ASTXML_H

#include "AST.h"
#include "ASTVistor.h"
#include "Scope.h"

#define LIBXML_TREE_ENABLED
#define LIBXML_OUTPUT_ENABLED
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

class ASTXml : public ASTVisitor 
{
public:
    ASTXml(const string &path, const string &file);
    ~ASTXml();
    
    void build(AST* ast);
    
    // type
    void accept(TypeSpec &type);
    // variable 
    void accept(Variable &var);
    
    // method
    void accept(Method &method);
    void accept(MethodParameterList &list);
    void accept(MethodParameter &para);
    void accept(MethodBlock &block);
    
    // class
    void accep(Class &cls);
    void accept(ClassBlock &block);
    
    // statement
    void accept(Statement &stmt);
    void accept(IncludeStatement &stmt);
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
    void accept(ExprStatement &stmt); 
    
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
    void handleSelectorExpr(PrimaryExpr &primExpr, vector<SelectorExpr *> &elements);
    void walk(AST *node);
    void pushXmlNode(xmlNodePtr node);
    void popXmlNode();
    // for xml output
    xmlNodePtr m_rootXmlNode;
    xmlDocPtr m_xmlDoc;
    xmlNodePtr m_curXmlNode;
    stack<xmlNodePtr> m_xmlNodes; 
    string m_file;
    string m_path;
};

#endif // TCC_ASTXML_H
