//
//  CodeBuilder.cpp
//  A toyable language compiler (like a simple c++)


#include "compiler/AST.h"
#include "compiler/Scope.h"
#include "compiler/Error.h"
#include "compiler/Label.h"
#include "compiler/Value.h"
#include "compiler/Frame.h"
#include "compiler/Struct.h"
#include "compiler/Variable.h"
#include "compiler/Class.h"
#include "compiler/Method.h"
#include "compiler/Expr.h"
#include "compiler/Statement.h"
#include "compiler/IREmiter.h"
#include "compiler/IRBuilder.h"

IRBuilder::IRBuilder()
{
    m_blocks = NULL;
}
IRBuilder::~IRBuilder()
{
}
/// @brief Enter a new scope
void IRBuilder::enterScope(const string &name, Scope *scope) {
    if (m_curScope) {
        scope->setParentScope(m_curScope);
        m_curScope = scope;
    } 
    m_curScopeName = name;
    if (!m_rootScope)
        m_rootScope = newScope;
}
/// @brief Exit the current scope
void IRBuilder::exitScope() {
    if (m_curScope != NULL)
        m_curScope = m_curScope->getParent();
}

/// @brief Check to see wether the symbol specified by name exist
bool IRBuilder::hasSymbol(const string &name, bool nested) {
    bool result = false;
    if (m_curScope && m_curScope->resolveSymbol(name, nested))
        result = true;

    return result;
}

/// @brief Check to see wether the type specified by name exist
bool IRBuilder::hasType(const string &name, bool nested) {
    bool result = false;
    if (m_curScope && m_curScope->resolveType(name, nested))
        result = true;
    
    return result;
    
}

/// @brief Get symbol by name 
Symbol* IRBBuilder::getSymbol(const string &name, bool nested) {
    Symbol *symbol = NULL;
    if (m_curScope!= NULL)
        symbol = m_curScope->resolveSymbol(name, nested);
    
    return symbol;
}

/// @brief Get type by name
Type* IRBBuilder::getType(const string &name, bool nested) {
    Type *type = NULL;
    if (m_curScope != NULL)
        type = m_curScope->resolveType(name, nested);
    
    return type;
}

/// @brief Define a new symbo in current scope
void IRBBuilder::defineSymbol(Symbol *symbol) {
    if (symbol && m_curScope) {
        m_curScope->defineSymbol(symbol);
    }
}

/// @brief Define a new type in current scope
void IRBBuilder::defineType(Type *type){
    if (type && m_curScope)
        m_curScope->defineType(type);
}


/// @brief Allocate locals from stack
Value* IRBuilder::allocValueInFrame(int size) {
    // get current frame
    Frame *frame = FrameStack::getCurrentFrame();
    return frame->allocValue(size, true);
}

Value* IRBuilder::allocValue(Type *type, bool inreg) {
    Value *local = NULL;
    int size = 0;
    
    if (type && !(size = type->getSize()))
        return NULL;
    
    if (!inreg) {
        local = allocValueInFrame(size);
        local->initializeWithType(type);
    }
    else {
        // alloc locals from register window
        local = RegisterWindow::allocValue(type);
    }
    return local;
    
}

int  IRBuilder::getLinkAddress(Method &func) {
    return 0; // for dummy now
}

void IRBuilder::build(AST *ast) {
    if (ast)
        ast->walk(this);
}

// typespecifier
void IRBuilder::accept(TypeSpec &type) {
}

// struct
void IRBuilder::accept(Struct &type) {
}

/// @brief IRBuidler handler for Variable
void IRBuilder::accept(Variable &var) {
    // if the variable is global, the variable should be added into globaa memory
    // according to wether it is initialized, the destional region is different.
    if (var.m_isGlobal) {
        // push all global variable into one regin
        m_globalVars.push_back(&var);
    }
    
    // if the variable is class variable
    else if (var.m_isOfClass) {
    }
    // if the variable is method local variable
    // reserve the memory from the current frame and initialize
    else {
        Value *local = allocValue(type);
        if (var.m_expr) {
            build(var.m_expr);
            Value *result = &var.m_expr->m_value;
            IREmiter::emitLoad(local, result);
        }
    }
}

