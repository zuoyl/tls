//
//  ASTBuilder.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Common.h"
#include <algorithm>
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
#include "ASTxml.h"
#include "Compile.h"

ASTBuilder::ASTBuilder(const string& path, const string& file) 
{
    m_path = path;
    m_file = file;
}

ASTBuilder::~ASTBuilder() 
{
    
}

// method to convert a parse tree into an AST tree
AST* ASTBuilder::build(Node* parseTree) 
{
    if (!parseTree) {
        Error::complain("the parse tree is null, just return\n");
        return NULL;
    }
    
    AST*  root = new AST();
    // the root node of pareTree must be compile unit
    vector<Node* >::iterator ite = parseTree->childs.begin();
    for (; ite != parseTree->childs.end(); ite++) {
        Node*  decls = *ite;
        AST* child = handleDecl(decls->childs[0]);
        if (child)
            root->addChildNode(child);
    }
    if (CompileOption::getInstance().isOutputAST()) {
        ASTXml xml(m_path, m_file);
        xml.build(root);
    }
    
    return root;
}

/// handle the declaration
AST* ASTBuilder::handleDecl(Node* node)
{
    if (node->assic == "includeDeclaration")
        return  handleIncludeDecl(node);
    else if (node->assic == "classDeclaration")
        return  handleClassDecl(node);
    else if (node->assic == "methodImplementation")
        return handleMethodImplementation(node);
    else  
        Error::complain(node->location, "the Parse Tree is not right\n");
    return NULL; 
}

/// handler for include Declaration
AST* ASTBuilder::handleIncludeDecl(Node* node) 
{
    string fullName = node->childs[1]->assic; 
    // make new ast
    return new IncludeStatement(fullName, node->location);
}

/// handler for class declaration 
AST* ASTBuilder::handleClassDecl(Node* node) 
{
    int index = 0;
    bool isPublic = false;
    bool isFinal = false;
    bool isAbstract = false;    
    // get scope specifier
    if (node->childs[index]->assic == "scopeSpecifier") {
        if (node->childs[index]->childs[0]->assic == "public")
            isPublic = true;
        index++;
    }
    if (node->childs[index]->assic == "classSignature") {
        if (node->childs[index]->childs[0]->assic == "final")
            isFinal = true;
        else if (node->childs[index]->childs[0]->assic == "abstract")
            isAbstract = true;
        index++;
    }
    
    // get id, skip 'class'
    index++;
    string id = node->childs[index++]->assic;
    vector<string> baseList;
    vector<string> abstractClassList;
    
    // loop to check the basis class and abstract class 
    while (index < (node->count() - 1)) {
        // check the base class and abstract class 
        // 'extend class1, class2,...
        if (node->childs[index]->assic == "classInheritDeclaration") {
            Node* subroot = node->childs[index];
            // get base class
            // 'extend' identifer (',' identifer)*
            for (int childIndex = 1; childIndex < subroot->count(); childIndex++) {
                baseList.push_back(subroot->childs[childIndex]->assic);
                childIndex += 2;
            }
            index++; 
        }
        // 'implement class1, class2,...
        else if (node->childs[index]->assic == "abstractClassImplementDeclaration") {
            Node* subroot = node->childs[index];
            // 'implement' identifier (',' identifier)*
            for (int childIndex = 1; childIndex < subroot->count(); childIndex++) {
                abstractClassList.push_back(subroot->childs[childIndex]->assic);
                childIndex += 2;
            }
            index++; 
        }
    }
    
    // get class block
    Node* blockNode = node->childs[node->count() -1];
    ClassBlock* clsBlock = (ClassBlock*)handleClassBlock(blockNode, id);

    return new Class(isPublic, isFinal, isAbstract, id, baseList, abstractClassList, clsBlock, node->location);
    
}

