//
//  ASTBuilder.cpp
//  A toyable language compiler (like a simple c++)
//

#include "compiler/Common.h"
#include "compiler/ASTBuilder.h"
#include "compiler/AST.h"
#include "compiler/Type.h"
#include "compiler/Struct.h"
#include "compiler/Variable.h"
#include "compiler/Class.h"
#include "compiler/Statement.h"
#include "compiler/Function.h"
#include "compiler/Expression.h"

ASTBuilder::ASTBuilder() {
}

ASTBuilder::~ASTBuilder() {
    
}

/// @brief ASTBuilder main function to convert a parse tree into an AST tree
AST* ASTBuilder::build(Node *parseTree) {
    assert(parseTree != NULL);
    AST * root = new AST(NULL);
    
    std::vector<Node *>::iterator ite = parseTree->childs.begin();
    while (ite != parseTree->childs.end()) {
        Node *node = *ite;
        AST *child = NULL;
        
        if (node->assic == "importDeclaration")
            child = handleImportDeclaration(node);
        else if (node->assic == "structDeclaration")
            child = handleStructDeclaration(node);
        else if (node->assic == "globalVarDeclaration")
            child = handleGlobalVarDeclaration(node);
        else if (node->assic == "functionDeclaration")
            child = handleFunctionDeclaration(node);
        else if (node->assic == "classDeclaration")
            child = handleClassDeclaration(node);
        else if (node->assic == "interfaceDeclaration")
            child = handleInterfaceDeclaration(node);
        else 
            throw InvalidSyntax(node->assic);
        
        if (!child)
            root->addChild(child);
		ite++;
    }
    return root;
}

/// @brief ASTBuilder handler for import declaration
AST* ASTBuilder::handleImportDeclaration(Node *node) {
    // get all package names
    vector<string> ids;
    for (int i = 0; i < node->count(); i++) {
        string id = node->childs[i]->assic;
        if (id != ".")
            ids.push_back(id);
    }
    // make new ast
    return new ImportStatement(ids);
}

/// @brief ASTBuilder handler for StructDeclaration
AST* ASTBuilder::handleStructDeclaration(Node *node) {
    bool isPublic = false;;
    int index = 0;
    
    if (node->childs[0]->assic == "ScopeSpecifier") {
        if (node->childs[0]->childs[0]->assic == "public")
            isPublic = true;
        index ++;
    }
    string name = node->childs[index + 1]->assic;
    Struct *pst = new Struct(name);
    for (; index < (int)node->childs.size() - 1; index++) {
        // member's type name and id
        string t = node->childs[index]->childs[0]->assic;
        string id = node->childs[index]->childs[1]->assic;
        pst->pushMember(t, id);
    }
    pst->m_isPublic = isPublic;
    // make new ast
    return pst;
}

// varDeclaration
// : storageSpecifier? constSpecifier? typeSpecifier identifier  ('=' expression)? ';'
//;
/// @breif ASTBuilder handler for variable declaration
AST* ASTBuilder::handleVarDeclaration(Node *node) {
    bool isStatic = false;
    bool isConst = false;
    int index = 0;
    int childCount = (int)node->childs.size();
    Expression *expr = NULL;
    
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
    string typeName = node->childs[index++]->childs[0]->assic;
    string id = node->childs[index++]->childs[0]->assic;
    
    // expression initializer
    if ((index + 1) < childCount) {
        // check to see wether have expression initialization list
        expr = (Expression *)handleExpression(node->childs[index]);
    }
    return new Variable(isStatic, isConst, typeName, id, expr);
}

// globalVarDeclaration
//  : scopeSpecifier ? varDeclration
/// @brief ASTBuilder handler for global variable declaration
AST* ASTBuilder::handleGlobalVarDeclaration(Node *node) {
    bool isPublic = false;
    int index = 0;
 
    // check the scope
    if (node->childs[index]->assic == "scopeSpecifier") {
        if (node->childs[index]->childs[0]->assic == "public")
            isPublic = true;
        index++;
    }
    
    Variable *var =  (Variable*)handleVarDeclaration(node->childs[index]);
    var->m_isPublic = isPublic;
    var->m_isGlobal = true;
    var->m_isOfClass = false;
    return var;
}