/// @brief  Generate method name's specification
// methodName@class@retType@parametersCount@para1Type@para2Type...
void IRBuilder::makeMethodName(Method &method, string &name) {
    name = method.m_name;
    name += "@";
    if (method.m_isOfClass)
        name += method.m_class;
    else if (method.m_isOfInterface)
        name += method.m_interface;
    
    name += "@";
    name += method.m_returnType;
    name += "@";
    if (method.hasParamter()) {
        name += method.getParameterCount();
        for (int index = 0; index < method.getParameterCount(); index++) {
            MethodParameter *parameter = method.getParameter(index);
            name += "@";
            name += parameter->m_name;
        }
    }
}

/// @brief Method generator
void IRBuilder::generateMethod(Method &method) {
    MethodType *funcType = (MethodType *)getType(method.m_name);

    // make specified method name according to method name and parameter type
    string functName;
    makeMethodName(method, functName);
    
    // mark funciton lable using the name
    Label label(functName);
    IREmiter::emitLabel(label);
    
    // get method regin information
    int linkAddr = getLinkAddress(method);
    
    
    // update the information into methodType
    funcType->setLinkAddress(linkAddr);
    
    
    if (method.m_paraList)
        build(method.m_paraList);
    
    // get all locals and reserve memory for them
    int size = method.getValuesSize();
    Frame *frame = FrameStack::allocNewFrame(size);
    FrameStack::push(frame);
        
    build(m_block);
    
    FrameStack::pop();
    IREmiter::emit(IR_RET);
}

/// @brief Handler for Method IRBuilder
void IRBuilder::accept(Method &method) {
    // enter the method scope
    enterScope(method.getName(), dynamic_cast<Scope*>(&method));
    if (funciton.m_isOfClass) {}
        // generate the code
        generateMethod(method);
    }
    
    // if the method is memeber of interface
    // the method must be in VTBL of the interface
    // do not generate instructions for the virtual funciton
    else if (method.m_isOfProtocol) {
    }
    
    // the method is not class/interface's method
    // in this case, generate code directly,
    else {
        generateMethod(method);
    }
    exitScope();
    
}

/// @brief Handler for MethodParameterList IRBuilder
void IRBuilder::accept(MethodParameterList &list) {
    int index = 0;
    Method* func = (Method*)list.getParent();
    assert(func != NULL);
    
    // if  the method is member of class, the class instance ref musb be added
    if (func->m_isOfClass) {
        Symbol *symbol = new Symbol();
        symbol->m_name = "this";
        symbol->m_type = getType(func->m_class);
        symbol->m_storage = LocalStackSymbol;
        symbo->m_addr = index++;      
    }
    // iterate all parameters fro right to left
    vector<MethodParameter*>::iterator ite = list.m_parameters.end();
    for (ite != list.m_parameters.begin(); ite--) {
        MethodParameter *parameter = *ite;
        parameter->m_method = func;
        parameter->m_index = index++;
        parameter->walk(this);
    }
}

/// @brief Handler for MethodParameter IRBuilder
void IRBuilder::accept(MethodParameter &para) {
    Method *func = para.m_method;
}

/// @brief Handler for MethodBlock IRBuilder
void IRBuilder::accept(MethodBlock &block) {
    // method block's prelog
    // adjust statc frame pointer according to all local's size
    Method* func = (Method*)block.getParent();
  
    // method block's body
    vector<Statement*>::iterator ite = block.m_stmts.begin();
    while (ite != block.m_stmts.end()) {
        Statement *stmt = *ite;
        build(stmt);
        ite++;
    }
}


/// @brief IRBuilder handler for Class
void IRBuilder::accep(Class &cls) {
    build(cls.m_block);
}
/// @brief IRBuilder handler for ClassBlock
void IRBuilder::accept(ClassBlock &block) {
    for_each(block.m_vars.begin(), block.m_vars.end(), build);
    for_each(block.m_methods.begin(), block.m_methods.end(), build);
}

/// @brief IRBuilder handler for Interface
void IRBuilder::accept(Interface &interface) {
    // check to see wether the interface is defined in current scope
    
    // make a vtble for the interface
    
    // place the vtable into global vtable list
    
    
}

/// @brief IRBuilder handler for statement
void IRBuilder::accept(Statement &stmt) {
    // Do nothing, this is base class for all statement
}

/// @brief IRBuilder handler for import statement
void IRBuilder::accept(ImportStatement &stmt) {
    
}

