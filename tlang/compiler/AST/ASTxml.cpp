//
//  ASTXml.cpp
//  A toyable language compiler (like a simple c++)

#include "AST.h"
#include "Declaration.h"
#include "Expression.h"
#include "Statement.h"
#include "Compile.h"
#include "Location.h"
#include "Scope.h"
#include "ASTxml.h"

ASTXml::ASTXml(const string& path, const string& file)
{
    m_file = file;
    m_path = path;
    // create the xml root node
    CompileOption& option = CompileOption::getInstance();
    if (option.isOutputAST() && m_xmlDoc) {
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
    CompileOption& option = CompileOption::getInstance();
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

void ASTXml::walk(AST* ast)
{
    if (ast)
        ast->walk(this);
}

void ASTXml::build(AST* ast)
{
    if (!ast)
        return;
    // check wether the option is specified
    CompileOption& option = CompileOption::getInstance();
    if (!option.isOutputAST())
        return;
    
    // push the root xml node ptr into stack
    pushXmlNode(m_rootXmlNode);
    // walk through the ast tre
    ast->walk(this); 
    
    popXmlNode();
    
    // save the xml file
    string fullFileName = m_path; 
    fullFileName += "/"; 
    fullFileName += m_file;
    fullFileName += ".ast";
    fullFileName += ".xml";
    xmlSaveFormatFileEnc(fullFileName.c_str(), m_xmlDoc, "UTF-8", 1);
} 

// class
void ASTXml::accept(Class& cls)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Class");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST cls.m_name.c_str());
    val = (cls.m_isPublic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    val = (cls.m_isAbstract)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "abstract", BAD_CAST val.c_str());
    val = (cls.m_isFinal)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "final", BAD_CAST val.c_str()); 
    
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    
    vector<string>::iterator ite = cls.m_base.begin();
    for (; ite != cls.m_base.end(); ite++)
        xmlNewProp(xmlNode, BAD_CAST "base_class", BAD_CAST (*ite).c_str());
    for (ite = cls.m_abstractCls.begin(); ite != cls.m_abstractCls.end(); ite++)
        xmlNewProp(xmlNode, BAD_CAST "abstract_class", BAD_CAST (*ite).c_str());
    walk(cls.m_block);
    
    popXmlNode();
}
void ASTXml::accept(ClassBlock& block)
{
    // output variable
    vector<Variable* >::iterator v = block.m_vars.begin();
    for (; v != block.m_vars.end(); v++) 
        walk(*v);

    // output method
    vector<Method* >::iterator m = block.m_methods.begin();
    for (; m != block.m_methods.end(); m++)
        walk(*m);
}

// type
void ASTXml::accept(TypeDecl& type)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Type");
    switch (type.m_typeid) {
        case TypeDecl::voidType:
            val = "void";
            break;
        case TypeDecl::intType:
            val = "int";
            break;
        case TypeDecl::boolType:
            val = "bool";
            break;
        case TypeDecl::stringType:
            val = "string";
            break;
        case TypeDecl::floatType:
            val = "float";
            break;
        case TypeDecl::idType:
            val = "id";
            break;
        case TypeDecl::mapType:
            val = "map";
            break;
        case TypeDecl::setType:
            val = "set";
            break;
        case TypeDecl::customType:
            val = type.m_t1;
            break;
        default:
            dbg("ASTXml::the type id is not right\n");
            return; 
            break;
    }
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST val.c_str());
    if (type.m_typeid == TypeDecl::mapType) {
        xmlNodePtr sxmlNode = xmlNewNode(NULL, BAD_CAST "subType");
        xmlNewProp(sxmlNode, BAD_CAST "type1", BAD_CAST type.m_t1.c_str());
        xmlNewProp(sxmlNode, BAD_CAST "type2", BAD_CAST type.m_t2.c_str());
        xmlAddChild(xmlNode, sxmlNode);
    }
    else if (type.m_typeid == TypeDecl::setType) {
        xmlNodePtr sxmlNode = xmlNewNode(NULL, BAD_CAST "subType");
        xmlNewProp(sxmlNode, BAD_CAST "type1", BAD_CAST type.m_t2.c_str());
        xmlAddChild(xmlNode, sxmlNode);
    }
    xmlAddChild(m_curXmlNode, xmlNode);
}
// 
// variable 
void ASTXml::accept(Variable& var)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Variable");
    // set attribute
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST var.m_name.c_str());
    if (!var.m_TypeDecl)
        val = "unknow";
    else
        val = var.m_TypeDecl->m_name;
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
void ASTXml::accept(Method& method)
{
    string val;
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Method");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST method.m_name.c_str());
    val = (method.m_isPublic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "publicity", BAD_CAST val.c_str());
    xmlNewProp(xmlNode, BAD_CAST "class", BAD_CAST method.m_class.c_str());
    val = (method.m_isVirtual)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "virtual", BAD_CAST val.c_str());
   
    val = (method.m_isStatic)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "static", BAD_CAST val.c_str());
   
    val = (method.m_isConst)?"true":"false";
    xmlNewProp(xmlNode, BAD_CAST "const", BAD_CAST val.c_str());

    if (method.m_retTypeDecl)
        val = method.m_retTypeDecl->m_name;
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
void ASTXml::accept(FormalParameterList& list)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "FormalParameterList");
    xmlAddChild(m_curXmlNode, xmlNode); 
    pushXmlNode(xmlNode);
    vector<FormalParameter* >::iterator ite = list.m_parameters.begin();
    for (; ite != list.m_parameters.end(); ite++) 
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(FormalParameter& para)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "FormalParameter");
    xmlAddChild(m_curXmlNode, xmlNode);
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST para.m_name.c_str());
    val = "unknow";
    if (para.m_TypeDecl) val = para.m_TypeDecl->m_name;
    xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
    if (para.m_hasDefault) {
        val = "true"; 
        xmlNewProp(xmlNode, BAD_CAST "default", BAD_CAST val.c_str());
    }
}
void ASTXml::accept(MethodBlock& block)
{
    vector<Variable* >::iterator v = block.m_vars.begin();
    for (; v != block.m_vars.end(); v++)
        walk(*v);

    vector<Statement* >::iterator s = block.m_stmts.begin();
    for (; s != block.m_stmts.end(); s++)
        walk(*s);
}


