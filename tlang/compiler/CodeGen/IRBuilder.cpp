//
//  CodeBuilder.cpp
//  A toyable language compiler (like a simple c++)


#include "AST.h"
#include "Scope.h"
#include "Error.h"
#include "Label.h"
#include "Value.h"
#include "Frame.h"
#include "Variable.h"
#include "Declaration.h"
#include "Expression.h"
#include "Statement.h"
#include "IREmiter.h"
#include "IRBuilder.h"
#include "Compile.h"


IRBuilder::IRBuilder(const string& path, const string& file)
{
    m_blocks = NULL;
    CompileOption& compileOption = CompileOption::getInstance();
    if (compileOption.isOutputAssembleFile()) {
        unsigned extendPosition = file.find_last_of(".");
        string assembleFile = file.substr(0, extendPosition);
        if (!assembleFile.empty())
            m_ir.setAssembleFile(assembleFile);
    }
}

IRBuilder::~IRBuilder()
{
}

void IRBuilder::build(AST* ast)
{
    if (ast)
        ast->walk(this);
}
/// @brief Enter a new scope
void IRBuilder::enterScope(const string& name, Scope* scope)
{
    if (m_curScope) {
        scope->setParentScope(m_curScope);
        m_curScope = scope;
    } 
    m_curScopeName = name;
    if (!m_rootScope)
        m_rootScope = scope;
}
/// @brief Exit the current scope
void IRBuilder::exitScope() 
{
    if (m_curScope != NULL)
        m_curScope = m_curScope->getParentScope();
}

/// @brief Get Object by name 
Object* IRBuilder::getObject(const string& name, bool nested)
{
    if (m_curScope!= NULL)
        return m_curScope->resolveObject(name, nested);
    else
        return NULL;
}

/// @brief Get type by name
Type* IRBuilder::getType(const string& name, bool nested) 
{
    if (m_curScope != NULL)
        return m_curScope->resolveType(name, nested);
    else
        return NULL;
}

void IRBuilder::pushIterablePoint(Statement* stmt)
{
    if (stmt)
        m_iterablePoints.push_back(stmt);
}

void IRBuilder::popIterablePoint()
{
    m_iterablePoints.pop_back();
}

Statement* IRBuilder::getCurIterablePoint()
{
    if (!m_iterablePoints.empty())
        return m_iterablePoints.back();
    else
        return NULL;
}

void IRBuilder::clearIterablePoint()
{
    m_iterablePoints.clear();
}
int  IRBuilder::getLinkAddress(Method& method) 
{
    return 0; // for dummy now
}

void IRBuilder::build(AST* ast, IRBlockList* blockList) 
{
    m_blocks = blockList;
    if (ast)
        ast->walk(this);

    makeAllGlobalVariables();
}

// typespecifier
void IRBuilder::accept(TypeSpec& type) 
{
    // do nothing for TypeSpec
}

/// @brief IRBuidler handler for Variable
void IRBuilder::accept(Variable& var) 
{
    // if the variable is global, the variable should be added into globaa memory
    // according to wether it is initialized, the destional region is different.
    if (var.m_isGlobal) {
        // push all global variable into one regin
        m_globalVars.push_back(&var);
    }
    
    // if the variable is class variable
    else if (var.m_isOfClass) {
        // do nothing during the code gen
    }
    // if the variable is method local variable
    // reserve the memory from the current frame and initialize
    else {
        // get the address of the local variable
        Object* localObject = getObject(var.m_name);
        if (!localObject)
            return;
        // ASSERT(localObject->m_storage == Object::LocalStackObject);
        int localVarOffset = localObject->getAddress();

        // localVariableAddress = sp + localVarOffset
        Value val1(IR_SP);
        Value val2(true, localVarOffset);
        Value local;
        m_ir.emitBinOP(IR_ADD, val1, val2, local);

        if (var.m_expr) {
            build(var.m_expr);
            m_ir.emitLoad(local, var.m_expr->m_value);
        }
    }
}



/// \brief  make all global variables
void IRBuilder::makeAllGlobalVariables()
{
    vector<Variable* >::iterator ite = m_globalVars.begin();
    for (; ite != m_globalVars.end(); ite++) {
        Variable* var =* ite;
        ASSERT(var->m_isGlobal);
        // store the global variable in image file according to type
    }

}