/// @brief IRBuilder handler for block statement
void IRBuilder::accept(BlockStatement &stmt) {
    /// Crate a new Block and insert it into blockList;
    IRBlock *block = new IRBlock();
    m_blocks.push_back(block);
    
    /// Iterate all statement and generate intermeidate instructions
    vector<Statement*>::iterator ite = stmt.m_statements.begin();
    for (; ite != stmt.m_statements.end(); ite++) {
        build(*ite);
    }
    
}

/// @brief IRBuilder handler for variable decl statement
void IRBuilder::accept(VariableDeclStatement &stmt) {
    // the variable must be a local variable
    // make a local in current frame
    Type *type = getType(stmt.m_var->m_type);
    Value *value1 = allocValue(type);
    Value *result = NULL;
    
    // if the declared variable is initialized
    if (stmt.m_expr) {
        build(stmt.m_expr);
        result = &stmt.m_expr->m_value;
    }
    
    // load the result into locals
    IREmiter::emitLoad(value1, result);
}

/// @brief IRBuilder handler for if statement
void IRBuilder::accept(IfStatement &stmt) {
    ASSERT(stmt.m_conditExpr != NULL);
    
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    
    // check wether there is only a if block
    if (stmt.m_elseBlockStmt == NULL) {
        build(stmt.m_conditExpr);
        if (!stmt.m_conditExpr->hasValidValue())
            return;
        Value *value1 = allocValue(true);
        IREmiter::emitStore(value1, stmt.m_conditExpr->m_value);
        IREmiter::cmitCMP(value1, 1, label1, label2);
        IREmiter::emitLabel(label1);
        build(stmt.m_ifBlockStmt);
        delete value1;
    }
    
    else {
        build(stmt.m_conditExpr);
        if (!stmt.m_conditExpr->hasValidValue())
            return;
        Label label3 = Label::newLabel();
        Value *value1 = allocValue(true);
        
        IREmiter::emitStore(value1, stmt.m_conditExpr->m_value);
        IREmiter::cmitCMP(value1, 1, label1, label3);
        IREmiter::emitLabel(label1);
        build(stmt.m_ifBlockStmt);
        
        IREmiter::emitLabel(label3);
        build(stmt.m_elseBlockStmt);
        delete value1;
    }
    
    IREmiter::emitLabel(label2);
}

/// @brief IRBuilder handler for while statement
void IRBuilder::accept(WhileStatement &stmt) {   
    ASSERT(stmt.m_conditExpr != NULL);
    
    // push iterable statement into frame
    Frame *frame = FrameStack::getCurrentFrame();
    frame->pushIterablePoint(&stmt);
    
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel(); 
    Label label3 = Label::newLabel();
    
    IREmiter::emitLabel(label1);
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);
    
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitStore(value1, stmt.m_conditExpr->m_value);
    IREmiter::emitCMP(value1, 1, label2, label3);
    delete value1;
    IREmiter::emitLabel(label2);
    build(stmt.m_stmt);
    IREmiter::emitJump(label1);
    IREmiter::emitLabel(label3);
    
    // popup the current iterable statement
    frame->popIterablePoint();
}

/// @brief IRBuilder handler for do while statement
void IRBuilder::accept(DoStatement &stmt) {
    ASSERT(stmt.m_conditExpr != NULL);
    
    // push iterable statement into frame
    Frame *frame = FrameStack::getCurrentFrame();
    frame->pushIterablePoint(&stmt);

    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();   
    
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label2);
    
    IREmiter::emitLabel(label1);
    build(stmt.m_stmt);
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitStore(value1, stmt.m_conditExpr->m_value);
    IREmiter::emitCMP(value1, 1, label1, label2);
    IREmiter::emitLabel(label2);
    delete value1;
    // popup the current iterable statement
    frame->popIterablePoint();
}