/// hanlder for class block
AST* ASTBuilder::handleClassBlock(Node* node, const string& cls) 
{
    ClassBlock* block = new ClassBlock(node->location);
    
    if (node->count() == 2)
        return block;
    
    for (int index = 1; index < node->count() - 1; index++) {
        bool isPublic = false;
        bool isConst = false; 
        bool isStatic = false;
        TypeSpec* typeSpec = NULL; 
        string memberName; 
        
        // check member's publicity
        if (node->childs[index]->assic == "scopeSpecifier") {
            if (node->childs[index]->childs[0]->assic == "public") 
                isPublic = true;
            index++; 
        }
        
        // class member declaration
        if (node->childs[index]->assic == "classMember") {
            Node* member = node->childs[index];
            size_t m = 0; 
        
            if (member->childs[m]->assic == "storageSpecifier") {
                if (member->childs[m]->childs[0]->assic == "static")
                    isStatic = true;
                m++;
            }
            if (member->childs[m]->assic == "constSpecifier") {
                if (member->childs[m]->childs[0]->assic == "const") 
                    isConst = true;
                m++; 
            }
            typeSpec = (TypeSpec*)handleTypeDecl(member->childs[m++]); 
            memberName = member->childs[m++]->assic;

            if (member->childs[m]->assic == "methodDeclSelector") {
                // it must be a method
                MethodParameterList* paraList =
                    (MethodParameterList*)handleMethodParameterList(member->childs[m]->childs[0]);
                Method* method = new Method(typeSpec, memberName, paraList, member->childs[m]->location); 
                method->m_isPublic = isPublic;
                method->m_isConst = isConst; 
                method->m_isStatic = isStatic; 
                method->m_isOfClass = true; 
                method->m_class = cls; 
                method->m_paraList = paraList; 
                method->m_isDeclaration = true; 
                block->addMethod(method);
            }
            else {
                // check wethere there is a initialization expression
                Expr* expr = NULL;
                bool isInitialized = false;
                if (member->childs[m]->count() > 1) { 
                    expr = (Expr*)handleExpr(member->childs[m]->childs[1]);
                    isInitialized = true;
                }
                Variable* var = new Variable(isPublic, isConst, typeSpec, memberName, expr, member->childs[m]->location );
                var->m_name = memberName; 
                var->m_isStatic = isStatic;
                var->m_isInitialized = isInitialized;
                var->m_class = cls;
                block->addVariable(var);
            }
        }
    }
    return block;
}


/// handler for class method
AST* ASTBuilder::handleMethodDecl(Node* node) 
{
    // return type and method name
    TypeSpec* retTypeSpec = 
        (TypeSpec*)handleTypeDecl(node->childs[0]);
    string methodName = node->childs[1]->assic;
    // method parameter list
    MethodParameterList* methodParameterList = 
        (MethodParameterList*)handleMethodParameterList(node->childs[2]);    
    // make AST tree
    Method* method =  new Method(retTypeSpec, methodName, methodParameterList, node->location);
    return method;
}

AST* ASTBuilder::handleMethodImplementation(Node* node)
{
    bool isStatic = false;
    bool isConst = false;
    int  index = 0; 

    if (node->childs[index]->assic == "storageSpecifier") {
        if (node->childs[index]->childs[0]->assic == "static")
            isStatic = true;
            index++;
    }
    if (node->childs[index]->assic == "constSpecifier") {
        if (node->childs[index]->childs[0]->assic == "const") 
            isConst = true;
            index++; 
    }
    TypeSpec* typeSpec = (TypeSpec*)handleTypeDecl(node->childs[index++]);
    string clsName = node->childs[index++]->assic;
    Location location = node->childs[index]->location; 
    index++; //skip the selector operator'::'
    string methodName = node->childs[index++]->assic;
    MethodParameterList* paraList = 
        (MethodParameterList*)handleMethodParameterList(node->childs[index++]);
    MethodBlock* methodBlock = (MethodBlock*)handleMethodBlock(node->childs[index]);
    Method* method = new Method(location);
    method->m_paraList = paraList;
    method->m_name = methodName;
    method->m_class = clsName;
    method->m_isOfClass = true; 
    method->m_isDeclaration = false;
    method->m_block = methodBlock;
    method->m_retTypeSpec = typeSpec;
    return method;
}

