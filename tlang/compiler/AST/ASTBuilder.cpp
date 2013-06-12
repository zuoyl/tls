//
//  ASTBuilder.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Common.h"
#include "ASTBuilder.h"
#include "AST.h"
#include "Parser.h"
#include "Type.h"
#include "Struct.h"
#include "Variable.h"
#include "Class.h"
#include "Statement.h"
#include "Method.h"
#include "Expression.h"
#include "Exception.h"

ASTBuilder::ASTBuilder(const string &path, const string &file) 
{
}

ASTBuilder::~ASTBuilder() 
{
    
}

/// @brief ASTBuilder main method to convert a parse tree into an AST tree
AST* ASTBuilder::build(Node *parseTree) 
{
    if (!parseTree) {
        Error::complain("the parse tree is null, just return\n");
        return NULL;
    }
    
    AST * root = new AST();
    // the root node of pareTree must be compile unit
    vector<Node *>::iterator ite = parseTree->childs.begin();
    for (; ite != parseTree->childs.end(); ite++) {
        Node * decls = *ite;
        AST *child = handleDecl(decls->childs[0]);
        if (child)
            root->addChildNode(child);
    }
    return root;
}

AST* ASTBuilder::handleDecl(Node *node)
{
    if (node->assic == "includeDeclaration")
        return  handleIncludeDecl(node);
    else if (node->assic == "classDeclaration")
        return  handleClassDecl(node);
    else if (node->assic == "protocolDeclaration")
        return  handleProtocolDecl(node);
    else  
        Error::complain(node->location, "the Parse Tree is not right\n");
    return NULL; 
}

/// @brief ASTBuilder handler for include Decl
AST* ASTBuilder::handleIncludeDecl(Node *node) 
{
    string fullName = node->childs[1]->assic; 
    // make new ast
    return new IncludeStatement(fullName, node->location);
}

/// @brief Handler fro class 
/// classDecl
/// : scopeSpecifier ? 'class' identifer (classInheritDecl)?(protoclImplementataion)? classBlock
/// ;
AST* ASTBuilder::handleClassDecl(Node *node) 
{
    int index = 0;
    bool isPublic = false;
    bool isFrozen = false;
    bool isAbstract = false;    
    // get scope specifier
    if (node->childs[index]->assic == "ScopeSpecifier") {
        if (node->childs[index]->childs[0]->assic == "public")
            isPublic = true;
        index++;
    }
    if (node->childs[index]->assic == "classSingature") {
        if (node->childs[index]->childs[0]->assic == "freezen")
            isFrozen = true;
        else if (node->childs[index]->childs[0]->assic == "abstract")
            isAbstract = true;
        index++;
    }
    
    // get id, skip 'class'
    index++;
    string id = node->childs[index++]->childs[0]->assic;
    vector<string> baseList;
    vector<string> protocolList;
    
    // loop to check the basis class and protocol
    while (index < (node->count() - 1)) {
        // check the base class and protocol
        // 'extend class1, class2,...
        if (node->childs[index]->childs[0]->assic == "ClassInheritDecl") {
            Node *subroot = node->childs[index];
            // get base class
            // 'extend' identifer (',' identifer)*
            for (int childIndex = 1; childIndex < subroot->count(); childIndex++) {
                baseList.push_back(subroot->childs[childIndex]->childs[0]->assic);
                childIndex += 2;
            }
        }
        // 'implement protocol1, protocol2,...
        else if (node->childs[index]->childs[0]->assic == "ProtocolImplementation") {
            Node *subroot = node->childs[index];
            // 'implement' identifier (',' identifier)*
            for (int childIndex = 1; childIndex < subroot->count(); childIndex++) {
                protocolList.push_back(subroot->childs[childIndex]->childs[0]->assic);
                childIndex += 2;
            }
        }
    }
    
    // get class block
    Node *blockNode = node->childs[node->count() -1];
    ClassBlock *clsBlock = (ClassBlock*)handleClassBlock(blockNode);

    return new Class(isPublic, isFrozen, isAbstract, id, baseList, protocolList, clsBlock, node->location);
    
}

/// @brief ASTBuilder handler for class block
AST* ASTBuilder::handleClassBlock(Node *node) 
{
    ClassBlock *block = new ClassBlock(node->location);
    
    if (node->count() == 2)
        return block;
    
    for (int index = 1; index < node->count() - 1; index++) {
        if (node->childs[index]->childs[0]->assic == "classVarDecl") {
            Variable *var = (Variable *)handleClassVariable(node->childs[index]);
            block->addVariable(var);
        }
        else if (node->childs[index]->childs[0]->assic == "classMethodDecl") {
            Method *method = (Method*)handleMethodDecl(node->childs[index]);
            block->addMethod(method);
        }
        else {
            throw Exception::InvalidSyntax(node->assic);
            break;
        }
    }
    return block;
}