/// @brief  Generate method name's specification
// methodName@class@retType@parametersCount@para1Type@para2Type...
void IRBuilder::makeMethodName(Method& method, string& name) 
{
    name = method.m_name;
    name += "@";
    name += method.m_class;
    
    name += "@";
    // name += method.m_returnType;
    name += "@";
    if (method.hasParamter()) {
        name += method.getParameterCount();
        for (int index = 0; index < method.getParameterCount(); index++) {
            MethodParameter* parameter = method.getParameter(index);
            name += "@";
            name += parameter->m_name;
        }
    }
}

/// @brief Method generator
void IRBuilder::generateMethod(Method& method) 
{
    MethodType* methodType = (MethodType* )getType(method.m_name);
    ASSERT(methodType != NULL);

    // make specified method name according to method name and parameter type
    string methodName;
    makeMethodName(method, methodName);
    
    // mark funciton lable using the name
    Label label(methodName);
    m_ir.emitLabel(label);
    
    // get method regin information and update it in MethodType
    int linkAddr = getLinkAddress(method);
    methodType->setLinkAddress(linkAddr);
    
    // walk through the parameter list
    build(method.m_paraList);

    // adjust the sp register
    int localVarSize = 0;
    if (method.m_block) {
        vector<Variable* >& vars = method.m_block->m_vars;
        vector<Variable* >::iterator ite  = vars.begin();
        for (; ite != vars.end(); ite++) {
            Variable* var =* ite;
            Type* type = getType(var->m_name);
            ASSERT(type != NULL);
            localVarSize += type->getSize();
        }
        // make value for sp register, sp = sp - localSize
        Value val1(IR_SP);
        Value val2(true, localVarSize);
        m_ir.emitBinOP(IR_SUB, val1, val2, val1);
    }
        
    build(method.m_block);
    m_ir.emit(IR_RET);
}

/// @brief Handler for Method IRBuilder
void IRBuilder::accept(Method& method) 
{
    // clear iterable point
    clearIterablePoint();
    // enter the method scope
    enterScope(method.m_name, dynamic_cast<Scope*>(&method));
    if (method.m_isOfClass) {
        // generate the code
        generateMethod(method);
    }
    
    // the method is not class/interface's method
    // in this case, generate code directly,
    else {
        generateMethod(method);
    }
    exitScope();
    clearIterablePoint(); 
}

/// @brief Handler for MethodParameterList IRBuilder
void IRBuilder::accept(MethodParameterList& list) 
{
    int index = 0;
    Method* method = (Method*)list.m_method;
    assert(method != NULL);
    
    // if  the method is member of class, the class instance ref musb be added
    if (method->m_isOfClass) {
        Object* object = new Object("this", getType(method->m_class));
        object->setStorage(Object::LocalObject);
    }
    // iterate all parameters fro right to left
    vector<MethodParameter*>::iterator ite = list.m_parameters.end();
    for (; ite != list.m_parameters.begin(); ite--) {
        MethodParameter* parameter =* ite;
        parameter->m_method = method;
        parameter->m_index = index++;
        parameter->walk(this);
    }
}

/// @brief Handler for MethodParameter IRBuilder
void IRBuilder::accept(MethodParameter& para) 
{
    Method* method = para.m_method;
    
}

/// @brief Handler for MethodBlock IRBuilder
void IRBuilder::accept(MethodBlock& block) 
{
    vector<Statement*>::iterator ite = block.m_stmts.begin();
    for (; ite != block.m_stmts.end(); ite++) 
        build(*ite);
}


/// @brief IRBuilder handler for Class
void IRBuilder::accep(Class& cls) 
{
    build(cls.m_block);
}
/// @brief IRBuilder handler for ClassBlock
void IRBuilder::accept(ClassBlock& block) 
{
    vector<Variable* >::iterator itv = block.m_vars.begin();
    for (; itv !=  block.m_vars.end(); itv++)
        build(*itv);

    vector<Method* >::iterator itm = block.m_methods.begin();
    for (; itm != block.m_methods.end(); itm++)
        build(*itm);
}


/// @brief IRBuilder handler for statement
void IRBuilder::accept(Statement& stmt) 
{
    // Do nothing, this is base class for all statement
}