/// hanlder handler for type specifierr
AST* ASTBuilder::handleTypeDecl(Node* node) 
{
    TypeSpec* typeSpec = new TypeSpec(node->location);
    Node* snode = node->childs[0];

    if (snode->assic == "basicType") {
        typeSpec->m_name = snode->childs[0]->assic;
        if (typeSpec->m_name == "void")
            typeSpec->m_typeid = TypeSpec::voidType;
        else if (typeSpec->m_name == "bool")
            typeSpec->m_typeid = TypeSpec::boolType;
        else if (typeSpec->m_name == "int")
            typeSpec->m_typeid = TypeSpec::intType;
        else if (typeSpec->m_name == "string")
            typeSpec->m_typeid = TypeSpec::stringType;
        else if (typeSpec->m_name == "float")
            typeSpec->m_typeid = TypeSpec::floatType;
    }
    else if (snode->assic == "mapType") {
        typeSpec->m_name = "map";
        typeSpec->m_typeid = TypeSpec::mapType;
        typeSpec->m_t1 = snode->childs[2]->childs[0]->assic;
        typeSpec->m_t2 = snode->childs[3]->childs[0]->assic;
    }
    else if (snode->assic == "setType") {
        typeSpec->m_name = "set";
        typeSpec->m_typeid = TypeSpec::setType;
        typeSpec->m_t1 = snode->childs[2]->childs[0]->assic;        
    }
    else {
        typeSpec->m_name = snode->assic;
        typeSpec->m_typeid = TypeSpec::customType;
    }
    return typeSpec;
}

/// handler for variable declaration
AST* ASTBuilder::handleVarDecl(Node* node) 
{
    bool isStatic = false;
    bool isConst = false;
    int index = 1; // skip the 'local' keyword
    int childCount = (int)node->childs.size();
    Expr* expr = NULL;
    
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
    TypeSpec* typeSpec = 
        (TypeSpec*)handleTypeDecl(node->childs[index++]);
    string id = node->childs[index++]->assic;
    
    // expression initializer
    if ((index + 1) < childCount) {
        // check to see wether have expression initialization list
        expr = (Expr*)handleExpr(node->childs[index + 1]);
    }
    return new Variable(isStatic, isConst, typeSpec, id, expr, node->location);
}


/// handler for method parameter list
AST* ASTBuilder::handleMethodParameterList(Node* node) 
{
    int childCount = node->count();
    MethodParameterList* paraList = new MethodParameterList(node->location);
    MethodParameter* parameter = NULL;
    
    if (childCount == 2) // no parameter
        return paraList;
    else if (childCount == 3)  { // with only normal parameter
        // handle normal parameter
        for (int index = 0; index < node->childs[1]->count(); index++) {
            parameter = (MethodParameter*)
                handleMethodNormalParameter(node->childs[1]->childs[index]);
            paraList->addParameter(parameter);
            index++; // skop the token ','
        }
    }
    else if (childCount == 5) { // with default parameter
        // handle normal parameter
        for (int index = 0; index < node->childs[1]->count(); index++) {
            parameter = (MethodParameter*)
                handleMethodNormalParameter(node->childs[1]->childs[index]);
            paraList->addParameter(parameter);
            index++; // skop the token ','
        }
        
        // handle default parameter
        for (int index = 0; index < node->childs[4]->count(); index++) {
            parameter = (MethodParameter*)
                handleMethodDefaultParameter(node->childs[4]->childs[index]);
            paraList->addParameter(parameter);
            index++;
        }
    }
    
    return paraList;
}

/// handler for method normal parameter
AST* ASTBuilder::handleMethodNormalParameter(Node* node) 
{
    bool isConst = false;
    int index = 0;
    
    // get const specifier
    if (node->count() == 3) {
        isConst = true;
        index = 1;
    }
    
    // get type name and id
    TypeSpec* typeSpec = 
        (TypeSpec*)handleTypeDecl(node->childs[index]);
    string id = node->childs[index + 1]->assic;
    return new MethodParameter(isConst, typeSpec, id, false, NULL, node->location);
    
}

/// handler for method default parameter 
AST* ASTBuilder::handleMethodDefaultParameter(Node* node) 
{
    MethodParameter*  para = (MethodParameter*) handleMethodNormalParameter(node->childs[0]);
    
    if (node->count() == 3)
        para->m_default = (Expr*)handleExpr(node->childs[2]);
    
    return para;
    
}

/// handler for method block
AST* ASTBuilder::handleMethodBlock(Node* node) 
{
    MethodBlock* block = new MethodBlock(node->location);
    
    for (int index = 1; index < node->count() - 1; index++) {
        Statement* stmt = (Statement*)handleStatement(node->childs[index]);
        if (!stmt) {
            Error::complain(node->childs[index]->location, "can not resolve the statement");
            continue; 
        }
        block->addStatement(stmt);
    }
    return block;
}