/// @brief ASTBuilder handler for class variable
AST* ASTBuilder::handleClassVariable(Node *node) 
{
    bool isPublic = false;
    int index = 0;
    
    // check the scope
    if (node->childs[index]->assic == "scopeSpecifier") {
        if (node->childs[index]->childs[0]->assic == "public")
            isPublic = true;
        index++;
    }
    
    Variable *var =  (Variable*)handleVarDecl(node->childs[index]);
    var->m_isPublic = isPublic;
    var->m_isOfClass = true;
    return var;
}

/// @brief ASTBuilder handler for class method
AST* ASTBuilder::handleMethodDecl(Node *node) 
{

    // return type and method name
    TypeSpec *retTypeSpec = 
        (TypeSpec *)handleTypeDecl(node->childs[0]);
    string methodName = node->childs[1]->assic;
    // method parameter list
    MethodParameterList *methodParameterList = 
        (MethodParameterList *)handleMethodParameters(node->childs[2]);    
    // make AST tree
    Method *method =  new Method(retTypeSpec, methodName, methodParameterList, node->location);
    return method;
}

/// @brief ASTBuilder handler for interface Decl
/// @brief ASTBuilder handler for typeSpecifier
AST* ASTBuilder::handleTypeDecl(Node *node) 
{
    TypeSpec *typeSpec = new TypeSpec(node->location);

    if (node->childs[0]->assic == "basicType") {
        typeSpec->m_name = node->childs[0]->childs[0]->assic;
        if (typeSpec->m_name == "bool")
            typeSpec->m_typeid = TypeSpec::boolType;
        else if (typeSpec->m_name == "int")
            typeSpec->m_typeid = TypeSpec::intType;
        else if (typeSpec->m_name == "string")
            typeSpec->m_typeid = TypeSpec::stringType;
        else if (typeSpec->m_name == "float")
            typeSpec->m_typeid = TypeSpec::floatType;
    }
    else if (node->childs[0]->assic == "identifer") {
        typeSpec->m_name = node->childs[0]->childs[0]->assic;
        typeSpec->m_typeid = TypeSpec::customType;
    }
    else if (node->childs[0]->assic == "mapType") {
        typeSpec->m_name = "map";
        typeSpec->m_typeid = TypeSpec::mapType;
        typeSpec->m_t1 = node->childs[0]->childs[2]->assic;
        typeSpec->m_t2 = node->childs[0]->childs[3]->assic;
    }
    else if (node->childs[0]->assic == "setType") {
        typeSpec->m_name = "set";
        typeSpec->m_typeid = TypeSpec::setType;
        typeSpec->m_t1 = node->childs[0]->childs[2]->assic;        
    }
    else {
        delete typeSpec;
        typeSpec = NULL;
        Error::complain(node->location, "the type  %s is not declared\n", 
                node->childs[0]->assic.c_str());
    }
    return typeSpec;
}

// varDecl
// : storageSpecifier? constSpecifier? typeSpecifier identifier  ('=' expression)? ';'
//;
/// @breif ASTBuilder handler for variable Decl
AST* ASTBuilder::handleVarDecl(Node *node) 
{
    bool isStatic = false;
    bool isConst = false;
    int index = 0;
    int childCount = (int)node->childs.size();
    Expr *expr = NULL;
    
    // check wether the variable is static
    if (node->childs[index]->assic == "storageSpecifier") {
        if (node->childs[index]->childs[0]->assic == "static") 
            isStatic = true;
        index++;
    }
    
    // check wether the variable is const
    if (node->childs[index]->assic == "constSpecifier") {
        if (node->childs[index]->childs[0]->assic == "const")
            isConst = true;
        index++;
    }
    
    // typespecifier and variable name
    TypeSpec *typeSpec = 
        (TypeSpec *)handleTypeDecl(node->childs[index++]->childs[0]);
    string id = node->childs[index++]->childs[0]->assic;
    
    // expression initializer
    if ((index + 1) < childCount) {
        // check to see wether have expression initialization list
        expr = (Expr *)handleExpr(node->childs[index]);
    }
    return new Variable(isStatic, isConst, typeSpec, id, expr, node->location);
}