/// @brief IRBuilder handler for include statement
void IRBuilder::accept(IncludeStatement& stmt) 
{
    
}

/// @brief IRBuilder handler for block statement
void IRBuilder::accept(BlockStatement& stmt) 
{
    /// Crate a new Block and insert it into blockList;
    IRBlock* block = new IRBlock();
    // m_blocks.push_back(block);
   
    enterScope("blockStatement", dynamic_cast<Scope*>(&stmt));
    /// Iterate all statement and generate intermeidate instructions
    vector<Statement*>::iterator ite = stmt.m_stmts.begin();
    for (; ite != stmt.m_stmts.end(); ite++) {
        build(*ite);
    }
    exitScope(); 
}

/// @brief IRBuilder handler for variable decl statement
void IRBuilder::accept(VariableDeclStatement& stmt) 
{
    // get the local variable's Object
    Object* Object = getObject(stmt.m_var->m_name);
    ASSERT(Object != NULL);
    ASSERT(Object->getStorage() == Object::LocalObject);

    // get address of  the local variable in stack
    Value val1(IR_SP);
    Value val2(true, Object->getAddress());
    m_ir.emitBinOP(IR_ADD, val1, val2, val1);
    
    // if the declared variable is initialized
    Value result(true);
    if (stmt.m_expr) {
        build(stmt.m_expr);
        result = stmt.m_expr->m_value;
    }
    
    // load the result into locals
   m_ir.emitLoad(val1, result);
}

/// @brief IRBuilder handler for if statement
void IRBuilder::accept(IfStatement& stmt) 
{
    ASSERT(stmt.m_conditExpr != NULL);
    
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    
    // check wether there is only a if block
    if (stmt.m_elseBlockStmt == NULL) {
        build(stmt.m_conditExpr);
        if (!stmt.m_conditExpr->hasValidValue())
            return;
        Value value1(true);
        Value value2(false, 1); 
        m_ir.emitStore(value1, stmt.m_conditExpr->m_value);
        m_ir.emitCMP(value1, value2, label1, label2);
        m_ir.emitLabel(label1);
        build(stmt.m_ifBlockStmt);
    }
    
    else {
        build(stmt.m_conditExpr);
        if (!stmt.m_conditExpr->hasValidValue())
            return;
        Label label3 = Label::newLabel();
        Value value1(true);
        Value value2(false, 1);
        m_ir.emitStore(value1, stmt.m_conditExpr->m_value);
        m_ir.emitCMP(value1, value2, label1, label3);
        m_ir.emitLabel(label1);
        build(stmt.m_ifBlockStmt);
        
        m_ir.emitLabel(label3);
        build(stmt.m_elseBlockStmt);
    }
    
    m_ir.emitLabel(label2);
}

/// @brief IRBuilder handler for while statement
void IRBuilder::accept(WhileStatement& stmt) 
{ 
    ASSERT(stmt.m_conditExpr != NULL);
    
    // push iterable statement into stack
    pushIterablePoint(&stmt);
    
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel(); 
    Label label3 = Label::newLabel();
    
    m_ir.emitLabel(label1);
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);
    
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value value1(true);
    Value value2(false, 1);
    m_ir.emitStore(value1, stmt.m_conditExpr->m_value);
    m_ir.emitCMP(value1, value2, label2, label3);
    m_ir.emitLabel(label2);
    
    build(stmt.m_stmt);
    m_ir.emitJump(label1);
    m_ir.emitLabel(label3);
    
    // popup the current iterable statement
    popIterablePoint();
}

/// @brief IRBuilder handler for do while statement
void IRBuilder::accept(DoStatement& stmt) 
{
    ASSERT(stmt.m_conditExpr != NULL);
        
    pushIterablePoint(&stmt);

    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();   
    
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label2);
    
    m_ir.emitLabel(label1);
    build(stmt.m_stmt);
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value value1(true);
    Value value2(false, 1);
    m_ir.emitStore(value1, stmt.m_conditExpr->m_value);
    m_ir.emitCMP(value1, value2, label1, label2);
    m_ir.emitLabel(label2);
    // popup the current iterable statement
    popIterablePoint();
}