/// @brief IRBuilder handler for for statement
void IRBuilder::accept(ForStatement &stmt) {
    // now only support normal loop mode
    // for (expr1; expr2; exprlist) statements
    if (stmt.m_expr1)
        build(stmt.m_expr1);
    // push iterable statement into frame
    Frame *frame = FrameStack::getCurrentFrame();
    frame->pushIterableStatement(&stmt);
             
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();

    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);

    IREmiter::emitLabel(label1);
        
    if (stmt.m_expr2) {
        build(stmt.m_expr2);
        Value *value2 = allocValue();
        IREmiter::emitStore(value2, stmt.m_expr->m_value);
        IREmiter::emitCMP(value2, 1, labe2, label3);
        delete value2;
    }
    IREmiter::emitLable(label2);
    build(stmt.m_stmt);
    
    if (stmt.m_exprList) 
        build(stmt.m_exprList);
    IREmiter::emit(IR_JUMP, label1);
    IREmiter::emitLabel(labe3);
    // popup the current iterable point
    frame->popIterablePoint();
    IREmiter::emitLabel(stmt.m_nextLabel);
}

void IRBuilder::accept(ForEachStatement &stmt) {
    
}

/// @brief IRBuilder handler for switch statement
void IRBuilder::accept(SwitchStatement &stmt) {
    // check parameter's validity
    ASSERT(stmt.m_conditExpr != NULL);
    
// condition judge statement
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitStore(value1, stmt.m_conditExpr->m_value);
    Label label2 = Label::newLabel();
    
    // generate case statement
    vector< std::pair<vector<Expr*>, Statement *> >::iterator ite = stmt.m_cases.begin();
    for (; ite != stmt.m_cases.end(); ite++)  {
    //
        std::pair<vector<Expr *>, Statement*>  &pt = *ite;
        std::vector<Expr *> exprList = pt.first;
        Statement *sts = pt.second;
        
        // case n
        // case n+1
        Label label1 = Label::newLabel();
        std::vector<Expr *>::iterator ite = exprList.begin();
        for (; ite != exprList.end(); ite++) {
            // get case value
            Expr * expr = *ite;
            build(expr);
            if (!expr->hasValidValue())
                continue;
            Value *value2 = allocValue(true);
            IREmiter::emitStore(value2, expr->m_value);
            IREmiter::emitCMP(value2, value1, label1, label2);
            delete value2;
        }
        
        IREmiter::emitLabel(label1);
        // build statement
        build(sts);
        IREmiter::emit(IR_JUMP, label2);
    }
    
    if (stmt.m_defaultStmt)
        build(stmt.m_defaultStmt);
    
    IREmiter::emitLabel(label2);
    delete value1;
}

/// @brief IRBuilder handler for continue statement
void IRBuilder::accept(ContinueStatement &stmt) {
    Statement *stmt = FrameStack::getCurrentFrame()->getIterablePoint();
    if (stmt && stmt->isIterable())
        IREmiter::emit(IR_JUMP, stmt.getIterableStartPoint());
}

/// @brief IRBuilder handler for break statement
void IRBuilder::accept(BreakStatement &stmt) {
    Statement *stmt = FrameStack::getCurrentFrame()->getIterablePoint();
    if (stmt && stmt->isIterable())
        IREmiter::emit(IR_JUMP, stmt.getIterableEndPoint());
}