/// @brief Handler for method parameter
AST* ASTBuilder::handleMethodParameters(Node *node) 
{
    int childCount = node->count();
    MethodParameterList *paraList = new MethodParameterList(node->location);
    MethodParameter *parameter = NULL;
    
    if (childCount == 2) // no parameter
        return paraList;
    else if (childCount == 3)  { // with only normal parameter
        parameter = (MethodParameter*)
            handleMethodNormalParameter(node->childs[1]);
        paraList->addParameter(parameter);
    }
    else if (childCount == 5) { // with default parameter
        // handle normal parameter
        for (int index = 0; index < node->childs[1]->count(); index++) {
            parameter = (MethodParameter *)
                handleMethodNormalParameter(node->childs[1]->childs[index]);
            paraList->addParameter(parameter);
            index++; // skop the token ','
        }
        
        // handle default parameter
        for (int index = 0; index < node->childs[4]->count(); index++) {
            parameter = (MethodParameter *)
                handleMethodDefaultParameter(node->childs[4]->childs[index]);
            paraList->addParameter(parameter);
            index++;
        }
    }
    
    return paraList;
}

/// @brief Handler for method normal parameter
AST* ASTBuilder::handleMethodNormalParameter(Node *node) 
{
    bool isConst = false;
    int index = 0;
    
    // get const specifier
    if (node->count() == 3) {
        isConst = true;
        index = 1;
    }
    
    // get type name and id
    TypeSpec *typeSpec = 
        (TypeSpec *)handleTypeDecl(node->childs[index]);
    string id = node->childs[index + 1]->assic;
    return new MethodParameter(isConst, typeSpec, id, false, NULL, node->location);
    
}

/// @brief Handler for method default parameter 
AST* ASTBuilder::handleMethodDefaultParameter(Node *node) 
{
    MethodParameter * para = (MethodParameter *) handleMethodNormalParameter(node->childs[0]);
    
    if (node->count() == 3)
        para->m_default = (Expr *)handleExpr(node->childs[2]);
    
    return para;
    
}

/// @brief Handler for method block
AST* ASTBuilder::handleMethodBlock(Node *node) 
{
    MethodBlock *block = new MethodBlock(node->location);
    
    for (int index = 1; index < node->count() - 1; index++) {
        Statement *stmt = (Statement *)handleStatement(node->childs[index]);
        stmt->setParentNode(block);
        VariableDeclStatement *varDeclStmt =
                        dynamic_cast<VariableDeclStatement*>(stmt);
        if (varDeclStmt)
            block->m_vars.push_back(varDeclStmt->m_var);
        
        block->addStatement(stmt);
    }
    return block;
}

/// @brief ASTBuilder handler for interface Decl
AST* ASTBuilder::handleProtocolDecl(Node *node) 
{
    bool isPublic = false;
    int index = 0;
    
    if (node->count() == 4) {
        TypeSpec *typeSpec = 
            (TypeSpec *)handleTypeDecl(node->childs[0]);
        index++;
    }
    // skip the protocol keyword 
    index++; 
    string id = node->childs[index++]->assic;
    Node *blockNode = node->childs[index];
    Protocol *protocol = new Protocol(id, node->location);
   
    for (int i = 1; i < blockNode->count() - 1; i ++) {
        bool isConst = false;
        int methodIndex = 0; 
        if (blockNode->childs[i]->count() == 2) {
            isConst = true;
            methodIndex = 1; 
        } 
        Method *method = 
            (Method*)handleMethodDecl(blockNode->childs[i]->childs[methodIndex]);
        method->m_isConst = isConst;
        method->m_isOfProtocol = true;
        method->m_protocol = id;
        protocol->addMethod(method); 
    }
    return protocol;
}

//
// Statements
//
/// @brief ASTBuilder handler for statement
AST* ASTBuilder::handleStatement(Node *node) 
{
    string type = node->childs[0]->assic;
    
    if (type == "blockStatement")
        return handleBlockStatement(node->childs[0]);
    else if (type == "valDeclStatement")
        return handleVarDeclStatement(node->childs[0]);
    else if (type == "ifStatement")
        return handleIfStatement(node->childs[0]);
    else if (type == "forStatement")
        return handleForStatement(node->childs[0]);
    else if (type == "doStatement")
        return handleDoStatement(node->childs[0]);
    else if (type == "whileStatement")
        return handleWhiletatement(node->childs[0]);
    else if (type == "returnStatement")
        return handleReturnStatement(node->childs[0]);
    else if (type == "tryStatement")
        return handleTryStatement(node->childs[0]);
    else if (type == "throwStatement")
        return handleTryStatement(node->childs[0]);
    else if (type == "breakStatement")
        return handleBreakStatement(node->childs[0]);
    else if (type == "assertStatement")
        return handleAssertStatement(node->childs[0]);
    else if (type == "expressionStatement")
        return handleExpreStatement(node->childs[0]);
    else if (type == "methodCallStatement")
        return handleMethodCallStatement(node->childs[0]);
    else {
        throw Exception::InvalidStatement(node->assic);
        return NULL;
    }
}