/// @brief IRBuilder handler for for statement
void IRBuilder::accept(ForStatement& stmt) 
{
    // now only support normal loop mode
    // for (expr1; expr2; exprlist) statements
    if (stmt.m_expr1)
        build(stmt.m_expr1);
    // push iterable statement into frame
    pushIterablePoint(&stmt);

    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Label label3 = Label::newLabel();
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);

    m_ir.emitLabel(label1);
        
    if (stmt.m_expr2) {
        build(stmt.m_expr2);
        Value value1(true);
        Value value2(false, 1);
        m_ir.emitStore(value1, stmt.m_expr2->m_value);
        m_ir.emitCMP(value1, value2, label2, label3);
    }
    m_ir.emitLabel(label2);
    build(stmt.m_stmt);
    
    if (stmt.m_exprList) 
        build(stmt.m_exprList);
    m_ir.emitJump(label1);
    m_ir.emitLabel(label3);
    // popup the current iterable point
    popIterablePoint();
}

// foreachStatement
// : 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' foreachSetObject ')' blockStatement
 //   ;
void IRBuilder::accept(ForEachStatement& stmt) 
{
    // push iterable statement into frame
    pushIterablePoint(&stmt);

    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Label label3 = Label::newLabel();
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);
    
    Object* object = NULL;
    Type* type = NULL;

    Value indexValue(true); 
    m_ir.emitLabel(label1);
    switch (stmt.m_objectSetType) {
        case ForEachStatement::Object:
            object = getObject(stmt.m_objectSetName);
            type = getType(stmt.m_objectSetName);
            ASSERT(object != NULL);
            ASSERT(type != NULL);
            if (type && isType(type, "set")) {
                ASSERT(stmt.m_varNumbers == 1);
                Type* valType = getType(stmt.m_typeSpec[0]->m_name); 
                SetType* setType = dynamic_cast<SetType* >(type); 
                // get object element size
                vector<Value> arguments; 
                Value objectSelf(false, object->getAddress());
                arguments.push_back(objectSelf);
                Value elementSize(true);
                string methodName = "size"; 
                callObjectMethod(stmt.m_objectSetName, methodName, arguments, elementSize); 
                // compare the indexValue with elmentSize
                m_ir.emitCMP(elementSize, indexValue, label2, label3);
                m_ir.emitLabel(label2); 
                // get the indexed elment
                arguments.clear();
                arguments.push_back(objectSelf);
                arguments.push_back(indexValue); 
                Value element(true);
                methodName = "at"; 
                callObjectMethod(stmt.m_objectSetName, methodName, arguments, element);
                // the Object should be updated according to the result from callObjectMethod 
                object = getObject(stmt.m_id[0]);
                Value val(true, object->getAddress());
                m_ir.emitLoad(val, element);
                build(stmt.m_stmt); 
                // increase the index value
                m_ir.emit(IR_INC, indexValue);
                // jump to the loop start 
                m_ir.emitJump(label2); 
            }
            else if (type && isType(type, "map")) {
                ASSERT(stmt.m_varNumbers == 2);
                Type* keyType = getType(stmt.m_typeSpec[0]->m_name);
                Type* valType = getType(stmt.m_typeSpec[1]->m_name);
                MapType* mapType = dynamic_cast<MapType* >(type); 
                // get object element size by calling enumerableObject::size() method
                vector<Value> arguments;
                Value objectSelf(false, object->getAddress());
                arguments.push_back(objectSelf);
                Value elementSize(true);
                string methodName = "size";
                callObjectMethod(stmt.m_objectSetName, methodName, arguments, elementSize);
                // compare the indexValue with elementSize
                m_ir.emitCMP(elementSize, indexValue, label2, label3);
                m_ir.emitLabel(label2);
                
                // get the indexed element
                Object* keyObject = getObject(stmt.m_id[0]);
                Object* valObject = getObject(stmt.m_id[1]);
                Value key(true, keyObject->getAddress());
                Value val(true, valObject->getAddress());
                
                // call the method 
                arguments.clear();
                arguments.push_back(objectSelf);
                arguments.push_back(indexValue);
                arguments.push_back(key);
                arguments.push_back(val);
                methodName = "getElement";
                Value result(true);
                callObjectMethod(stmt.m_objectSetName, methodName, arguments, result);
                // update the Object
                build(stmt.m_stmt);
                // increase the index value
                m_ir.emit(IR_INC, indexValue);
                // jump to the loop start
                m_ir.emitJump(label2);
            }
            else {
                Error::complain(stmt, "Unknow object type in foreach statement\n");
                popIterablePoint(); 
                return;
            }
            break;
        case ForEachStatement::MapObject:
            break;
        case ForEachStatement::SetObject:
            break;
        default:
            break;
    }
    m_ir.emitJump(label1);
    m_ir.emitLabel(label3);
}