/// @brief IRBuilder handler for return statement
void IRBuilder::accept(ReturnStatement &stmt) {
    if (stmt.m_resultExpr) {
        build(stmt.m_resultExpr);
        if (stmt.m_resultExpr->hasValidValue()) {
            // store the result
            Value *local = allocValue(true);
            IREmiter::emitLoad(local, stmt.m_valueExpr->m_value);
            // the return value local will be release in caller
            FrameStack::getCurrentFrame()->setReturnValue(local);
    }
    IREmiter::emit(IR_RET);
}

/// @brief IRBuilder handler for assert statement
void IRBuilder::accept(AssertStatement &stmt) {
    ASSERT(stmt.m_resultExpr != NULL);
    
    build(stmt.m_valueExpr);
    if (!stmt.m_valueExpr->hasValidValue())
        return;
    
    // judge the expression 
    Value *local = allocValue(true);
    IREmiter::emitStore(local, stmt.m_valueExpr->m_value);
    
    // generate the jump labe
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    IREmiter::emitCMP(local, 1, label1, label2);
    
    IREmiter::emitLabel(label2);
    IREmiter::emitException();
    IREmiter::emitLabel(label1);
    delete local;
}

/// @brief IRBuilder handler for throw statement
/// the exception handler should be implement by method type
/// throw 'expression'
void IRBuilder::accept(ThrowStatement &stmt) {
    
    
}

/// @brief IRBuilder handler for try statement
/// 'try' blockStatement ((catchPart+finallyPart)?|finallyPart)
void IRBuilder::accept(TryStatement &stmt) {
    
}
/// @brief IRBuilder handler for catch statement
void IRBuilder::accept(CatchStatement &stmt) {
    
}

/// @brief IRBuilder handler for finally catch satement
void IRBuilder::accept(FinallyCatchStatement &stmt) {
    
}

// expression
void IRBuilder::accept(Expr &expr)
{
}
void IRBuilder::accept(ExprList &exprList)
{
    vector<Expr *>::iterator ite = exprList.m_exprs.begin();
    for (; ite != exprList.m_exprs.end(); ite++) 
    {
        Expr *expr = *ite;
        build(expr);
    }
    
}
/// @brief IRBuilder handler for BinaryOpExpr
void IRBuilder::accept(BinaryOpExpr &expr) {
    ASSERT(expr.m_left != NULL);
    ASSERT(expr.m_right != NULL);
    
    build(expr.m_left);
    if (!expr.m_left->hasValidValue())
        return;
    Value *left = allocValue(true);
    IREmiter::emitLoad(left, expr.m_left->m_value);
    
    build(expr.m_right);
    if (e!expr.m_left->hasValieValue()) {
        delete left;
        return;
    }
    Value *right = allocValue(true);
    IREmiter::emitLoad(right, expr.m_right->m_value);
    
    // convert the binary operatiorn
    int op = IR_INVALID;
    switch (expr.m_op) {
        case BinaryOpExpr::BOP_ADD:
            op = IR_ADD;
            break;
        case BinaryOpExpr::BOP_SUB:
            op = IR_SUB;
            break;
        case BinaryOpExpr::BOP_MUL:
            op = IR_MUL;
            break;
        case BinaryOpExpr::BOP_DIV:
            op = IR_DIV;
            break;
            
        default:
        // others operation will be added here
        break;
    }
    
    if (op != IR_INVALID) 
        IREmiter::emitBinOP(op, left, right, left);
    expr.m_value = *left;
    delete left;
    delete right;
}

/// @brief IRBuilder handler for ConditionalExpr
void IRBuilder::accept(ConditionalExpr &expr) {

}

/// @brief IRBuilder handler for LogicOrExpr
void IRBuilder::accept(LogicOrExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value *local = allocValue(true);
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    IREmiter::emitCMP(local, 1, label1, label2);
    IREmiter::emitLable(labe2);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value *value2 = allocValue(true);
        IREmiter::emitLoad(value2, element->m_value);
        IREmiter::emitBinOP(IR_OR, value1, loca2, value1);
        delete value2;
        
        Label label3 = Label::newLabel();
        IREmiter::emitCMP(value1, 1, nextLable, label3);
        IREmiter::emitLable(label3);
    }
    
    // store the result into expr.result
    expr.m_value = *value1;
    IREmiter::emitLable(label1);
    delete value1;
}

/// @brief IRBilder handler for LogicAndExpr
void IRBuilder::accept(LogicAndExpr &expr) {
    ASSERT(expr.m_target != NULL);
   
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Value *value1 = allocValue(true);
    
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    IREmiter::emitCMP(local, 1, label1, label2);
    IREmiter::emitLabel(label2);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value *value2 = allocValue(true);
        Label label3 = Label::newLabel();
        IREmiter::emitLoad(value2, element->m_value);
        IREmiter::emitBinOP(IR_AND, value1, value2, value1);
        delete value2;
        
        IREmiter::emitCMP(value1, 1, label3, label1);
        IREmiter::emitLabel(label3);
    }
    
    // store the result into expr.result
    expr.m_value = *value1;
    IREmiter::emitLable(label1);
    delete value1;
}


/// @brief IRBilder handler for bitwise or expression
// BitwiseOrExpr : BitwiseXorExpr ( '|' bitwiseXorExpr)*
void IRBuilder::accept(BitwiseOrExpr &expr) {
    ASSERT(expr.m_target != NULL);
   
    build(expr.m_target);
    if (!expr.m_target->hasValieValue())
        return;
    
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, &expr.m_target->m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value *value2 = allocal(true);
        IREmiter::emitLoad(value2, element->m_value);
        IREmiter::emitBinOP(IR_BIT_OR, value1, value2, value1);
        delete value2;
    }
    
    expr.m_value = *value1; 
    delete value1;
}