/// @brief ASTBuilder handler for block statment
AST* ASTBuilder::handleBlockStatement(Node *node) 
{
    BlockStatement *blockStmt = new BlockStatement(node->location);
    for (int index = 1; index < node->count() - 1; index++) {
        Statement * stmt = (Statement *)handleStatement(node->childs[index]);
        stmt->setParentNode(blockStmt);
        VariableDeclStatement *varDeclStmt = 
                    dynamic_cast<VariableDeclStatement*>(stmt); 
        if (varDeclStmt)
            blockStmt->m_vars.push_back(varDeclStmt->m_var);     
            
        blockStmt->addStatement(stmt);
    }
    return blockStmt;
}

/// @brief ASTBuilder handler for variable Decl statement
AST* ASTBuilder::handleVarDeclStatement(Node *node) 
{
    Variable *var = (Variable*)handleVarDecl(node->childs[0]);
    var->m_isGlobal = false;
    var->m_isOfClass = false;
    var->m_isPublic = false;
   
    return new VariableDeclStatement(var, var->m_expr, node->location);
}

/// @brief ASTBuidler handler for if statement
AST* ASTBuilder::handleIfStatement(Node *node) 
{
    Expr *conditExpr = (Expr *)handleExpr(node->childs[2]);
    Statement *stmt1 = (Statement*)handleStatement(node->childs[4]);
    Statement *stmt2 = NULL;
    if (node->count() == 7) {
        stmt2 = (Statement*)handleStatement(node->childs[6]);
    }
    return new IfStatement(conditExpr, stmt1, stmt2, node->location);
}

/// @brief ASTBuilder handler for for statement
AST* ASTBuilder::handleForStatement(Node *node) 
{
    int index = 2;
    int sindex = 0;
    Expr *exprs[2] = { NULL, NULL};
    ExprList *exprList = NULL;

    // 'for' '('expression? ';' expression? ';' expressionList? ')' statement
    while (index < (node->count() - 1)) {
        if (node->childs[index]->assic == "expression") {
            if (sindex > 2) break;
            exprs[sindex++] = (Expr *)handleExpr(node->childs[index]);
        }
        else if (node->childs[index]->assic == "expressionList") {
            exprList= (ExprList *)handleExprList(node->childs[index]);
            break;
        }
        index++;
    }
    Statement *stmt = (Statement *)handleStatement(node->childs[node->count() -1]);
    return new ForStatement(exprs[0], exprs[1], exprList, stmt, node->location);
}

/// @brief ASTBuilder handler for foreach statement
// 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' (identifier|mapLiteral|setLitieral) ')' blockStatement
AST* ASTBuilder::handleForEachStatement(Node *node) 
{
    int index = 2;
    ForEachStatement *stmt = new ForEachStatement(node->location);
    
    // check the foreachVarItem
    for (int idx = 0; idx < 2; idx++) {
        if (node->childs[index + idx]->assic == "foreachVarItem") {
            stmt->m_varNumbers++;
            Node *snode = node->childs[index + idx];
            if (snode->count() == 2) {
                stmt->m_typeSpec[idx] = (TypeSpec *)handleTypeDecl(snode->childs[0]);
                stmt->m_id[0] = snode->childs[1]->assic;
            }
            else
                stmt->m_id[0] = snode->childs[0]->assic;
            index++;
        }
    }
    index ++; // skip the 'in' keyword
    
    if (node->childs[index]->assic == "identifer") {
        stmt->m_objectSetName = node->childs[index]->childs[0]->assic; 
        stmt->m_objectSetType = ForEachStatement::Object;
    }
    else if (node->childs[index]->assic == "mapListeral") {
        stmt->m_objectSetType = ForEachStatement::MapObject;
        stmt->m_expr = (Expr *)handleMapLiteral(node->childs[index]);
    }
    else if (node->childs[index]->assic == "setListeral") {
        stmt->m_objectSetType = ForEachStatement::SetObject;
        stmt->m_expr = (Expr *)handleSetLiteral(node->childs[index]);        
    }
    else {
        // error
        delete stmt;
        stmt = NULL;
        Error::complain(node->location, "the set object is not right\n");
    }
    stmt->m_stmt = (Statement *)handleStatement(node->childs[node->count() -1]);  
    return stmt;
}