/// @brief IRBuilder handler for switch statement
void IRBuilder::accept(SwitchStatement& stmt) 
{
    // check parameter's validity
    ASSERT(stmt.m_conditExpr != NULL);
    
    // condition judge statement
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value value1(true);
    m_ir.emitStore(value1, stmt.m_conditExpr->m_value);
    Label label2 = Label::newLabel();
    
    // generate case statement
    vector< std::pair<vector<Expr*>, Statement* > >::iterator ite = stmt.m_cases.begin();
    for (; ite != stmt.m_cases.end(); ite++)  {
        //
        std::pair<vector<Expr* >, Statement*>& pt =* ite;
        std::vector<Expr* > exprList = pt.first;
        Statement* sts = pt.second;
        
        // case n
        // case n+1
        Label label1 = Label::newLabel();
        std::vector<Expr* >::iterator ite = exprList.begin();
        for (; ite != exprList.end(); ite++) {
            // get case value
            Expr*  expr =* ite;
            build(expr);
            if (!expr->hasValidValue())
                continue;
            Value value2(true);
            m_ir.emitStore(value2, expr->m_value);
            m_ir.emitCMP(value2, value1, label1, label2);
        }
        
        m_ir.emitLabel(label1);
        // build statement
        build(sts);
        m_ir.emitJump(label2);
    }
    
    if (stmt.m_defaultStmt)
        build(stmt.m_defaultStmt);
    
    m_ir.emitLabel(label2);
}

/// @brief IRBuilder handler for continue statement
void IRBuilder::accept(ContinueStatement& stmt) 
{
    Statement* startStmt = getCurIterablePoint();
    if (startStmt && startStmt->isIterable())
        m_ir.emitJump(startStmt->getIterableStartPoint());
}

/// @brief IRBuilder handler for break statement
void IRBuilder::accept(BreakStatement& stmt) 
{
    Statement* startStmt = getCurIterablePoint();
    if (startStmt && startStmt->isIterable())
        m_ir.emitJump(startStmt->getIterableEndPoint());
}


/// @brief IRBuilder handler for return statement
void IRBuilder::accept(ReturnStatement& stmt) 
{
    Value ret(IR_R0);
    if (stmt.m_resultExpr) {
        build(stmt.m_resultExpr);
        if (stmt.m_resultExpr->hasValidValue()) {
            // store the result
            m_ir.emitLoad(ret, stmt.m_resultExpr->m_value);
        }
        else {
            Value result(false, 0);
            m_ir.emitLoad(ret, result); 
        }
    }
    m_ir.emit(IR_RET);
}

/// @brief IRBuilder handler for assert statement
void IRBuilder::accept(AssertStatement& stmt) 
{
    ASSERT(stmt.m_resultExpr != NULL);
    
    build(stmt.m_resultExpr);
    if (!stmt.m_resultExpr->hasValidValue())
        return;
    
    // judge the expression 
    Value local(true);
    m_ir.emitStore(local, stmt.m_resultExpr->m_value);
    
    // generate the jump labe
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    m_ir.emitCMP(local, 1, label1, label2);
    
    m_ir.emitLabel(label2);
    m_ir.emitException();
    m_ir.emitLabel(label1);
}

/// @brief IRBuilder handler for throw statement
/// the exception handler should be implement by method type
/// throw 'expression'
void IRBuilder::accept(ThrowStatement& stmt) 
{
    
}

/// @brief IRBuilder handler for try statement
/// 'try' blockStatement ((catchPart+finallyPart)?|finallyPart)
void IRBuilder::accept(TryStatement& stmt) 
{
    
}
/// @brief IRBuilder handler for catch statement
void IRBuilder::accept(CatchStatement& stmt) 
{
    
}

