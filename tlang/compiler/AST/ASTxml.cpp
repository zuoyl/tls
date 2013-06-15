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
        m_rootXmlNode = xmlNewNode(NULL, BAD_CAST "AbstractSyntaxTree");
        m_curXmlNode = m_rootXmlNode;
        xmlDocSetRootElement(m_xmlDoc, m_rootXmlNode); 
    }
    else {
        m_xmlDoc = NULL;
        m_rootXmlNode = NULL;
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
void ASTXml::pushXmlNode(xmlNodePtr node)
{
    m_xmlNodes.push(node);
    m_curXmlNode = node;
}

void ASTXml::popXmlNode()
{
    if (!m_xmlNodes.empty()) {
        m_xmlNodes.pop();
        if (!m_xmlNodes.empty())
            m_curXmlNode = m_xmlNodes.top();
        else
            m_curXmlNode = m_rootXmlNode;
    }
}

void ASTXml::walk(AST *ast)
{
    if (ast)
        ast->walk(this);
}

void ASTXml::build(AST* ast)
{
    if (!ast)
        return;
    // check wether the option is specified
    CompileOption &option = CompileOption::getInstance();
    if (!option.isOutputAST())
        return;
    
    // push the root xml node ptr into stack
    pushXmlNode(m_rootXmlNode);
    // walk through the ast tre
    ast->walk(this); 
    
    popXmlNode();
    
    // save the xml file
    string fullFileName = m_path; 
    unsigned found = m_file.find_last_of(".");
    fullFileName += "/"; 
    fullFileName += m_file.substr(0, found);
    fullFileName += "_ast";
    fullFileName += ".xml";
    xmlSaveFormatFileEnc(fullFileName.c_str(), m_xmlDoc, "UTF-8", 1);
} 

// class
void ASTXml::accep(Class &cls)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Class");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST cls.m_name.c_str());
    val = (cls.m_isPublic == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    val = (cls.m_isAbstract == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "abstract", BAD_CAST val.c_str());
    val = (cls.m_isFrozen == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "final", BAD_CAST val.c_str()); 
    
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    
    vector<string>::iterator ite = cls.m_base.begin();
    for (; ite != cls.m_base.end(); ite++)
        xmlNewProp(xmlNode, BAD_CAST "base_class", BAD_CAST (*ite).c_str());
    for (ite = cls.m_protocols.begin(); ite != cls.m_protocols.end(); ite++)
        xmlNewProp(xmlNode, BAD_CAST "protocol", BAD_CAST (*ite).c_str());
    walk(cls.m_block);
    
    popXmlNode();
}
void ASTXml::accept(ClassBlock &block)
{
    

}

// protocol
void ASTXml::accept(Protocol &protocol)
{
    


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
    string val;
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Method");
    xmlAddChild(m_curXmlNode, xmlNode);
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST method.m_name.c_str());
    xmlNewProp(xmlNode, BAD_CAST "class", BAD_CAST method.m_class.c_str());
   
    val = (method.m_isPublic == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    
    val = (method.m_isStatic == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "static", BAD_CAST val.c_str());
    
    // walk through the child node
    pushXmlNode(xmlNode);

    walk(method.m_paraList);
    walk(method.m_block);
    popXmlNode();
}
void ASTXml::accept(MethodParameterList &list)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "MethodParameterList");
    pushXmlNode(xmlNode);
    xmlAddChild(m_curXmlNode, xmlNode); 
    vector<MethodParameter *>::iterator ite = list.m_parameters.begin();
    for (; ite != list.m_parameters.end(); ite++) 
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(MethodParameter &para)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "MethodParameter");
    xmlAddChild(m_curXmlNode, xmlNode);
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST para.m_name.c_str());
    val = (para.m_isConst == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "static", BAD_CAST val.c_str());
    val = "null";
    if (para.m_typeSpec) val = para.m_typeSpec->m_name;
    xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
}
void ASTXml::accept(MethodBlock &block)
{
    vector<Variable *>::iterator itv = block.m_vars.begin();
    for (; itv != block.m_vars.end(); itv++)
        walk(*itv);

    vector<Statement *>::iterator its = block.m_stmts.begin();
    for (; its != block.m_stmts.end(); its++)
        walk(*its);
}


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
