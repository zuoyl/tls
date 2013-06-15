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
    // output variable
    vector<Variable *>::iterator v = block.m_vars.begin();
    for (; v != block.m_vars.end(); v++) 
        walk(*v);

    // output method
    vector<Method *>::iterator m = block.m_methods.begin();
    for (; m != block.m_methods.end(); m++)
        walk(*m);
}

// protocol
void ASTXml::accept(Protocol &protocol)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Protocol");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST protocol.m_name.c_str());
    val = (protocol.m_isPublic == true)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    xmlAddChild(m_curXmlNode, xmlNode); 
    pushXmlNode(xmlNode);

    vector<Method *>::iterator ite = protocol.m_methods.begin();
    for (; ite != protocol.m_methods.end(); ite++) 
        walk(*ite);
    popXmlNode();

}
// type
void ASTXml::accept(TypeSpec &type)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Type");
    switch (type.m_typeid) {
        case TypeSpec::voidType:
            val = "void";
            break;
        case TypeSpec::intType:
            val = "int";
            break;
        case TypeSpec::boolType:
            val = "bool";
            break;
        case TypeSpec::stringType:
            val = "string";
            break;
        case TypeSpec::floatType:
            val = "float";
            break;
        case TypeSpec::idType:
            val = "id";
            break;
        case TypeSpec::mapType:
            val = "map";
            break;
        case TypeSpec::setType:
            val = "set";
            break;
        case TypeSpec::customType:
            val = type.m_t1;
            break;
        default:
            dbg("ASTXml::the type id is not right\n");
            return; 
            break;
    }
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST val.c_str());
    if (type.m_typeid == TypeSpec::mapType) {
        xmlNodePtr sxmlNode = xmlNewNode(NULL, BAD_CAST "subType");
        xmlNewProp(sxmlNode, BAD_CAST "type1", BAD_CAST type.m_t1.c_str());
        xmlNewProp(sxmlNode, BAD_CAST "type2", BAD_CAST type.m_t2.c_str());
        xmlAddChild(xmlNode, sxmlNode);
    }
    else if (type.m_typeid == TypeSpec::setType) {
        xmlNodePtr sxmlNode = xmlNewNode(NULL, BAD_CAST "subType");
        xmlNewProp(sxmlNode, BAD_CAST "type1", BAD_CAST type.m_t2.c_str());
        xmlAddChild(xmlNode, sxmlNode);
    }
    xmlAddChild(m_curXmlNode, xmlNode);
}
// 
// variable 
void ASTXml::accept(Variable &var)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Variable");
    // set attribute
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST var.m_name.c_str());
    if (!var.m_typeSpec)
        val = "unknow";
    else
        val = var.m_typeSpec->m_name;
    xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
    val = (var.m_isPublic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    val = (var.m_isStatic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "static", BAD_CAST val.c_str());
    val = (var.m_isConst)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "const", BAD_CAST val.c_str());
    val = (var.m_class.empty())?"unknow":var.m_class;
    xmlNewProp(xmlNode, BAD_CAST "class", BAD_CAST val.c_str());
    val = (var.m_isInitialized)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "initialize", BAD_CAST val.c_str());

    xmlAddChild(m_curXmlNode, xmlNode);
}

// method
void ASTXml::accept(Method &method)
{
    string val;
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Method");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST method.m_name.c_str());
    val = (method.m_isPublic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    if (method.m_isOfClass)
        xmlNewProp(xmlNode, BAD_CAST "class", BAD_CAST method.m_class.c_str());
    else 
        xmlNewProp(xmlNode, BAD_CAST "protocol", BAD_CAST method.m_protocol.c_str());
    val = (method.m_isVirtual)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "virtual", BAD_CAST val.c_str());
   
    val = (method.m_isStatic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "static", BAD_CAST val.c_str());
   
    val = (method.m_isConst)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "const", BAD_CAST val.c_str());

    if (method.m_retTypeSpec)
        val = method.m_retTypeSpec->m_name;
    else
        val = "unknow";
    xmlNewProp(xmlNode, BAD_CAST "returnType", BAD_CAST val.c_str());


    // walk through the child node
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);

    walk(method.m_paraList);
    walk(method.m_block);
    popXmlNode();
}
void ASTXml::accept(MethodParameterList &list)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "MethodParameterList");
    xmlAddChild(m_curXmlNode, xmlNode); 
    pushXmlNode(xmlNode);
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
    val = "unknow";
    if (para.m_typeSpec) val = para.m_typeSpec->m_name;
    xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
    if (para.m_hasDefault) {
        val = "true"; 
        xmlNewProp(xmlNode, BAD_CAST "default", BAD_CAST val.c_str());
    }
}
void ASTXml::accept(MethodBlock &block)
{
    vector<Variable *>::iterator v = block.m_vars.begin();
    for (; v != block.m_vars.end(); v++)
        walk(*v);

    vector<Statement *>::iterator s = block.m_stmts.begin();
    for (; s != block.m_stmts.end(); s++)
        walk(*s);
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