/// @brief IRBuilder handler for finally catch satement
void IRBuilder::accept(FinallyCatchStatement& stmt) 
{
    
}

void IRBuilder::accept(ExprStatement& stmt)
{

}
// expression
void IRBuilder::accept(Expr& expr)
{
}
void IRBuilder::accept(ExprList& exprList)
{
    vector<Expr* >::iterator ite = exprList.m_exprs.begin();
    for (; ite != exprList.m_exprs.end(); ite++) 
    {
        Expr* expr =* ite;
        build(expr);
    }
    
}
/// @brief IRBuilder handler for BinaryOpExpr
void IRBuilder::accept(BinaryOpExpr& expr) 
{
    ASSERT(expr.m_left != NULL);
    ASSERT(expr.m_right != NULL);
    
    build(expr.m_left);
    if (!expr.m_left->hasValidValue())
        return;
    Value left(true);
    m_ir.emitLoad(left, expr.m_left->m_value);
    
    build(expr.m_right);
    if (!expr.m_left->hasValidValue()) {
        return;
    }
    Value right(true);
    m_ir.emitLoad(right, expr.m_right->m_value);
    
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
        m_ir.emitBinOP(op, left, right, left);
    expr.m_value = left;
}

/// @brief IRBuilder handler for ConditionalExpr
void IRBuilder::accept(ConditionalExpr& expr) 
{

}

/// @brief IRBuilder handler for LogicOrExpr
void IRBuilder::accept(LogicOrExpr& expr)
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;

    Value value1(true);
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    
    m_ir.emitLoad(value1, expr.m_target->m_value);
    m_ir.emitCMP(value1, 1, label1, label2);
    m_ir.emitLabel(label2);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        m_ir.emitLoad(value2, element->m_value);
        m_ir.emitBinOP(IR_OR, value1, value2, value1);
        
        Label label3 = Label::newLabel();
        m_ir.emitCMP(value1, 1, label1, label3);
        m_ir.emitLabel(label3);
    }
    
    // store the result into expr.result
    expr.m_value = value1;
    m_ir.emitLabel(label1);
}

/// @brief IRBilder handler for LogicAndExpr
void IRBuilder::accept(LogicAndExpr& expr)
{
    ASSERT(expr.m_target != NULL);
   
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Value value1(true);
    
    m_ir.emitLoad(value1, expr.m_target->m_value);
    m_ir.emitCMP(value1, 1, label1, label2);
    m_ir.emitLabel(label2);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        Label label3 = Label::newLabel();
        m_ir.emitLoad(value2, element->m_value);
        m_ir.emitBinOP(IR_AND, value1, value2, value1);
        
        m_ir.emitCMP(value1, 1, label3, label1);
        m_ir.emitLabel(label3);
    }
    
    // store the result into expr.result
    expr.m_value = value1;
    m_ir.emitLabel(label1);
}


/// @brief IRBilder handler for bitwise or expression
// BitwiseOrExpr : BitwiseXorExpr ( '|' bitwiseXorExpr)*
void IRBuilder::accept(BitwiseOrExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
   
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        m_ir.emitLoad(value2, element->m_value);
        m_ir.emitBinOP(IR_BIT_OR, value1, value2, value1);
    }
    
    expr.m_value = value1; 
}

/// @brief IRBuilder handler for bitwise xor expression
/// BitwiseXorExpr : BitwiseAndExpr ('^' bitwiseAndExpr)*
void IRBuilder::accept(BitwiseXorExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        Value value2(true);
        m_ir.emitStore(value2, element->m_value);
        m_ir.emitBinOP(IR_BIT_XOR, value1, value2, value1);
    }
    expr.m_value = value1;
}

/// @brief IRBuilder handler for bitwise and expression
/// BitwiseAndExpr : EqualityExpr ('&' EqualilityExpr)*
void IRBuilder::accept(BitwiseAndExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        m_ir.emitStore(value2, element->m_value);
        m_ir.emitBinOP(IR_BIT_AND, value1, value2, value1);
    }
    expr.m_value = value1;
}