/// @brief ASTBuilder handler for while statement
AST* ASTBuilder::handleWhiletatement(Node *node) 
{
    Expr *conditExpr = (Expr *)handleExpr(node->childs[2]);
    Statement *stmt = (Statement *)handleStatement(node->childs[4]);
    return new WhileStatement(conditExpr, stmt, node->location);
}

/// @brief ASTBuilder handler for do while statement
AST* ASTBuilder::handleDoStatement(Node *node) 
{
    Expr *conditExpr = (Expr *)handleExpr(node->childs[2]);
    Statement *stmt = (Statement *)handleStatement(node->childs[4]);
    return new DoStatement(conditExpr, stmt, node->location);
}

/// @brief ASTBuilder handler for switch statement
AST* ASTBuilder::handleSwitchStatement(Node *node) 
{
    Expr *resultExpr = (Expr *)handleExpr(node->childs[2]);
    SwitchStatement *switchStmt  = new SwitchStatement(resultExpr, node->location);
    
    for (int index = 5; index < node->count(); index++) {
        if (node->childs[index]->assic == "switchCase") {
            Node *subnode = node->childs[index];
            vector<Expr *> *exprList = new vector<Expr *>();
            for (int subIndex = 0; subIndex < subnode->count() - 1; subIndex += 3) {
                Expr *caseExpr = 
                (Expr *)handleExpr(subnode->childs[subIndex + 1]);
                exprList->push_back(caseExpr);
            }
            Statement *stmt = 
            (Statement *)handleSwitchStatement(subnode->childs[subnode->count() - 1]);
            switchStmt->addCaseStatement(exprList, stmt);
            
        }
        else if (node->childs[index]->assic == "defaultCase") {
            Node *subnode = node->childs[index];
            Statement *stmt = (Statement *)handleSwitchStatement(subnode->childs[2]);
            switchStmt->addDefaultStatement(stmt);
        }
        else {
            delete switchStmt;
            switchStmt = NULL;
            throw Exception::InvalidStatement(node->assic);
            break;
        }
    }
    return switchStmt;
}

/// @brief ASTBuilder hander for return statement
AST* ASTBuilder::handleReturnStatement(Node *node) 
{
    Expr *expr = NULL;
    if (node->count() == 3)
        expr = (Expr *)handleExpr(node->childs[2]);
    return new ReturnStatement(expr, node->location);
}
/// @brief ASTBuilder hander for assert statement
AST* ASTBuilder::handleAssertStatement(Node *node) 
{
    Expr * expr = (Expr *)handleExpr(node->childs[1]);
    return new AssertStatement(expr, node->location);
}

/// @brief ASTBuilder hander for break statement
AST* ASTBuilder::handleBreakStatement(Node *node) 
{
    return new BreakStatement(node->location);
}

/// @brief ASTBuilder hander for continue statement
AST* ASTBuilder::handleContinueStatement(Node *node) 
{
    return new ContinueStatement(node->location);
}
/// @brief ASTBuilder hander for throw statement
AST* ASTBuilder::handleThrowStatement(Node *node) 
{
    Expr *expr = NULL;
    if (node->count() == 3)
        expr = (Expr *)handleExpr(node->childs[1]);
    return new ThrowStatement(expr, node->location);
}

/// @brief ASTBuilder hander for try statement
AST* ASTBuilder::handleTryStatement(Node *node) 
{
    BlockStatement *blockStmt = 
        (BlockStatement *)handleBlockStatement(node->childs[1]);
    TryStatement *tryStmt = new TryStatement(blockStmt, node->location);
    
    for (int index = 2; index < node->count(); index ++) {
        if (node->childs[index]->assic == "catchPart") {
            CatchStatement *catchStmt = 
                        (CatchStatement *)handleCatchStatement(node->childs[index]);
            tryStmt->addCatchPart(catchStmt);
        }
        else if (node->childs[index]->assic == "finallyPart") {
            FinallyCatchStatement *finallyStmt = (FinallyCatchStatement *)
                        handleFinallyCatchStatement(node->childs[index]);
            tryStmt->setFinallyCatchPart(finallyStmt);
        }
        else {
            throw Exception::InvalidStatement(node->assic);
            delete tryStmt;
            tryStmt = NULL;
            break;
        }
    }
    
    return tryStmt;
}