/// @brief IRBuilder handler for bitwise xor expression
/// BitwiseXorExpr : BitwiseAndExpr ('^' bitwiseAndExpr)*
void IRBuilder::accept(BitwiseXorExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        Value *value2 = allocValue(true);
        IREmiter::emitStore(value2, element->m_value);
        IREmiter::emitBinOP(IR_BIT_XOR, value1, value2, value1);
        delete value2;
    }
    expr.m_value = *value1;
    delete value1;
}

/// @brief IRBuilder handler for bitwise and expression
/// BitwiseAndExpr : EqualityExpr ('&' EqualilityExpr)*
void IRBuilder::accept(BitwiseAndExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        continue;
    
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value *value2 = allocValue(true);
        IREmiter::emitStore(value2, element->m_value);
        IREmiter::emitBinOP(IR_BIT_AND, value1, value2, value1);
        delete value2;
    }
    expr.m_value = *value1;
    delete value1;
}

/// @brief IRBuilder handler for equality Expr
/// EqualityExpr : RelationalExpr (('==' | '!=') RelationalExpr)*
void IRBuilder::accept(EqualityExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValud())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == EqualityExpr::OP_EQ)
            irt = IR_EQ;
        else if (expr.m_op == EqualityExpr::OP_NEQ)
            irt = IR_NEQ;
        
        if (irt != IR_INVALID) { 
            Value *value2 = allocValue(true);
            IREmiter::emitStore(value2, element->m_value);
            IREmiter::emitBinOP(irt, value1, value2, value1);
            delete value2; 
        }
    }
    
    expr.m_value = *value1;
    delete value1;
}

/// @brief IRBuilder handler ffor relational expression
/// RelationalExpr :
///  ShiftExpr (('>' | '<' | '>=' | '<=') ShiftExpr)*
void IRBuilder::accept(RelationalExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    // build expression and get result into local
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!elment->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        switch (expr.m_op) {
            case  RelationalExpr::OP_LT:
                irt = IR_LT;
                break;
            case RelationalExpr::OP_LTEQ:
                irt = IR_LTEQ;
                break;
            case RelationalExpr::OP_GT:
                irt = IR_GT;
                break;
            case RelationalExpr::OP_GTEQ:
                irt = IR_GTEQ;
                break;
            default:
                irt = IR_INVALID;
                break;
        }
        
        if (irt != IR_INVALID) {
            Value *value2 = allocValue(true);
            IREmiter::emitStore(value2, element->m_value);
            IREmiter::emitBinOP(irt, value1, value2, value1);
            delete value2;
        }
    }
    expr.m_value = *value1;
    delete value1;
}

/// @brief IRBuilder handler for shift expression
/// ShiftExpr : AdditiveExpr (('>>'|'<<') AdditiveExpr)*
void IRBuilder::accept(ShiftExpr &expr) {
    ASSERT(expr.m_target != NULL);

    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitStore(value1, expr.m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == ShiftExpr::OP_LSHIFT)
            irt = IR_LSHIFT;
        else if (expr.m_op == ShiftExpr::OP_RSHIFT)
            irt = IR_RSHIFT;
        
        if (irt != IR_INVALID) {
            Value *value2 = allocValue(true);
            IREmiter::emitStore(value2, element->m_value);
            IREmiter::emitBinOP(irt, value1, value2, value1);
            delete value2;
        }
    }
    expr.m_value = *value1;
    delete value1;   
}

/// @brief IRBuilder for multipicative expression
/// AdditiveExpr :
///  MultiplicativeExpr (('+' | '-') MultiplicativeExpr
void IRBuilder::accept(AdditiveExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            cointinue;
        
        int irt = IR_INVALID;
        if (expr.m_op == AdditiveExpr::OP_PLUS)
            irt = IR_ADD;
        else if (expr.m_op == AdditiveExpr::OP_SUB)
            irt = IR_SUB;
        
        if (irt != IR_INVALID) {
            Value *value2 = allocValue(true);
            IREmiter::emitStore(value2, element->m_value);
            IREmiter::emitBinOP(irt, value1, value2, value1);
            delete value2;
        }
    }
    expr.m_value = *value1;
    delete value1;
}