/// @brief Handler for function declaration
AST* ASTBuilder::handleFunctionDeclaration(Node *node) {
    string signature = "";
    int index = 0;
    
    // function signature
    if (node->childs.size() == 5) {
        signature = node->childs[0]->childs[0]->assic;
        index = 1;
    }
    // return type and function name
    string returnType = node->childs[index]->childs[0]->assic;
    string functionName = node->childs[index + 1]->childs[0]->assic;
    // function parameter list
    FunctionParameterList *funcParameterList = 
        (FunctionParameterList *)handleFunctionParameters(node->childs[index + 2]);    
    // function block
    FunctionBlock *block = 
        (FunctionBlock *)handleFunctionBlock(node->childs[index + 3]);
    // make AST tree
    return new Function(signature, returnType, functionName,funcParameterList, block);
}

/// @brief Handler for function parameter
AST* ASTBuilder::handleFunctionParameters(Node *node) {
    int childCount = node->count();
    FunctionParameterList *paraList = new FunctionParameterList();
    FunctionParameter *parameter = NULL;
    
    if (childCount == 2) // no parameter
        return paraList;
    else if (childCount == 3)  { // with only normal parameter
        parameter = (FunctionParameter*)
        handleFunctionNormalParameter(node->childs[1]);
        paraList->addParameter(parameter);
    }
    else if (childCount == 5) { // with default parameter
        // handle normal parameter
        for (int index = 0; index < node->childs[1]->count(); index++) {
            parameter = (FunctionParameter *)
            handleFunctionNormalParameter(node->childs[1]->childs[index]);
            paraList->addParameter(parameter);
            index++; // skop the token ','
        }
        
        // handle default parameter
        for (int index = 0; index < node->childs[4]->count(); index++) {
            parameter = (FunctionParameter *)
            handleFunctionDefaultParameter(node->childs[4]->childs[index]);
            paraList->addParameter(parameter);
            index++;
        }
    }
    
    return paraList;
}

/// @brief Handler for function normal parameter
AST* ASTBuilder::handleFunctionNormalParameter(Node *node) {
    bool isConst = false;
    int index = 0;
    
    // get const specifier
    if (node->count() == 3) {
        isConst = true;
        index = 1;
    }
    
    // get type name and id
    string type = node->childs[index]->childs[0]->assic;
    string id = node->childs[index + 1]->childs[0]->assic;
    return new FunctionParameter(isConst, type, id, false, NULL);
    
}

/// @brief Handler for function default parameter 
AST* ASTBuilder::handleFunctionDefaultParameter(Node *node) {
    FunctionParameter * para = (FunctionParameter *)
    handleFunctionNormalParameter(node->childs[0]);
    
    if (node->count() == 3)
        para->m_default = (Expression *)handleExpression(node->childs[2]);
    
    return para;
    
}

/// @brief Handler for function block
AST* ASTBuilder::handleFunctionBlock(Node *node) {
    FunctionBlock *block = new FunctionBlock();
    
    for (int index = 1; index < node->count() - 1; index++) {
        Statement *stmt = (Statement *)handleStatement(node->childs[index]);
        block->addStatement(stmt);
    }
    return block;
}