//
// Statements
//
/// handler for statement
AST* ASTBuilder::handleStatement(Node* node) 
{
    string type = node->childs[0]->assic;
    
    if (type == "blockStatement")
        return handleBlockStatement(node->childs[0]);
    else if (type == "varDeclStatement")
        return handleVarDeclStatement(node->childs[0]);
    else if (type == "ifStatement")
        return handleIfStatement(node->childs[0]);
    else if (type == "forStatement")
        return handleForStatement(node->childs[0]);
    else if (type == "foreachStatement")
        return handleForEachStatement(node->childs[0]);
    else if (type == "doStatement")
        return handleDoStatement(node->childs[0]);
    else if (type == "whileStatement")
        return handleWhileStatement(node->childs[0]);
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
        return handleExprStatement(node->childs[0]);
    else if (type == "methodCallStatement")
        return handleMethodCallStatement(node->childs[0]);
    else {
        throw Exception::InvalidStatement(type);
        return NULL;
    }
}

/// handler for block statment
AST* ASTBuilder::handleBlockStatement(Node* node) 
{
    BlockStatement* blockStmt = new BlockStatement(node->location);
    for (int index = 1; index < node->count() - 1; index++) {
        Statement*  stmt = (Statement*)handleStatement(node->childs[index]);
        stmt->setParentNode(blockStmt);
        VariableDeclStatement* varDeclStmt = 
                    dynamic_cast<VariableDeclStatement*>(stmt); 
        if (varDeclStmt)
            blockStmt->m_vars.push_back(varDeclStmt->m_var);     
            
        blockStmt->addStatement(stmt);
    }
    return blockStmt;
}

/// handler for variable Decl statement
AST* ASTBuilder::handleVarDeclStatement(Node* node) 
{
    Variable* var = (Variable*)handleVarDecl(node);
    var->m_isGlobal = false;
    var->m_isOfClass = false;
    var->m_isPublic = false;
   
    return new VariableDeclStatement(var, var->m_expr, node->location);
}

/// handler for if statement
AST* ASTBuilder::handleIfStatement(Node* node) 
{
    Expr* conditExpr = (Expr*)handleCompareExpr(node->childs[2]);
    Statement* stmt1 = (Statement*)handleStatement(node->childs[4]);
    Statement* stmt2 = NULL;
    if (node->count() == 7) {
        stmt2 = (Statement*)handleStatement(node->childs[6]);
    }
    return new IfStatement(conditExpr, stmt1, stmt2, node->location);
}

/// handler for for statement
AST* ASTBuilder::handleForStatement(Node* node) 
{
    int index = 2;
    int sindex = 0;
    Expr* exprs[2] = { NULL, NULL};
    ExprList* exprList = NULL;
    
    // 'for' '('expression? ';' comparisonExpression? ';' expressionList? ')' statement
   
    if (node->childs[index]->assic == "expression")
        exprs[0] = (Expr*)handleExpr(node->childs[index++]);
    index++;

    if (node->childs[index]->assic == "comparisonExpr")
        exprs[1] = (Expr*)handleCompareExpr(node->childs[index++]);
    index++;

    if (node->childs[index]->assic == "expressionList")
        exprList = (ExprList*)handleExprList(node->childs[index++]);
    index++;

    Statement* stmt = (Statement*)handleStatement(node->childs[index]);
    return new ForStatement(exprs[0], exprs[1], exprList, stmt, node->location);
}

/// handler for foreach statement
AST* ASTBuilder::handleForEachStatement(Node* node) 
{
    int index = 2;
    ForEachStatement* stmt = new ForEachStatement(node->location);
    
    // check the foreachVarItem
    if (node->childs[index]->assic == "foreachVarItem") {
        stmt->m_varNumbers++;
        Node* snode = node->childs[index];
        stmt->m_typeSpec[0] = (TypeSpec*)handleTypeDecl(snode->childs[0]);
        stmt->m_id[0] = snode->childs[1]->assic;
        index++; 
    }
    if (node->childs[index]->assic == ",") {
        index++;
        if (node->childs[index]->assic == "foreachVarItem") {
            stmt->m_varNumbers++;
            Node* snode = node->childs[index];
            stmt->m_typeSpec[1] = (TypeSpec*)handleTypeDecl(snode->childs[0]);
            stmt->m_id[1] = snode->childs[1]->assic;
            index++; 
        }
    }
    index ++; // skip the 'in' keyword
    Node* snode = node->childs[index];
    string type = node->childs[index]->childs[0]->assic;
    if (type == "mapListeral") {
        stmt->m_objectSetType = ForEachStatement::MapObject;
        stmt->m_expr = (Expr*)handleMapLiteral(snode->childs[0]);
    }
    else if (type == "setListeral") {
        stmt->m_objectSetType = ForEachStatement::SetObject;
        stmt->m_expr = (Expr*)handleSetLiteral(snode->childs[0]);        
    }
    else {
        stmt->m_objectSetName = snode->childs[0]->assic; 
        stmt->m_objectSetType = ForEachStatement::Object;
    }
    stmt->m_stmt = (Statement*)handleStatement(node->childs[node->count() -1]);  
    return stmt;
}