/// @brief IRBuilder handler for multiplicative expression
/// MultiplicativeExpr :
/// UnaryExpresion (('*' | '/' | '%') UnaryExpr)*
void IRBuilder::accept(MultiplicativeExpr &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value *value1 = allocValue(true);
    IREmiter::emitLoad(value1, expr.m_target->m_value);
  
    vector<Expr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr *element = *ite; 
        build(element);
        if (!element->hasValidValue())
            continue;
    
        int irt = IR_INVALID;
        if (expr.m_op == MultiplicativeExpr::OP_MUL)
            irt = IR_MUL;
        else if (expr.m_op == MultiplicativeExpr::OP_DIV)
            irt = IR_DIV;
        else if (expr.m_op == MultiplicativeExpr::OP_MODULO)
            irt = IR_MODULO;
        
        if (irt != IR_INVALID) {
            Value *value2 = allocValue(true);
            IREmiter::emitStore(value2, element->m_value);
            IREmiter::emitBinOP(irt, value1, value2, value1);
            delete value2;
        }
        
    }
    expr.m_value = *value1;
    delete value1;
}

/*
unaryExpr
    | primary selector*
    ;

selector
    : assignableSelector
    | arguments
    ;

identifier
    : IDENTIFIER
    ;
arguments
        : '(' argumentList? ')'
        ;

    argumentList
        : expression (',' expression)*
        ;
    assignableSelctor
        : '.'identifier
        | '[' expression ']'
        ;
*/
/// @brief IRBuilder handler for unary expression
void IRBuilder::accept(UnaryExpr &expr) {
    ASSERT( expr.m_primary != NULL);
    Value *value = NULL;
    
    // first, process the basic unary expression
    switch (expr.m_primary->m_type) {
        case PrimaryExpr::T_TRUE:
            value = new Value(VT_CONST, 1);
            break;
        case PrimaryExpr::T_FALSE:
            value = new Value(VT_CONST, 0);
            break;
        case PrimaryExpr::T_HEX_NUMBER:
            value = new Value(VT_CINT, 0);
            break;
        case PrimaryExpr::T_NULL:
            value = new Value(VT_CINT, 0);
            break;
        case PrimaryExpr::T_STRING:
            value = new Value(VT_CSTRING, 0);
            break;
        case PrimaryExpr::T_NUMBER:
            value = new Value(VT_INT, 0);
            break;
    }
    if (value) {
        expr.m_value = *value;
        delete value;
        return;
    }
    
    // if the primary is this or super, the following syntax is only supported
    // this.identifier, this.identifier.id2 or this.identifier[2], or 
    // this.identifier()
    if (expr.m_primary->m_type == PrimaryExpr::T_THIS ||
        expr.m_primary->m_type == PrimaryExpr::T_SUPER ||
        Class *cls = getCurrentClass();
        if (!cls) return;
        
        // the first identifier must be .identifier, or null
        vector<Expr *>::iterator ite = expr.m_selectors.begin();
        // if the first selector is null, just return this only
        if (ite = expr.m_selectors.end()) {
            // get class object ptr and return
            return;
        }
        // the identifier should had be checked in type builder, 
        // so the first selector must be dot selector
        SelectorExpr *selExpr = *ite++;
        ASSERT(selExpr->m_type == SelectorExpr::DOT_SELECTOR);
        // now we have class name, the first member
        // we want get the first id's type
        for (; ite != expr.m_selectors.end(); ite++) {
            SelectorExpr *selExpr2 = *ite;
            switch (selExpr2->m_type) {
                // this.id1.id2.idx...
                case SelectorExpr::DOT_SELECTOR:
                    // getFieldOffset(Type *type, string &filed)
                    break;
                // this.id1.id2.idx()
                case SelectorExpr::FUNCTION_SELECTOR:
                    break;
                // this.id1.id2.idx[]
                case SelectorExpr::ARRAY_SELECTOR:
                    break;
                default:
                    break;
            }
        }
    // if the primary is identifier
    
    // if the primary is map/list/
}


/// @brief IRBuilder handler for primary expression
void IRBuilder::accept(PrimaryExpr &expr) {
    
}

/*
selector
    : assignableSelector
    | arguments
    ;
    */
void IRBuilder::accept(SelectorExpr &expr)
{
    
}

// new
void IRBuilder::accept(NewExpr &expr)
{
    
}

// map & list
void IRBuilder::accept(MapExpr &expr)
{
    
}
void IRBuilder::accept(ListExpr &expr)
{
    
}