/// @brief ASTBuilder hander for catch statement
AST* ASTBuilder::handleCatchStatement(Node *node) 
{
    string type = node->childs[1]->childs[0]->assic;
    string id = node->childs[2]->childs[0]->assic;
    BlockStatement *blockStmt = 
                (BlockStatement *)handleBlockStatement(node->childs[5]);
    
    return new CatchStatement(type, id, blockStmt, node->location);
}

/// @brief ASTBuilder hander for finally catch statement
AST* ASTBuilder::handleFinallyCatchStatement(Node *node) 
{
    BlockStatement *blockStmt = (BlockStatement *)handleBlockStatement(node->childs[1]);
    return new FinallyCatchStatement(blockStmt, node->location);
}

/// @brief ASTBuilder hander for expression statement
AST* ASTBuilder::handleExpreStatement(Node *node) 
{
    return NULL;
}

/// @brief ASTBuilder hander for funcation statement
AST* ASTBuilder::handleMethodCallStatement(Node *node) 
{
    return NULL;
}

/// @brief ASTBuilder handler for expression list
AST* ASTBuilder::handleExprList(Node *node) 
{
    ExprList *exprList = new ExprList(node->location);
    Expr *expr = NULL;
    
    for (int index = 0; index < node->count(); index++) {
        expr = (Expr *)handleExpr(node->childs[index]);
        exprList->appendExpr(expr);
        index++;
    }
    return exprList;
}

/// @brief ASTBuilder handler for expression
AST* ASTBuilder::handleExpr(Node *node) 
{
    if (node->count() == 1)
        return handleConditionalExpr(node);
    
    Expr *leftExpr = (Expr *)handleAssignableExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    Expr *rightExpr = (Expr *)handleExpr(node->childs[2]);
    
    return new BinaryOpExpr(op, leftExpr, rightExpr, node->location);
}

/// @brief ASTBuilder handler for assignalbe expression
AST* ASTBuilder::handleAssignableExpr(Node *node) 
{
    return NULL;
}

/// @brief ASTBuilder handler for conditional expression
AST* ASTBuilder::handleConditionalExpr(Node *node) 
{
    return handleLogicOrExpr(node);
}

/// @brief ASTBuilder handler for logic or expression
AST* ASTBuilder::handleLogicOrExpr(Node *node) 
{
    if (node->count() == 1)
        return handleLogicAndExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleLogicAndExpr(node->childs[0]);
    LogicOrExpr *logicOrExpre = new LogicOrExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr *target = (Expr *)handleLogicAndExpr(node->childs[index]);
        logicOrExpre->appendElement(target);
    }
    return logicOrExpre;
}

/// @brief ASTBuilder handler for logic and expression
AST* ASTBuilder::handleLogicAndExpr(Node *node) 
{
    if (node->count() == 1)
        return handleBitwiseOrExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleBitwiseOrExpr(node->childs[0]);
    LogicAndExpr *logicAndExpr = new LogicAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr *target = (Expr *)handleBitwiseAndExpr(node->childs[index]);
        logicAndExpr->appendElement(target);
    }
    return logicAndExpr;
}

/// @brief ASTBuilder handler  for bitwise or expression
AST* ASTBuilder::handleBitwiseOrExpr(Node *node) 
{
    if (node->count() == 1)
        return handleBitwiseXorExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleBitwiseXorExpr(node->childs[0]);
    BitwiseOrExpr *expr = new BitwiseOrExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr *target = (Expr *)handleBitwiseXorExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}