/// handler for while statement
AST* ASTBuilder::handleWhileStatement(Node* node) 
{
    Expr* conditExpr = (Expr*)handleCompareExpr(node->childs[2]);
    Statement* stmt = (Statement*)handleStatement(node->childs[4]);
    return new WhileStatement(conditExpr, stmt, node->location);
}

/// handler for do while statement
AST* ASTBuilder::handleDoStatement(Node* node) 
{
    Expr* conditExpr = (Expr*)handleCompareExpr(node->childs[2]);
    Statement* stmt = (Statement*)handleStatement(node->childs[4]);
    return new DoStatement(conditExpr, stmt, node->location);
}

/// handler for switch statement
AST* ASTBuilder::handleSwitchStatement(Node* node) 
{
    Expr* resultExpr = (Expr*)handleExpr(node->childs[2]);
    SwitchStatement* switchStmt  = new SwitchStatement(resultExpr, node->location);
    
    for (int index = 5; index < node->count(); index++) {
        if (node->childs[index]->assic == "switchCase") {
            Node* subnode = node->childs[index];
            vector<Expr* >* exprList = new vector<Expr* >();
            for (int subIndex = 0; subIndex < subnode->count() - 1; subIndex += 3) {
                Expr* caseExpr = 
                (Expr*)handleExpr(subnode->childs[subIndex + 1]);
                exprList->push_back(caseExpr);
            }
            Statement* stmt = 
            (Statement*)handleSwitchStatement(subnode->childs[subnode->count() - 1]);
            switchStmt->addCaseStatement(exprList, stmt);
            
        }
        else if (node->childs[index]->assic == "defaultCase") {
            Node* subnode = node->childs[index];
            Statement* stmt = (Statement*)handleSwitchStatement(subnode->childs[2]);
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

/// hander for return statement
AST* ASTBuilder::handleReturnStatement(Node* node) 
{
    Expr* expr = NULL;
    if (node->count() == 3)
        expr = (Expr*)handleExpr(node->childs[1]);
    return new ReturnStatement(expr, node->location);
}
/// hander for assert statement
AST* ASTBuilder::handleAssertStatement(Node* node) 
{
    Expr*  expr = (Expr*)handleExpr(node->childs[1]);
    return new AssertStatement(expr, node->location);
}

/// hander for break statement
AST* ASTBuilder::handleBreakStatement(Node* node) 
{
    return new BreakStatement(node->location);
}

/// hander for continue statement
AST* ASTBuilder::handleContinueStatement(Node* node) 
{
    return new ContinueStatement(node->location);
}
/// hander for throw statement
AST* ASTBuilder::handleThrowStatement(Node* node) 
{
    Expr* expr = NULL;
    if (node->count() == 3)
        expr = (Expr*)handleExpr(node->childs[1]);
    return new ThrowStatement(expr, node->location);
}

/// hander for try statement
AST* ASTBuilder::handleTryStatement(Node* node) 
{
    BlockStatement* blockStmt = 
        (BlockStatement*)handleBlockStatement(node->childs[1]);
    TryStatement* tryStmt = new TryStatement(blockStmt, node->location);
    
    for (int index = 2; index < node->count(); index ++) {
        if (node->childs[index]->assic == "catchPart") {
            CatchStatement* catchStmt = 
                        (CatchStatement*)handleCatchStatement(node->childs[index]);
            tryStmt->addCatchPart(catchStmt);
        }
        else if (node->childs[index]->assic == "finallyPart") {
            FinallyCatchStatement* finallyStmt = (FinallyCatchStatement*)
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

/// hander for catch statement
AST* ASTBuilder::handleCatchStatement(Node* node) 
{
    string type = node->childs[1]->childs[0]->assic;
    string id = node->childs[2]->childs[0]->assic;
    BlockStatement* blockStmt = 
                (BlockStatement*)handleBlockStatement(node->childs[5]);
    
    return new CatchStatement(type, id, blockStmt, node->location);
}

/// hander for finally catch statement
AST* ASTBuilder::handleFinallyCatchStatement(Node* node) 
{
    BlockStatement* blockStmt = (BlockStatement*)handleBlockStatement(node->childs[1]);
    return new FinallyCatchStatement(blockStmt, node->location);
}

/// handler for expression statement
AST* ASTBuilder::handleExprStatement(Node* node) 
{
    ExprStatement* stmt = new ExprStatement(node->location);
    stmt->m_target = (Expr*)handleExpr(node->childs[0]);
    for (size_t index = 1; index < node->count() - 1; index++) {
        string op = node->childs[index++]->childs[0]->assic;
        Expr* expr = (Expr*)handleExpr(node->childs[index]);
        stmt->addElement(op, expr);
    }
    return stmt;
}

/// hander for funcation statement
AST* ASTBuilder::handleMethodCallStatement(Node* node) 
{
    return NULL;
}

/// handler for expression list
AST* ASTBuilder::handleExprList(Node* node) 
{
    ExprList* exprList = new ExprList(node->location);
    Expr* expr = NULL;
    
    for (int index = 0; index < node->count(); index++) {
        expr = (Expr*)handleExpr(node->childs[index]);
        exprList->appendExpr(expr);
        index++;
    }
    return exprList;
}

/// handler for expression

AST* ASTBuilder::handleCompareExpr(Node* node)
{
    if (node->count() == 1)
        return handleExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleExpr(node->childs[0]);
    ComparisonExpr* comparExpr = new ComparisonExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index ++ ) {
        string op = node->childs[index++]->assic; 
        Expr* target = (Expr*)handleExpr(node->childs[index]);
        comparExpr->appendElement(op, target);
    }
    return comparExpr;
}

AST* ASTBuilder::handleExpr(Node* node) 
{
    if (node->childs[0]->assic == "logicalOrExpr")
        return handleLogicOrExpr(node->childs[0]);
    else if (node->childs[0]->assic == "newExpr")
        return handleNewExpr(node->childs[0]);
    else
        return NULL;


#if 0
    if (node->count() == 1)
        return handleConditionalExpr(node);
    
    Expr* leftExpr = (Expr*)handleAssignableExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    Expr* rightExpr = (Expr*)handleExpr(node->childs[2]);
    return new BinaryOpExpr(op, leftExpr, rightExpr, node->location);
#endif 
}

/// handler for assignalbe expression
AST* ASTBuilder::handleAssignableExpr(Node* node) 
{
    return NULL;
}

/// handler for conditional expression
AST* ASTBuilder::handleConditionalExpr(Node* node) 
{
    return handleLogicOrExpr(node);
}

/// handler for logic or expression
AST* ASTBuilder::handleLogicOrExpr(Node* node) 
{
    if (node->count() == 1)
        return handleLogicAndExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleLogicAndExpr(node->childs[0]);
    LogicOrExpr* logicOrExpre = new LogicOrExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleLogicAndExpr(node->childs[index]);
        logicOrExpre->appendElement(target);
    }
    return logicOrExpre;
}

/// handler for logic and expression
AST* ASTBuilder::handleLogicAndExpr(Node* node) 
{
    if (node->count() == 1)
        return handleBitwiseOrExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleBitwiseOrExpr(node->childs[0]);
    LogicAndExpr* logicAndExpr = new LogicAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleBitwiseAndExpr(node->childs[index]);
        logicAndExpr->appendElement(target);
    }
    return logicAndExpr;
}

/// handler  for bitwise or expression
AST* ASTBuilder::handleBitwiseOrExpr(Node* node) 
{
    if (node->count() == 1)
        return handleBitwiseXorExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleBitwiseXorExpr(node->childs[0]);
    BitwiseOrExpr* expr = new BitwiseOrExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleBitwiseXorExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}


/// handler for bitwise xor expression
AST* ASTBuilder::handleBitwiseXorExpr(Node* node) 
{
    if (node->count() == 1)
        return handleBitwiseAndExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleBitwiseAndExpr(node->childs[0]);
    BitwiseAndExpr* expr = new BitwiseAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleBitwiseAndExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// handler for bitwise and expression
AST* ASTBuilder::handleBitwiseAndExpr(Node* node) 
{
    if (node->count() == 1)
        return handleEqualityExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleEqualityExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    BitwiseAndExpr* expr = new BitwiseAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleEqualityExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// handler for equality expression
AST* ASTBuilder::handleEqualityExpr(Node* node) 
{
    if (node->count() == 1)
        return handleRelationalExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleRelationalExpr(node->childs[0]);
    EqualityExpr* expr = new EqualityExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        int op = -1;
        if (node->childs[index]->childs[0]->assic == "==")
            op = EqualityExpr::OP_EQ;
        else if (node->childs[index]->childs[0]->assic == "!=")
            op = EqualityExpr::OP_NEQ;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        Expr* target = (Expr*)handleRelationalExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for relational expression
AST* ASTBuilder::handleRelationalExpr(Node* node) 
{
    if (node->count() == 1)
        return handleShiftExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleShiftExpr(node->childs[0]);
    RelationalExpr* expr = new RelationalExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        
        int op = -1;
        if (node->childs[index]->childs[0]->assic == ">")
            op = RelationalExpr::OP_GT;
        else if (node->childs[index]->childs[0]->assic == "<")
            op = RelationalExpr::OP_LT;
        else if (node->childs[index]->childs[0]->assic == ">=")
            op = RelationalExpr::OP_GTEQ;
        else if (node->childs[index]->childs[0]->assic == "<=")
            op = RelationalExpr::OP_LTEQ;
        else 
            throw Exception::InvalidStatement(node->assic);        
        index++; 
        Expr* target = (Expr*)handleShiftExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for shift expression
AST* ASTBuilder::handleShiftExpr(Node* node) 
{
    if (node->count() == 1)
        return handleAdditiveExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleAdditiveExpr(node->childs[0]);
    ShiftExpr* expr = new ShiftExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        int op = -1;
        if (node->childs[index]->childs[0]->assic == ">>")
            op = ShiftExpr::OP_RSHIFT;
        else if (node->childs[index]->childs[0]->assic == "<<")
            op = ShiftExpr::OP_LSHIFT;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        Expr* target = (Expr*)handleAdditiveExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for additive expression
AST* ASTBuilder::handleAdditiveExpr(Node* node) 
{
    if (node->count() == 1)
        return handleMultiplicativeExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleMultiplicativeExpr(node->childs[0]);
    AdditiveExpr* expr = new AdditiveExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        int op = -1;
        if (node->childs[index]->childs[0]->assic == "+")
            op = AdditiveExpr::OP_PLUS;
        else if (node->childs[index]->childs[0]->assic == "-")
            op = AdditiveExpr::OP_SUB;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        Expr* target = (Expr*)handleMultiplicativeExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for multiplicative expression
AST* ASTBuilder::handleMultiplicativeExpr(Node* node) 
{
    if (node->count() == 1)
        return handleUnaryExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleUnaryExpr(node->childs[0]);
    AdditiveExpr* expr = new AdditiveExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        int op = -1;
        if (node->childs[index]->childs[0]->assic == "*")
            op = MultiplicativeExpr::OP_MUL;
        else if (node->childs[index]->childs[0]->assic == "/")
            op = MultiplicativeExpr::OP_DIV;
        else if (node->childs[index]->childs[0]->assic == "%")
            op = MultiplicativeExpr::OP_MODULO;    
        else 
            throw Exception::InvalidStatement(node->assic);
        index++;     
        Expr* target = (Expr*)handleUnaryExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// hanlder for unary expression
AST* ASTBuilder::handleUnaryExpr(Node* node) 
{
    PrimaryExpr* expr = (PrimaryExpr*)handlePrimary(node->childs[0]);
    if (node->count() == 1)
        return expr;
    
    UnaryExpr* unaryExpr = new UnaryExpr(expr, node->location);  
    for (int index = 1; index < node->count(); index++) {
        SelectorExpr* sel = (SelectorExpr*)handleSelector(node->childs[index]);
        unaryExpr->appendElement(sel);
    }
    
    return unaryExpr;
}

/// handler for primary expression
AST* ASTBuilder::handlePrimary(Node* node) 
{
    string text = node->childs[0]->assic;
    Expr* expr = NULL; 
    if (text == "self")
        return new PrimaryExpr(PrimaryExpr::T_SELF, text, node->location);
    
    if (text == "super")
        return new PrimaryExpr(PrimaryExpr::T_SUPER, text, node->location);
    
    if (text == "true")
        return new PrimaryExpr(PrimaryExpr::T_TRUE, text, node->location);
    
    if (text == "false")
        return new PrimaryExpr(PrimaryExpr::T_FALSE, text, node->location);
    
    if (text == "null")
        return new PrimaryExpr(PrimaryExpr::T_NULL, text, node->location);
    
    if (text == "mapLiteral")
        return handleMapExpr(node->childs[0]);
    
    if (text == "setLiteral")
        return handleSetExpr(node->childs[0]);
    
    
    if (text == "NUMBER")
        return new PrimaryExpr(PrimaryExpr::T_NUMBER, node->childs[0]->assic, node->location);
    
    if (text == "HEXNUMBER")
        return new PrimaryExpr(PrimaryExpr::T_NUMBER, node->childs[0]->assic, node->location);
    
    if (node->count() == 3) { // compound expression 
		expr = (Expr*) handleExpr(node->childs[1]);
        return new PrimaryExpr(PrimaryExpr::T_COMPOUND, expr, node->location);
    }
    return new PrimaryExpr(PrimaryExpr::T_IDENTIFIER, node->childs[0]->assic, node->location);
}

/// handler for selector
AST* ASTBuilder::handleSelector(Node* node) 
{
    SelectorExpr* selExpr = NULL;
    
    if (node->childs[0]->assic == "assignableSelector") {
        Node* subNode = node->childs[0];
        if (subNode->count() == 2) {// .identifier
            selExpr = new SelectorExpr(SelectorExpr::DOT_SELECTOR, subNode->childs[1]->assic, node->location);
            return selExpr;
        }
        
        else if (subNode->count() == 3) { // [ expression ]
            selExpr =  new SelectorExpr(node->location);
            selExpr->m_arrayExpr = (Expr*)handleExpr(subNode->childs[1]);
            selExpr->m_type = SelectorExpr::ARRAY_SELECTOR;
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    else if (node->childs[0]->assic == "arguments") {
        Node* subNode = node->childs[0];
   
        selExpr = new SelectorExpr(node->location);
        selExpr->m_type = SelectorExpr::METHOD_SELECTOR;
        selExpr->m_methodCallExpr = new MethodCallExpr(node->location);
   
        if (subNode->count() == 2)  // no argument
            return selExpr;
        
        else if (subNode->count() == 3) {
            subNode = subNode->childs[1];
            for (int index = 0; index < subNode->count(); index += 2) {
                Expr* expr = (Expr*)handleExpr(subNode->childs[index]);
                selExpr->m_methodCallExpr->appendArgument(expr);
            }
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    return NULL;
    
}

/// handler for new expression
AST* ASTBuilder::handleArgumentList(Node* node)
{
    ArgumentList* argumentList = new ArgumentList(node->location);
    for (int index = 1; index < node->count() - 1; index++) {
        Expr* expr = (Expr*)handleExpr(node->childs[index]);
        argumentList->appendArgument(expr);
    }
    return argumentList;
}
AST* ASTBuilder::handleNewExpr(Node* node) 
{
    string type = node->childs[1]->childs[0]->assic;
    ArgumentList* list = 
        (ArgumentList*)handleArgumentList(node->childs[2]); 
    NewExpr* newExpr = new NewExpr(type, list, node->location);
    return newExpr; 
}
/// handler for map expression
AST* ASTBuilder::handleMapExpr(Node* node) 
{
    MapExpr* mapExpr = new MapExpr(node->location);
    if (node->count() == 3) {
        Node* itemNode = node->childs[1];
        for (int index = 0; index < itemNode->count(); index += 2) {
            MapItemExpr* item = (MapItemExpr*) handleMapItemExpr(itemNode->childs[index]);
            mapExpr->appendItem(item);
        }
    }
    return mapExpr;
}

/// handler for map expression
AST* ASTBuilder::handleMapItemExpr(Node* node) 
{
    Expr* key = (Expr*) handleExpr(node->childs[0]);
    Expr* val = (Expr*) handleExpr(node->childs[2]);
    return new MapItemExpr(key, val, node->location);
}

/// handler for set expression
AST* ASTBuilder::handleSetExpr(Node* node) 
{
    ExprList* exprList = (ExprList*)handleExprList(node->childs[1]);
    return new SetExpr(exprList, node->location);
}
   

/// handler for map literal
AST* ASTBuilder::handleMapLiteral(Node* node)
{
    return NULL;
}
/// handler for set literal
AST* ASTBuilder::handleSetLiteral(Node* node)
{
    return NULL;
}

