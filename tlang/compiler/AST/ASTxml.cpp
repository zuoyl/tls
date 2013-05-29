//
//  ASTXml.cpp
//  A toyable language compiler (like a simple c++)


#include "ASTxml.h"
#include "Compile.h"
#include "Error.h"
#include "OSWraper.h"
#include "Frame.h"
#include "Struct.h"
#include "Variable.h"
#include "Class.h"
#include "Method.h"
#include "Expression.h"
#include "Statement.h"
#include "TypeBuilder.h"

ASTXml::ASTXml(const string &path, const string &file)
{
    m_file = file;
    m_path = path;
    // create the xml root node
    CompileOption &option = CompileOption::getInstance();
    if (option.isOutputAST()) {
        m_xmlDoc = xmlNewDoc(BAD_CAST "1.0");
        m_xmlRootNode = xmlNewNode(NULL, BAD_CAST "root");
        m_curXmlNode = m_xmlRootNode;
    }
    else {
        m_xmlDoc = NULL;
        m_xmlRootNode = NULL;
        m_curXmlNode = NULL;
    }
}
ASTXml::~ASTXml()
{
    // free resource for xml
    CompileOption &option = CompileOption::getInstance();
    if (option.isOutputAST()) {
        xmlFreeDoc(m_xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    }
}
void ASTXml::build(AST* ast)
{
    if (!ast)
        return;
    // check wether the option is specified
    CompileOption &option = CompileOption::getInstance();
    if (!option.isOutputAST())
        return;
    // walk through the ast tre
    ast->walk(this); 
    // save the xml file
    unsigned found = m_file.find_last_of(".");
    string fileName = m_file.substr(0, found);
    fileName += "_ast";
    fileName += ".xml";
    xmlSaveFile(fileName.c_str(), m_xmlDoc);
} 
// type
void ASTXml::accept(TypeSpec &type)
{}
// 
// variable 
void ASTXml::accept(Variable &var)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Variable");
    // set attribute
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST var.m_name.c_str());
    xmlAddChild(m_curXmlNode, xmlNode);
}

// method
void ASTXml::accept(Method &method)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Method");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(MethodParameterList &list)
{}
void ASTXml::accept(MethodParameter &para)
{}
void ASTXml::accept(MethodBlock &block)
{}

// class
void ASTXml::accep(Class &cls)
{}
void ASTXml::accept(ClassBlock &block)
{}

// protocol
void ASTXml::accept(Protocol &protocol)
{}

// statement
void ASTXml::accept(Statement &stmt)
{}
void ASTXml::accept(IncludeStatement &stmt)
{}
void ASTXml::accept(BlockStatement &stmt)
{}
void ASTXml::accept(VariableDeclStatement &stmt)
{}
void ASTXml::accept(IfStatement &stmt)
{}
void ASTXml::accept(WhileStatement &stmt)
{}
void ASTXml::accept(DoStatement &stmt)
{}
void ASTXml::accept(ForStatement &stmt)
{}
void ASTXml::accept(ForEachStatement &stmt)
{}
void ASTXml::accept(SwitchStatement &stmt)
{}
void ASTXml::accept(ContinueStatement &stmt)
{}
void ASTXml::accept(BreakStatement &stmt)
{}
void ASTXml::accept(ReturnStatement &stmt)
{}
void ASTXml::accept(ThrowStatement &stmt)
{}
void ASTXml::accept(AssertStatement &stmt)
{}
void ASTXml::accept(TryStatement &stmt)
{}
void ASTXml::accept(CatchStatement &stmt)
{}
void ASTXml::accept(FinallyCatchStatement &stmt)
{}


// expression
void ASTXml::accept(Expr &expr)
{}
void ASTXml::accept(ExprList &list)
{}
void ASTXml::accept(BinaryOpExpr &expr)
{}
void ASTXml::accept(ConditionalExpr &expr)
{}
void ASTXml::accept(LogicOrExpr &expr)
{}
void ASTXml::accept(LogicAndExpr &expr)
{}
void ASTXml::accept(BitwiseOrExpr &expr)
{}
void ASTXml::accept(BitwiseXorExpr &expr)
{}
void ASTXml::accept(BitwiseAndExpr &expr)
{}
void ASTXml::accept(EqualityExpr &expr)
{}
void ASTXml::accept(RelationalExpr &expr)
{}
void ASTXml::accept(ShiftExpr &expr)
{}
void ASTXml::accept(AdditiveExpr &expr)
{}
void ASTXml::accept(MultiplicativeExpr &expr)
{}
void ASTXml::accept(UnaryExpr &expr)
{}
void ASTXml::accept(PrimaryExpr &expr)
{}
void ASTXml::accept(SelectorExpr &expr)
{}
void ASTXml::accept(MethodCallExpr &expr)
{}

// new
void ASTXml::accept(NewExpr &expr)
{}

// map & list
void ASTXml::accept(MapExpr &expr)
{}
void ASTXml::accept(MapItemExpr &expr)
{}
void ASTXml::accept(SetExpr &expr)
{}
void ASTXml::handleSelectorExpr(PrimaryExpr &primExpr, vector<SelectorExpr *> &elements)
{}