// statement
void ASTXml::accept(Statement& stmt)
{}
void ASTXml::accept(IncludeStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "IncludeStatement");
    xmlAddChild(m_curXmlNode, xmlNode);
    xmlNewProp(xmlNode, BAD_CAST "file", BAD_CAST stmt.m_fullName.c_str());
}
void ASTXml::accept(BlockStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "BlockStatement");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    
    vector<Variable* >::iterator v = stmt.m_vars.begin();
    for (; v != stmt.m_vars.end(); v++)
        walk(*v);

    vector<Statement* >::iterator s = stmt.m_stmts.begin();
    for (; s != stmt.m_stmts.end(); s++)
        walk(*s);

    popXmlNode();
}
void ASTXml::accept(LocalVariableDeclarationStatement& stmt)
{
    string val;

    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "VariableDeclStatement");
    walk(stmt.m_var);
    if (stmt.m_expr)
        val = "true";
    xmlNewProp(xmlNode, BAD_CAST "initialized", BAD_CAST val.c_str());
    walk(stmt.m_expr);
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(IfStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "IfStatement");
    walk(stmt.m_conditExpr);
    walk(stmt.m_ifBlockStmt);
    walk(stmt.m_elseBlockStmt);
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(WhileStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "WhileStatement");
    walk(stmt.m_conditExpr);
    walk(stmt.m_stmt);
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(DoStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "DoStatement");
    walk(stmt.m_conditExpr);
    walk(stmt.m_stmt);
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(ForStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ForStatement");
    walk(stmt.m_expr1);
    walk(stmt.m_expr2);
    walk(stmt.m_exprList);
    walk(stmt.m_stmt);
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(ForeachStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ForeachStatement");
    char buf[255]; 
    for (int index = 0; index < stmt.m_varNumbers; index++) {
        if (stmt.m_TypeDecl[index]) {
            sprintf(buf, "type%d", index);
            xmlNewProp(xmlNode, BAD_CAST buf, BAD_CAST stmt.m_TypeDecl[index]->m_name.c_str()); 
            sprintf(buf, "var%d", index);
            xmlNewProp(xmlNode, BAD_CAST buf, BAD_CAST stmt.m_id[index].c_str());
        }
    }
    walk(stmt.m_objectTypeExpr);
    walk(stmt.m_stmt);
    
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(SwitchStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "SwitchStatement");
    vector<pair<vector<Expr* >, Statement* > >::iterator ite = stmt.m_cases.begin();
    for (size_t  index = 0; index < stmt.m_cases.size(); index++) {
        char buf[10]; 
        sprintf(buf, "case%d", (int)index); 
        xmlNodePtr nxmlNode = xmlNewNode(NULL, BAD_CAST buf); 
        xmlAddChild(m_curXmlNode, nxmlNode);
        pushXmlNode(nxmlNode);
        pair<vector<Expr* >, Statement*>& item = stmt.m_cases[index]; 
        vector<Expr* >& exprs = item.first;
        Statement* cstmt = item.second;
        vector<Expr* >::iterator i = exprs.begin();
        for (; i != exprs.end(); i++)
            walk(*i);
        walk(cstmt);
        popXmlNode(); 
    }
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(ContinueStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ContinueStatement");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(BreakStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "BreakStatement");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(ReturnStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ReturnStatement");
    walk(stmt.m_resultExpr); 
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(ThrowStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ThrowStatement");
    walk(stmt.m_resultExpr); 
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(AssertStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "AssertStatement");
    walk(stmt.m_resultExpr); 
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(TryStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "TryStatement");
    walk(stmt.m_blockStmt);
    vector<CatchStatement* >::iterator ite;
    for (ite = stmt.m_catchStmts.begin(); ite != stmt.m_catchStmts.end(); ite++)
        walk(*ite);
    walk(stmt.m_finallyStmt);
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(CatchStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "CatchStatement");
    xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST stmt.m_type.c_str());
    xmlNewProp(xmlNode, BAD_CAST "id", BAD_CAST stmt.m_id.c_str());
    walk(stmt.m_block); 
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(FinallyCatchStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "FinallyCatchStatement");
    walk(stmt.m_block); 
    xmlAddChild(m_curXmlNode, xmlNode);
}

void ASTXml::accept(ExprStatement& stmt)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ExprStatement");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode); 
    walk(stmt.m_target);
    vector<pair<string, Expr* > >::iterator ite;
    for (ite = stmt.m_elements.begin(); ite != stmt.m_elements.end(); ite++) { 
        pair<string, Expr* >& item =* ite;
        xmlNewProp(xmlNode, BAD_CAST "operator", BAD_CAST item.first.c_str());
        walk(item.second);
    }
    popXmlNode();
}
// expression
void ASTXml::accept(Expr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "Expr");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(ExprList& list)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ExprList");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode); 
    vector<Expr* >::iterator ite = list.m_exprs.begin();
    for (; ite != list.m_exprs.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(BinaryOpExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "BinaryOpExpr");
    xmlNewProp(xmlNode, BAD_CAST "operator", BAD_CAST expr.m_opname.c_str()); 
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_left);
    walk(expr.m_right);
    popXmlNode();
}
void ASTXml::accept(ConditionalExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ContionalExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(LogicOrExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "LogicOrExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(LogicAndExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "LogicAnd");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(BitwiseOrExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "BitwiseorExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(BitwiseXorExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "BitwiseXorExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(BitwiseAndExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "BitwiseAndExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(EqualityExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "EqualityExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(RelationalExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "RelationalExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(ShiftExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "ShiftExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(AdditiveExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "AdditiveExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(MultiplicativeExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "multiplicativeExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_target);
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++)
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(UnaryExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "UnaryExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_primary);
    vector<SelectorExpr* >::iterator ite = expr.m_selectors.begin();
    for (; ite != expr.m_selectors.end(); ite++) 
        walk(*ite);
    popXmlNode();
}
void ASTXml::accept(PrimaryExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "PrimaryExpr");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST expr.m_text.c_str()); 
    xmlAddChild(m_curXmlNode, xmlNode);
    pushXmlNode(xmlNode);
    walk(expr.m_expr);
    popXmlNode();
}
void ASTXml::accept(SelectorExpr& expr)
{
    string val; 
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "SelectorExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
    if (expr.m_type == SelectorExpr::DOT_SELECTOR) {
        val = "dotSelector";
        xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
        xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST expr.m_id.c_str());
    }
    else if (expr.m_type == SelectorExpr::ARRAY_SELECTOR) {
        val = "arraySelector";
        xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
        pushXmlNode(xmlNode);
        walk(expr.m_arrayExpr);
        popXmlNode();
    }
    else {
        val = "methodSelctor";
        xmlNewProp(xmlNode, BAD_CAST "type", BAD_CAST val.c_str());
        pushXmlNode(xmlNode);
        walk(expr.m_methodCallExpr);
        popXmlNode();
    }
}
void ASTXml::accept(MethodCallExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "MethodCallExpr");
    xmlNewProp(xmlNode, BAD_CAST "name", BAD_CAST expr.m_methodName.c_str());
    for (size_t index  = 0; index < expr.m_arguments.size(); index++) 
        walk(expr.m_arguments[index]); 
    xmlAddChild(m_curXmlNode, xmlNode);
}

// new
void ASTXml::accept(NewExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "NewExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
}

// map &  list
void ASTXml::accept(MapExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "MapExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(MapItemExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "MapItemExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
}
void ASTXml::accept(SetExpr& expr)
{
    xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "SetExpr");
    xmlAddChild(m_curXmlNode, xmlNode);
}