/// @brief Handler fro class 
/// classDeclaration
/// : scopeSpecifier ? 'class' identifer (classInheritDeclaration)?(protoclImplementataion)? classBlock
/// ;
AST* ASTBuilder::handleClassDeclaration(Node *node) {
    int index = 0;
    bool isPublic = false;
    
    // get scope specifier
    if (node->childs[0]->assic == "ScopeSpecifier") {
        if (node->childs[0]->childs[0]->assic == "public")
            isPublic = true;
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
        if (node->childs[index]->childs[0]->assic == "ClassInheritDeclaration") {
            Node *subroot = node->childs[index];
            // get base class
            // 'extend' identifer (',' identifer)*
            for (int childIndex = 1; childIndex < subroot->count(); childIndex++) {
                baseList.push_back(subroot->childs[childIndex]->childs[0]->assic);
                childIndex += 2;
            }
        }
        // 'implement protocol1, protocol2,...
        else if (node->childs[index]->childs[0]->assic = "ProtocolImplementation") {
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

    return new Class(isPublic, id, baseList, protocolList, clsBlock);
    
}

/// @brief ASTBuilder handler for class block
AST* ASTBuilder::handleClassBlock(Node *node) {
    ClassBlock *block = new ClassBlock();
    
    if (node->count() == 2)
        return block;
    
    for (int index = 1; index < node->count() - 1; index++) {
        if (node->childs[index]->childs[0]->assic == "classVarDeclaration") {
            Variable *var = (Variable *)handleClassVariable(node->childs[index]);
            block->addVariable(var);
        }
        else if (node->childs[index]->childs[0]->assic == "classFunctionDeclaration") {
            Function *function = (Function*)handleClassFunction(node->childs[index]);
            block->addFunction(function);
        }
        else {
            throw InvalidSyntax(node->assic);
            break;
        }
    }
    return block;
}

/// @brief ASTBuilder handler for class variable
AST* ASTBuilder::handleClassVariable(Node *node) {
    bool isPublic = false;
    int index = 0;
    
    // check the scope
    if (node->childs[index]->assic == "scopeSpecifier") {
        if (node->childs[index]->childs[0]->assic == "public")
            isPublic = true;
        index++;
    }
    
    Variable *var =  (Variable*)handleVarDeclaration(node->childs[index]);
    var->m_isPublic = isPublic;
    var->m_isGlobal = false;
    var->m_isOfClass = true;
    return var;
}

/// @brief ASTBuilder handler for class function
AST* ASTBuilder::handleClassFunction(Node *node) {
    int index = 0;
    bool isPublic = false;
    bool isConst = false;
    string signature = "";
    
    if (node->childs[index]->assic == "scopeSpecifier") {
        if (node->childs[0]->childs[0]->assic == "public")
            isPublic = true;
        index++;
    }
    
    if (node->childs[index]->assic == "constSpecifier") {
        if (node->childs[index]->childs[0]->assic == "const") 
            isConst = true;
        index++;
    }
    
    if (node->childs[index]->assic == "classFunctionSignature") {
        signature = node->childs[index]->childs[0]->assic;
        index++;
    }
    
    Function *function = (Function *)handleFunctionDeclaration(node->childs[index]);
    function->m_isPublic = isPublic;
    function->m_isConst = isConst;
    if (signature == "static")
        function->m_isStatic = true;
    if (signature == "virtual")
        function->m_isVirtual = true;
    
    return function;
}

/// @brief ASTBuilder handler for interface declaration
AST * ASTBuilder::handleProtocolDeclaration(Node *node) {
    bool isPublic = false;
    int index = 0;
    
    if (node->count() == 4) {
        isPublic = true;
        index++;
    }
    
    string id = node->childs[index + 1]->childs[0]->assic;
    Node *blockNode = node->childs[index + 2]->childs[0];
    Protocol *protocol = new Protocol(id);
    
    for (index = 1; index < blockNode->count() - 1; index++) {
        Node *ifblock = blockNode->childs[0];
        string type = ifblock->childs[0]->assic;
        string name = ifblock->childs[1]->assic;
        FunctionParameterList *paraList = (FunctionParameterList *)
        handleFunctionParameters(ifblock->childs[2]);
        
        Function *function = new Function();
        function->m_name = name;
        function->m_returnType = type;
        function->m_paraList = paraList;
        function->m_isVirtual = true;
        function->m_isPublic = true;
        function->m_isStatic = false;
        function->m_isOfProtocol = true;
        function->m_interface = id;
        protocol->addFunction(function);
    }
    return protocol;
}


//
// Statements
//
/// @brief ASTBuilder handler for statement
AST* ASTBuilder::handleStatement(Node *node) {
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
    else if (type == "functionCallStatement")
        return handleFunctionCallStatement(node->childs[0]);
    else {
        throw InvalidStatement(node->assic);
        return NULL;
    }
}

/// @brief ASTBuilder handler for block statment
AST* ASTBuilder::handleBlockStatement(Node *node) {
    BlockStatement *blockStmt = new BlockStatement();
    for (int index = 1; index < node->count() - 1; index++) {
        Statement * stmt = (Statement *)handleStatement(node->childs[index]);
        blockStmt->addStatement(stmt);
    }
    return blockStmt;
}

/// @brief ASTBuilder handler for variable declaration statement
AST* ASTBuilder::handleVarDeclStatement(Node *node) {
    Variable *var = (Variable*)handleVarDeclaration(node->childs[0]);
    var->m_isGlobal = false;
    var->m_isOfClass = false;
    var->m_isPublic = false;
    
    return new VariableDeclStatement(var, var->m_expr);
}

/// @brief ASTBuidler handler for if statement
AST* ASTBuilder::handleIfStatement(Node *node) {
    Expression *conditExpr = (Expression *)handleExpression(node->childs[2]);
    Statement *stmt1 = (Statement*)handleStatement(node->childs[4]);
    Statement *stmt2 = NULL;
    if (node->count() == 7) {
        stmt2 = (Statement*)handleStatement(node->childs[6]);
    }
    return new IfStatement(conditExpr, stmt1, stmt2);
}

/// @brief ASTBuilder handler for for statement
AST* ASTBuilder::handleForStatement(Node *node) {
    Statement *stmt = (Statement *)handleStatement(node->childs[4]);
    node = node->childs[2];  // for loop parts
    
    if (node->childs[0]->assic == "varDeclaration") {
        string type = node->childs[0]->childs[0]->assic;
        string id = node->childs[1]->childs[0]->assic;
        Expression *expr = (Expression *)handleExpression(node->childs[2]);
        return new ForStatement(type, id, expr, stmt);
    }
    
    
    int index = 0;
    Expression *expr1 = NULL;
    Expression *expr2 = NULL;
    ExpressionList *exprList = NULL;
    
    if (node->childs[0]->assic == "expression") {
        expr1 = (Expression *)handleExpression(node->childs[0]);
        index++;
    }
    expr2 = (Expression *)handleExpression(node->childs[index++]);
    exprList = (ExpressionList *)handleExpressionList(node->childs[index]);
    return new ForStatement(expr1, expr2, exprList, stmt);
}

/// @brief ASTBuilder handler for while statement
AST* ASTBuilder::handleWhiletatement(Node *node) {
    Expression *conditExpr = (Expression *)handleExpression(node->childs[2]);
    Statement *stmt = (Statement *)handleStatement(node->childs[4]);
    return new WhileStatement(conditExpr, stmt);
}

/// @brief ASTBuilder handler for do while statement
AST* ASTBuilder::handleDoStatement(Node *node) {
    Expression *conditExpr = (Expression *)handleExpression(node->childs[2]);
    Statement *stmt = (Statement *)handleStatement(node->childs[4]);
    return new DoStatement(conditExpr, stmt);
}

/// @brief ASTBuilder handler for switch statement
AST* ASTBuilder::handleSwitchStatement(Node *node) {
    Expression *resultExpr = (Expression *)handleExpression(node->childs[2]);
    SwitchStatement *switchStmt  = new SwitchStatement(resultExpr);
    
    for (int index = 5; index < node->count(); index++) {
        if (node->childs[index]->assic == "switchCase") {
            Node *subnode = node->childs[index];
            vector<Expression *> *exprList = new vector<Expression *>();
            for (int subIndex = 0; subIndex < subnode->count() - 1; subIndex += 3) {
                Expression *caseExpr = 
                (Expression *)handleExpression(subnode->childs[subIndex + 1]);
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
            throw InavlidStatement(node->assic);
            break;
        }
    }
    return switchStmt;
}

/// @brief ASTBuilder hander for return statement
AST* ASTBuilder::handleReturnStatement(Node *node) {
    Expression *expr = NULL;
    if (node->count() == 3)
        expr = (Expression *)handleExpression(node->childs[2]);
    return new ReturnStatement(expr);
}
/// @brief ASTBuilder hander for assert statement
AST* ASTBuilder::handleAssertStatement(Node *node) {
    Expression * expr = (Expression *)handleExpression(node->childs[1]);
    return new AssertStatement(expr);
}

/// @brief ASTBuilder hander for break statement
AST* ASTBuilder::handleBreakStatement(Node *node) {
    return new BreakStatement();
}

/// @brief ASTBuilder hander for continue statement
AST* ASTBuilder::handleContinueStatement(Node *node) {
    return new ContinueStatement();
}
/// @brief ASTBuilder hander for throw statement
AST* ASTBuilder::handleThrowStatement(Node *node) {
    Expression *expr = NULL;
    if (node->count() == 3)
        expr = (Expression *)handleExpression(node->childs[1]);
    return new ThrowStatement(expr);
}

/// @brief ASTBuilder hander for try statement
AST* ASTBuilder::handleTryStatement(Node *node) {
    BlockStatement *blockStmt = (BlockStatement *)handleBlockStatement(node->childs[1]);
    TryStatement *tryStmt = new TryStatement(blockStmt);
    
    for (int index = 2; index < node->count(); index ++) {
        if (node->childs[index]->assic == "catchPart") {
            CatchStatement *catchStmt = 
            (CatchStatement *)handleCatchStatement(node->childs[index]);
            tryStmt->addCatchPart(catchStmt);
        }
        else if (node->childs[index]->assic == "finallyPart") {
            FinallyCatchStatement *finallyStmt = 
            (FinallyCatchStatement *)handleFinallyCatchStatement(node->childs[index]);
            tryStmt->setFinallyCatchPart(finallyStmt);
        }
        else {
            throw InavlidStatement(node->assic);
            delete tryStmt;
            tryStmt = NULL;
            break;
        }
    }
    
    return tryStmt;
}

/// @brief ASTBuilder hander for catch statement
AST* ASTBuilder::handleCatchStatement(Node *node) {
    string type = node->childs[1]->childs[0]->assic;
    string id = node->childs[2]->childs[0]->assic;
    BlockStatement *blockStmt = (BlockStatement *)handleBlockStatement(node->childs[5]);
    
    return new CatchStatement(type, id, blockStmt);
}

/// @brief ASTBuilder hander for finally catch statement
AST* ASTBuilder::handleFinallyCatchStatement(Node *node) {
    BlockStatement *blockStmt = (BlockStatement *)handleBlockStatement(node->childs[1]);
    return new FinallyCatchStatement(blockStmt);
}

/// @brief ASTBuilder hander for expression statement
AST* ASTBuilder::handleExpreStatement(Node *node) {
    return NULL;
}

/// @brief ASTBuilder hander for funcation statement
AST* ASTBuilder::handleFunctionCallStatement(Node *node) {
    return NULL;
}

/// @brief ASTBuilder handler for expression list
AST* ASTBuilder::handleExpressionList(Node *node) {
    ExpressionList *exprList = new ExpressionList();
    Expression *expr = NULL;
    
    for (int index = 0; index < node->count(); index++) {
        expr = (Expression *)handleExpression(node->childs[index]);
        exprList->appendExpression(expr);
        index++;
    }
    return exprList;
}

/// @brief ASTBuilder handler for expression
AST* ASTBuilder::handleExpression(Node *node) {
    if (node->count() == 1)
        return handleConditionalExpression(node);
    
    Expression *leftExpr = (Expression *)handleAssignableExpression(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    Expression *rightExpr = (Expression *)handleExpression(node->childs[2]);
    
    return new BinaryOpExpression(op, leftExpr, rightExpr);
}

/// @brief ASTBuilder handler for assignalbe expression
AST* ASTBuilder::handleAssignableExpression(Node *node) {
    return NULL;
}

/// @brief ASTBuilder handler for conditional expression
AST* ASTBuilder::handleConditionalExpression(Node *node) {
    return handleLogicOrExpression(node);
}

/// @brief ASTBuilder handler for logic or expression
AST* ASTBuilder::handleLogicOrExpression(Node *node) {
    if (node->count() == 1)
        return handleLogicAndExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleLogicAndExpression(node->childs[0]);
    LogicOrExpression *logicOrExpre = new LogicOrExpression(leftExpr);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expression *target = (Expression *)handleLogicAndExpression(node->childs[index]);
        logicOrExpre->appendElement(target);
    }
    return logicOrExpre;
}

/// @brief ASTBuilder handler for logic and expression
AST* ASTBuilder::handleLogicAndExpression(Node *node) {
    if (node->count() == 1)
        return handleBitwiseOrExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleBitwiseOrExpression(node->childs[0]);
    LogicAndExpression *logicAndExpr = new LogicAndExpression(leftExpr);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expression *target = (Expression *)handleBitwiseAndExpression(node->childs[index]);
        logicAndExpr->appendElement(target);
    }
    return logicAndExpr;
}

/// @brief ASTBuilder handler  for bitwise or expression
AST* ASTBuilder::handleBitwiseOrExpression(Node *node) {
    if (node->count() == 1)
        return handleBitwiseXorExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleBitwiseXorExpression(node->childs[0]);
    BitwiseOrExpression *expr = new BitwiseOrExpression(leftExpr);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expression *target = (Expression *)handleBitwiseXorExpression(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}


/// @brief ASTBuilder handler for bitwise xor expression
AST* ASTBuilder::handleBitwiseXorExpression(Node *node) {
    if (node->count() == 1)
        return handleBitwiseAndExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleBitwiseAndExpression(node->childs[0]);
    BitwiseAndExpression *expr = new BitwiseAndExpression(leftExpr);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expression *target = (Expression *)handleBitwiseAndExpression(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// @brief ASTBuilder handler for bitwise and expression
AST* ASTBuilder::handleBitwiseAndExpression(Node *node) {
    if (node->count() == 1)
        return handleEqualityExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleEqualityExpression(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    BitwiseAndExpression *expr = new BitwiseAndExpression(leftExpr);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expression *target = (Expression *)handleEqualityExpression(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// @brief ASTBuilder handler for equality expression
AST* ASTBuilder::handleEqualityExpression(Node *node) {
    if (node->count() == 1)
        return handleRelationalExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleRelationalExpression(node->childs[0]);
    EqualityExpression *expr = new EqualityExpression(leftExpr);
    
    for (int index = 1; index < node->count(); index++) {
        Expression *target = (Expression *)handleBitwiseAndExpression(node->childs[index]);
        
        int op = -1;
        if (node->childs[index]->assic == "==")
            op = EqualityExpression::OP_EQ;
        else if (node->childs[index]->assic == "!=")
            op = EqualityExpression::OP_NEQ;
        else 
            throw InvalidStatement(node->assic);
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for relational expression
AST* ASTBuilder::handleRelationalExpression(Node *node) {
    if (node->count() == 1)
        return handleShiftExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleShiftExpression(node->childs[0]);
    RelationalExpression *expr = new RelationalExpression(leftExpr);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expression *target = (Expression *)handleShiftExpression(node->childs[index]);
        
        int op = -1;
        if (node->childs[index]->assic == ">")
            op = RelationalExpression::OP_GT;
        else if (node->childs[index]->assic == "<")
            op = RelationalExpression::OP_LT;
        else if (node->childs[index]->assic == ">=")
            op = RelationalExpression::OP_GTEQ;
        else if (node->childs[index]->assic == "<=")
            op = RelationalExpression::OP_LTEQ;
        else 
            throw InvalidStatement(node->assic);        
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for shift expression
AST* ASTBuilder::handleShiftExpression(Node *node) {
    if (node->count() == 1)
        return handleAdditiveExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleAdditiveExpression(node->childs[0]);
    ShiftExpression *expr = new ShiftExpression(leftExpr);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expression *target = (Expression *)handleAdditiveExpression(node->childs[index]);
        int op = -1;
        if (node->childs[index]->assic == ">>")
            op = ShiftExpression::OP_RSHIFT;
        else if (node->childs[index]->assic == "<<")
            op = ShiftExpression::OP_LSHIFT;
        else 
            throw InvalidStatement(node->assic);
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for additive expression
AST* ASTBuilder::handleAdditiveExpression(Node *node) {
    if (node->count() == 1)
        return handleMultiplicativeExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleMultiplicativeExpression(node->childs[0]);
    AdditiveExpression *expr = new AdditiveExpression(leftExpr);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expression *target = 
        (Expression *)handleMultiplicativeExpression(node->childs[index]);
        int op = -1;
        if (node->childs[index]->assic == "+")
            op = AdditiveExpression::OP_PLUS;
        else if (node->childs[index]->assic == "-")
            op = AdditiveExpression::OP_SUB;
        else 
            throw InvalidStatement(node->assic);
          
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder handler for multiplicative expression
AST* ASTBuilder::handleMultiplicativeExpression(Node *node) {
    if (node->count() == 1)
        return handleUnaryExpression(node->childs[0]);
    
    Expression *leftExpr = (Expression *)handleUnaryExpression(node->childs[0]);
    AdditiveExpression *expr = new AdditiveExpression(leftExpr);
    
    for (int index = 1; index < node->count(); index++) {
        
        Expression *target = 
        (Expression *)handleUnaryExpression(node->childs[index]);
        int op = -1;
        if (node->childs[index]->assic == "*")
            op = MultiplicativeExpression::OP_MUL;
        else if (node->childs[index]->assic == "/")
            op = MultiplicativeExpression::OP_DIV;
        else if (node->childs[index]->assic == "%")
            op = MultiplicativeExpression::OP_MODULO;    
        else 
            throw InvalidStatement(node->assic);
        
        expr->appendElement(op, target);
    }
    return expr;
}

/// @brief ASTBuilder for unary expression
AST* ASTBuilder::handleUnaryExpression(Node *node) {
    Expression *expr = (Expression *)handlePrimary(node->childs[0]);
    if (node->count() == 1)
        return expr;
    
    UnaryExpression *unaryExpr = new UnaryExpression(expr);  
    for (int index = 1; index < node->count(); index++) {
        SelectorExpression *sel = (SelectorExpression *)handleSelector(node->childs[index]);
        unaryExpr->appendElement(sel);
    }
    
    return unaryExpr;
}

/// @brief ASTBuilder handler for primary expression
AST* ASTBuilder::handlePrimary(Node *node) {
    string text = node->childs[0]->assic;
    
    if (text == "this")
        return new PrimaryExpression(PrimaryExpression::T_THIS);
    
    if (text == "super")
        return new PrimaryExpression(PrimaryExpression::T_SUPER);
    
    if (text == "true")
        return new PrimaryExpression(PrimaryExpression::T_TRUE);
    
    if (text == "false")
        return new PrimaryExpression(PrimaryExpression::T_FALSE);
    
    if (text == "null")
        return new PrimaryExpression(PrimaryExpression::T_NULL);
    
    if (text == "mapLiteral")
        return handleMapExpression(node->childs[0]);
    
    if (text == "setLiteral")
        return handleSetExpression(node->childs[0]);
    
    if (text == "identifer")
        return new PrimaryExpression(PrimaryExpression::T_IDENTIFIER, node->childs[0]->assic);
    
    if (text == "NUMBER")
        return new PrimaryExpression(PrimaryExpression::T_NUMBER, node->childs[0]->assic);
    
    if (text == "HEX_NUMBER")
        return new PrimaryExpression(PrimaryExpression::T_NUMBER, node->childs[0]->assic);
    
    if (node->count() == 3) // compound expression
        return new PrimaryExpression(PrimaryExpression::T_COMPOUND, node->childs[1]);
      
    throw InavlidExpression(text);
    return NULL;
    
}

/// @brief ASTBuilder handler for selector
AST* ASTBuilder::handleSelector(Node *node) {
    if (node->childs[0]->assic == "assignalbeSelector") {
        Node *subNode = node->childs[0];
        if (subNode->count() == 2) // .identifier
            return new SelectorExpression(subNode->childs[1]->assic);
        
        else if (subNode->count() == 3) { // [ expression ]
            Expression *expr = (Expression *)handleExpression(subNode->childs[1]);
            return new SelectorExpression(expr);
        }
        else
            throw InavlidExpression(node->childs[0]->assic);
        
    }
    else if (node->childs[0]->assic == "arguments") {
        Node *subNode = node->childs[0];
        SelectorExpression *selExpr = new SelectorExpression();
        if (subNode->count() == 2)  // no argument
            return selExpr;
        
        else if (subNode->count() == 3) {
            subNode = subNode->childs[1];
            for (int index = 0; index < subNode->count(); index += 2) {
                Expression *expr = (Expression *)handleExpression(subNode->childs[index]);
                selExpr->appendArgument(expr);
            }
            return selExpr;
        }
        else
            throw InavlidExpression(node->childs[0]->assic);
    }
    else 
        throw InavlidExpression(node->childs[0]->assic);
        
    return NULL;
    
}

/// @brief ASTBuilder handler for new expression
AST* ASTBuilder::handleNewExpression(Node *node) {
    string type;
    // the current node is pointed to  arguments
    
    if (node->count() == 1) { // no parameter, just new id;
        type = node->childs[0]->childs[0]->assic;
        return new NewExpression(type);
    }
    else if (node->count() == 2) { // with arguments, new id arguments
        // new id arguments
        type = node->childs[0]->childs[0]->assic;
        NewExpression *newExpr = new NewExpression(type);
        
        if (node->childs[1]->count() == 3) {
            node = node->childs[1]->childs[1];
            for (int index = 0; index < node->childs[1]->count(); index += 2) {
                Expression *expr = (Expression *)handleExpression(node->childs[1]->childs[index]);
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
AST* ASTBuilder::handleMapExpression(Node *node) {
    MapExpression *mapExpr = new MapExpression();
    if (node->count() == 3) {
        Node *itemNode = node->childs[1];
        for (int index = 0; index < itemNode->count(); index += 2) {
            MapLitemExpression *item = handleMapItemExpression(itemNode->childs[index]);
            mapExpr->appendItem(item);
        }
    }
    return mapExpr;
}

/// @brief ASTBuilder handler for map expression
AST* ASTBuilder::handleMapItemExpression(Node *node) {
    Expression *key = handleExpression(node->childs[0]);
    Expression *val = handleExpression(node->childs[2]);
    return new MapItemExpression(key, val);
}

/// @brief ASTBuilder handler for set expression
AST* ASTBuilder::handleSetExpression(Node *node) {
    ExpressionList *exprList = handleExpressionList(node->childs[1]);
    return new SetExpression(exprList);
}
   