/// @brief ASTBuilder handler for bitwise xor expression
AST* ASTBuilder::handleBitwiseXorExpr(Node *node) 
{
    if (node->count() == 1)
        return handleBitwiseAndExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleBitwiseAndExpr(node->childs[0]);
    BitwiseAndExpr *expr = new BitwiseAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr *target = (Expr *)handleBitwiseAndExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// @brief ASTBuilder handler for bitwise and expression
AST* ASTBuilder::handleBitwiseAndExpr(Node *node) 
{
    if (node->count() == 1)
        return handleEqualityExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleEqualityExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    BitwiseAndExpr *expr = new BitwiseAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr *target = (Expr *)handleEqualityExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// @brief ASTBuilder handler for equality expression
AST* ASTBuilder::handleEqualityExpr(Node *node) 
{
    if (node->count() == 1)
        return handleRelationalExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleRelationalExpr(node->childs[0]);
    EqualityExpr *expr = new EqualityExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        Expr *target = (Expr *)handleBitwiseAndExpr(node->childs[index]);
        
        int op = -1;
        if (node->childs[index]->assic == "==")
            op = EqualityExpr::OP_EQ;
        else if (node->childs[index]->assic == "!=")
            op = EqualityExpr::OP_NEQ;
        else 
            throw Exception::InvalidStatement(node->assic);
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for relational expression
AST* ASTBuilder::handleRelationalExpr(Node *node) 
{
    if (node->count() == 1)
        return handleShiftExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleShiftExpr(node->childs[0]);
    RelationalExpr *expr = new RelationalExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expr *target = (Expr *)handleShiftExpr(node->childs[index]);
        
        int op = -1;
        if (node->childs[index]->assic == ">")
            op = RelationalExpr::OP_GT;
        else if (node->childs[index]->assic == "<")
            op = RelationalExpr::OP_LT;
        else if (node->childs[index]->assic == ">=")
            op = RelationalExpr::OP_GTEQ;
        else if (node->childs[index]->assic == "<=")
            op = RelationalExpr::OP_LTEQ;
        else 
            throw Exception::InvalidStatement(node->assic);        
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for shift expression
AST* ASTBuilder::handleShiftExpr(Node *node) 
{
    if (node->count() == 1)
        return handleAdditiveExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleAdditiveExpr(node->childs[0]);
    ShiftExpr *expr = new ShiftExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expr *target = (Expr *)handleAdditiveExpr(node->childs[index]);
        int op = -1;
        if (node->childs[index]->assic == ">>")
            op = ShiftExpr::OP_RSHIFT;
        else if (node->childs[index]->assic == "<<")
            op = ShiftExpr::OP_LSHIFT;
        else 
            throw Exception::InvalidStatement(node->assic);
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for additive expression
AST* ASTBuilder::handleAdditiveExpr(Node *node) 
{
    if (node->count() == 1)
        return handleMultiplicativeExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleMultiplicativeExpr(node->childs[0]);
    AdditiveExpr *expr = new AdditiveExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expr *target = 
        (Expr *)handleMultiplicativeExpr(node->childs[index]);
        int op = -1;
        if (node->childs[index]->assic == "+")
            op = AdditiveExpr::OP_PLUS;
        else if (node->childs[index]->assic == "-")
            op = AdditiveExpr::OP_SUB;
        else 
            throw Exception::InvalidStatement(node->assic);
          
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for multiplicative expression
AST* ASTBuilder::handleMultiplicativeExpr(Node *node) 
{
    if (node->count() == 1)
        return handleUnaryExpr(node->childs[0]);
    
    Expr *leftExpr = (Expr *)handleUnaryExpr(node->childs[0]);
    AdditiveExpr *expr = new AdditiveExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expr *target = 
        (Expr *)handleUnaryExpr(node->childs[index]);
        int op = -1;
        if (node->childs[index]->assic == "*")
            op = MultiplicativeExpr::OP_MUL;
        else if (node->childs[index]->assic == "/")
            op = MultiplicativeExpr::OP_DIV;
        else if (node->childs[index]->assic == "%")
            op = MultiplicativeExpr::OP_MODULO;    
        else 
            throw Exception::InvalidStatement(node->assic);
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder for unary expression
AST* ASTBuilder::handleUnaryExpr(Node *node) 
{
    PrimaryExpr *expr = (PrimaryExpr *)handlePrimary(node->childs[0]);
    if (node->count() == 1)
        return expr;
    
    UnaryExpr *unaryExpr = new UnaryExpr(expr, node->location);  
    for (int index = 1; index < node->count(); index++) {
        SelectorExpr *sel = (SelectorExpr *)handleSelector(node->childs[index]);
        unaryExpr->appendElement(sel);
    }
    
    return unaryExpr;
}

/// @brief ASTBuilder handler for primary expression
AST* ASTBuilder::handlePrimary(Node *node) 
{
    string text = node->childs[0]->assic;
    Expr *expr = NULL; 
    if (text == "self")
        return new PrimaryExpr(PrimaryExpr::T_SELF, node->location);
    
    if (text == "super")
        return new PrimaryExpr(PrimaryExpr::T_SUPER, node->location);
    
    if (text == "true")
        return new PrimaryExpr(PrimaryExpr::T_TRUE, node->location);
    
    if (text == "false")
        return new PrimaryExpr(PrimaryExpr::T_FALSE, node->location);
    
    if (text == "null")
        return new PrimaryExpr(PrimaryExpr::T_NULL, node->location);
    
    if (text == "mapLiteral")
        return handleMapExpr(node->childs[0]);
    
    if (text == "setLiteral")
        return handleSetExpr(node->childs[0]);
    
    if (text == "identifer")
        return new PrimaryExpr(PrimaryExpr::T_IDENTIFIER, node->childs[0]->assic, node->location);
    
    if (text == "NUMBER")
        return new PrimaryExpr(PrimaryExpr::T_NUMBER, node->childs[0]->assic, node->location);
    
    if (text == "HEX_NUMBER")
        return new PrimaryExpr(PrimaryExpr::T_NUMBER, node->childs[0]->assic, node->location);
    
    if (node->count() == 3) { // compound expression 
		expr = (Expr *) handleExpr(node->childs[1]);
        return new PrimaryExpr(PrimaryExpr::T_COMPOUND, expr, node->location);
    }
    throw Exception::InvalidExpr(text);
    return NULL;
    
}

/// @brief ASTBuilder handler for selector
AST* ASTBuilder::handleSelector(Node *node) 
{
    SelectorExpr *selExpr = NULL;
    
    if (node->childs[0]->assic == "assignalbeSelector") {
        Node *subNode = node->childs[0];
        if (subNode->count() == 2) {// .identifier
            selExpr = new SelectorExpr(subNode->childs[1]->assic, node->location);
            selExpr->m_type = SelectorExpr::DOT_SELECTOR;
            return selExpr;
        }
        
        else if (subNode->count() == 3) { // [ expression ]
            selExpr =  new SelectorExpr(node->location);
            selExpr->m_arrayExpr = (Expr *)handleExpr(subNode->childs[1]);
            selExpr->m_type = SelectorExpr::ARRAY_SELECTOR;
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    else if (node->childs[0]->assic == "arguments") {
        Node *subNode = node->childs[0];
   
        selExpr = new SelectorExpr(node->location);
        selExpr->m_type = SelectorExpr::METHOD_SELECTOR;
        selExpr->m_methodCallExpr = new MethodCallExpr(node->location);
   
        if (subNode->count() == 2)  // no argument
            return selExpr;
        
        else if (subNode->count() == 3) {
            subNode = subNode->childs[1];
            for (int index = 0; index < subNode->count(); index += 2) {
                Expr *expr = (Expr *)handleExpr(subNode->childs[index]);
                selExpr->m_methodCallExpr->appendArgument(expr);
            }
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    return NULL;
    
}

/// @brief ASTBuilder handler for new expression
AST* ASTBuilder::handleNewExpr(Node *node) 
{
    string type;
    // the current node is pointed to  arguments
    
    if (node->count() == 1) { // no parameter, just new id;
        type = node->childs[0]->childs[0]->assic;
        return new NewExpr(type, node->location);
    }
    else if (node->count() == 2) { // with arguments, new id arguments
        // new id arguments
        type = node->childs[0]->childs[0]->assic;
        NewExpr *newExpr = new NewExpr(type, node->location);
        
        if (node->childs[1]->count() == 3) {
            node = node->childs[1]->childs[1];
            for (int index = 0; index < node->childs[1]->count(); index += 2) {
                Expr *expr = (Expr *)handleExpr(node->childs[1]->childs[index]);
                newExpr->appendArgument(expr);
            }
        }
        return newExpr;
    }
    else {
        Error::complain("the new expression is not right\n");
        return NULL;
    }
}
/// @brief ASTBuilder handler for map expression
AST* ASTBuilder::handleMapExpr(Node *node) 
{
    MapExpr *mapExpr = new MapExpr(node->location);
    if (node->count() == 3) {
        Node *itemNode = node->childs[1];
        for (int index = 0; index < itemNode->count(); index += 2) {
            MapItemExpr *item = (MapItemExpr *) handleMapItemExpr(itemNode->childs[index]);
            mapExpr->appendItem(item);
        }
    }
    return mapExpr;
}

/// @brief ASTBuilder handler for map expression
AST* ASTBuilder::handleMapItemExpr(Node *node) 
{
    Expr *key = (Expr*) handleExpr(node->childs[0]);
    Expr *val = (Expr*) handleExpr(node->childs[2]);
    return new MapItemExpr(key, val, node->location);
}

/// @brief ASTBuilder handler for set expression
AST* ASTBuilder::handleSetExpr(Node *node) 
{
    ExprList *exprList = (ExprList *) handleExprList(node->childs[1]);
    return new SetExpr(exprList, node->location);
}
   


AST* ASTBuilder::handleMapLiteral(Node *node)
{
    return NULL;
}
AST* ASTBuilder::handleSetLiteral(Node *node)
{
    return NULL;
}