/// @brief IRBuilder handler for equality Expr
/// EqualityExpr : RelationalExpr (('==' | '!=') RelationalExpr)*
void IRBuilder::accept(EqualityExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == EqualityExpr::OP_EQ)
            irt = IR_EQ;
        else if (expr.m_op == EqualityExpr::OP_NEQ)
            irt = IR_NEQ;
        
        if (irt != IR_INVALID) { 
            Value value2(true);
            m_ir.emitStore(value2, element->m_value);
            m_ir.emitBinOP(irt, value1, value2, value1);
        }
    }
    
    expr.m_value = value1;
}

/// @brief IRBuilder handler ffor relational expression
/// RelationalExpr :
///  ShiftExpr (('>' | '<' | '>=' | '<=') ShiftExpr)*
void IRBuilder::accept(RelationalExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
    
    // build expression and get result into local
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
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
            Value value2(true);
            m_ir.emitStore(value2, element->m_value);
            m_ir.emitBinOP(irt, value1, value2, value1);
        }
    }
    expr.m_value = value1;
}

/// @brief IRBuilder handler for shift expression
/// ShiftExpr : AdditiveExpr (('>>'|'<<') AdditiveExpr)*
void IRBuilder::accept(ShiftExpr& expr) 
{
    ASSERT(expr.m_target != NULL);

    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emitStore(value1, expr.m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == ShiftExpr::OP_LSHIFT)
            irt = IR_LSHIFT;
        else if (expr.m_op == ShiftExpr::OP_RSHIFT)
            irt = IR_RSHIFT;
        
        if (irt != IR_INVALID) {
            Value value2(true);
            m_ir.emitStore(value2, element->m_value);
            m_ir.emitBinOP(irt, value1, value2, value1);
        }
    }
    expr.m_value = value1;
}

/// @brief IRBuilder for multipicative expression
/// AdditiveExpr :
///  MultiplicativeExpr (('+' | '-') MultiplicativeExpr
void IRBuilder::accept(AdditiveExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
    
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == AdditiveExpr::OP_PLUS)
            irt = IR_ADD;
        else if (expr.m_op == AdditiveExpr::OP_SUB)
            irt = IR_SUB;
        
        if (irt != IR_INVALID) {
            Value value2(true);
            m_ir.emitStore(value2, element->m_value);
            m_ir.emitBinOP(irt, value1, value2, value1);
        }
    }
    expr.m_value = value1;
}


/// @brief IRBuilder handler for multiplicative expression
/// MultiplicativeExpr :
/// UnaryExpresion (('*' | '/' | '%') UnaryExpr)*
void IRBuilder::accept(MultiplicativeExpr& expr) 
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emitLoad(value1, expr.m_target->m_value);
  
    vector<Expr* >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expr* element =* ite; 
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
            Value value2(true);
            m_ir.emitStore(value2, element->m_value);
            m_ir.emitBinOP(irt, value1, value2, value1);
        }
        
    }
    expr.m_value = value1;
}

/// @brief IRBuilder handler for unary expression
void IRBuilder::accept(UnaryExpr& expr) 
{

    // the min and key expression, 
    // it must tell other expression wether it has valid value and the value
    ASSERT( expr.m_primary != NULL);
    Value* value = NULL;
    
    // first, process the basic unary expression
    switch (expr.m_primary->m_type) {
        case PrimaryExpr::T_TRUE:
            value = new Value(false, 1);
            break;
        case PrimaryExpr::T_FALSE:
            value = new Value(false, 0);
            break;
        case PrimaryExpr::T_HEX_NUMBER:
            value = new Value(false, 0);
            break;
        case PrimaryExpr::T_NULL:
            value = new Value(false, 0);
            break;
        case PrimaryExpr::T_STRING:
            value = new Value(false, 0);
            break;
        case PrimaryExpr::T_NUMBER:
            value = new Value(false, 0);
            break;
    }
    if (value) {
        expr.m_value =* value;
        delete value;
        return;
    }
    
    switch (expr.m_primary->m_type) {
        case PrimaryExpr::T_SELF:
        case PrimaryExpr::T_SUPER:
        case PrimaryExpr::T_IDENTIFIER: 
            value = handleSelectorExpr(*expr.m_primary, expr.m_selectors);
            if (value) { 
                expr.m_value =* value;
                delete value;
            }
            break;
        case PrimaryExpr::T_COMPOUND:
           build(expr.m_primary);
           expr.m_value = expr.m_primary->m_value;
           break;
        default:
           Error::complain(expr, "Unknow primary expression\n");
           break;
    }
}

Value*  IRBuilder::handleSelectorExpr(
        PrimaryExpr& primExpr, 
        vector<SelectorExpr*>& elements)
{
    // only handle the following primaryExpression
    if (primExpr.m_type != PrimaryExpr::T_IDENTIFIER ||
        primExpr.m_type != PrimaryExpr::T_SELF ||
        primExpr.m_type != PrimaryExpr::T_SUPER )
            return NULL;
    // get Object infor about the primExpr
    Object* Object = getObject(primExpr.m_text);
    ASSERT(Object != NULL);

    // load the Object address into register
    Value base(true);
    Value offset(false, Object->getAddress());
    m_ir.emitLoad(base, offset);
    base = offset; 

    // get type of current primary test
    Type* type = getType(primExpr.m_text);
    string curText = primExpr.m_text;
    // for each selector
    vector<SelectorExpr* >::iterator ite = elements.begin();
    for (; ite != elements.end(); ite++) {
        SelectorExpr* selector = dynamic_cast<SelectorExpr* >(*ite);
        ASSERT(selector != NULL);

        if (selector->m_type == SelectorExpr::DOT_SELECTOR) {
            type = type->getSlot(selector->m_id);
            // adjust the offset
            ASSERT(type != NULL);
            Value val(false, type->getSize());
            m_ir.emitBinOP(IR_ADD, base, val, base);

            curText = selector->m_id;
        }

        else if (selector->m_type == SelectorExpr::ARRAY_SELECTOR) {
            // if the selector is array, the subscriptor must be known
            ASSERT(selector->m_arrayExpr != NULL);
            // build the arrary subscrip expression to get index 
            build(selector->m_arrayExpr);
            // get the element type
            type = type->getSlot(0);
            ASSERT(type != NULL);

            Value val1(true);
            m_ir.emitLoad(val1, selector->m_arrayExpr->m_value);
            // till now, the array index is loaded into val1
            Value val2(true, type->getSize());
            m_ir.emitBinOP(IR_MUL, val1, val2, val1);
            // now, the target element offset is loaded into val1
            // update the base
            m_ir.emitBinOP(IR_ADD, val1, base, base);
        }
        else if (selector->m_type == SelectorExpr::METHOD_SELECTOR) {
            MethodType* methodType = (MethodType* )getType(curText);
            ASSERT(methodType != NULL);
            // if the method is member of class, the first parameter should be 
            // ref of class object which will be stored as the first parameter 
            if (methodType->isOfClassMember()) {
                Value val(true); 
                m_ir.emitLoad(val, base);
                m_ir.emitPush(val); 
            }
            MethodCallExpr* methodCallExpr = selector->m_methodCallExpr;
            build(methodCallExpr);
            type = methodType->getReturnType();

        }
        else
            Error::complain(primExpr, "Unknow selector\n");
    }
}


/// @brief IRBuilder handler for primary expression
void IRBuilder::accept(PrimaryExpr& expr) 
{
    // do nothig    
}

void IRBuilder::accept(SelectorExpr& expr)
{
   // do nothing now 
}

void IRBuilder::accept(MethodCallExpr& expr) 
{
   vector<Expr* >::iterator ite = expr.m_arguments.end();
   for (; ite != expr.m_arguments.begin(); ite--) {
        Expr* argument =* ite;
        build(argument);
        // the argument shoud be push into stack
        Value val(true);
        m_ir.emitLoad(val, argument->m_value);
        m_ir.emitPush(val);
   }
   string& methodName = expr.getMethodName();
   Object* Object = getObject(methodName);
   ASSERT(Object != NULL);

   Value methodAddr(true, Object->getAddress());
   m_ir.emitMethodCall(methodAddr);
}

// new
void IRBuilder::accept(NewExpr& expr)
{
    
}

// map &  list
void IRBuilder::accept(MapExpr& expr)
{
    
}
void IRBuilder::accept(SetExpr& expr)
{
    
}

void IRBuilder::accept(MapItemExpr& expr)
{
}


