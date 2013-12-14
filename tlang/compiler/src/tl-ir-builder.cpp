//
//  tl-builder.cpp
//  A toyable language compiler (like a simple c++)
//

#include "tl-ast.h"
#include "tl-scope.h"
#include "tl-error.h"
#include "tl-label.h"
#include "tl-value.h"
#include "tl-frame.h"
#include "tl-ast-decl.h"
#include "tl-ast-expr.h"
#include "tl-ast-stmt.h"
#include "tl-ir-emiter.h"
#include "tl-ir-builder.h"
#include "tl-compile.h"
#include "tl-compile-option.h"


using namespace tlang;

//===---------------------------------------------------------------------===//
//  Class implementation
//===---------------------------------------------------------------------===//


/// Constructor for IRBuilder.
/// If the compiler option for assemble is provided, 
/// the intermediate assemble file will be generated.
/// @param path the compiled file full path
/// @param file the compiled file name
IRBuilder::IRBuilder(const string &path, const string &file)
{
    m_blocks = NULL;
    m_curScope = NULL;
    m_rootScope = NULL;

    CompileOption &compileOption = CompileOption::getInstance();
    if (compileOption.isOutputAssembleFile()) {
        unsigned extendPosition = file.find_last_of(".");
        string assembleFile = file.substr(0, extendPosition);
        assembleFile += ".tasm"; 
        if (!assembleFile.empty())
            m_ir.setAssembleFile(assembleFile);
        m_ir.setOutputPath(path);
        m_ir.prepare(); 
    }
}
/// Destructor for IRBuilder.
IRBuilder::~IRBuilder()
{
}

/// helper method to walk ast node.
/// @param ast the abstract syntax tree node that will be visited
/// @see AST
/// @see ASTVistor
void 
IRBuilder::build(AST *ast)
{
    if (ast)
        ast->walk(this);
}

/// Enter a new scope.
/// @param name the scope's name
/// @param scope the new scope
void 
IRBuilder::enterScope(const string &name, Scope *scope)
{
    if (m_curScope) 
        scope->setParentScope(m_curScope);
    m_curScope = scope;
    m_curScopeName = name;
    if (!m_rootScope)
        m_rootScope = scope;
}
/// Exit the current scope.
void 
IRBuilder::exitScope() 
{
    Assert(m_curScope != NULL); 
    m_curScope = m_curScope->getParentScope();
}

/// Get Object from current scope by name 
Object* 
IRBuilder::getObject(const string &name, bool nested)
{
    if (m_curScope!= NULL)
        return m_curScope->resolveObject(name, nested);
    else
        return NULL;
}

/// Push the current class in stack
void 
IRBuilder::pushClass(ASTClass *cls)
{
    m_classStack.push(cls);
}
/// popup the currrent class
void 
IRBuilder::popClass()
{
    if (!m_classStack.empty())
        m_classStack.pop();
}
/// get current class
ASTClass* 
IRBuilder::getCurrentClass()
{
    if (!m_classStack.empty())
        return m_classStack.top();
    else
        return NULL;
}

/// Get type by name
Type* 
IRBuilder::getType(const string &name, bool nested) 
{
    Type *type = NULL; 
    if (m_typeDomain) {
        // first,check the builtin type domain 
        string domain = "builtin"; 
        m_typeDomain->getType(domain, name,  &type);
        if (!type) {
            domain = "classType";
            m_typeDomain->getType(domain, name,  &type);
        }
    } 
    return type;
}
/// Get type pointer in class context by object's name
Type* 
IRBuilder::getType(const string &clsName, const string &name)
{
    Type *type = NULL; 
    if (m_typeDomain)
        m_typeDomain->getType(clsName, name,  &type);
    return type;
}

/// Save current iterable point.
/// If the statement can be breaked or continue,
/// the statement must be reserved to deal the break or continue statement
void 
IRBuilder::pushIterablePoint(ASTStatement *stmt)
{
    if (stmt)
        m_iterablePoints.push_back(stmt);
}
/// Popup the iterable statement.
void 
IRBuilder::popIterablePoint()
{
    m_iterablePoints.pop_back();
}
/// Get the current iterable statement.
/// To deal with break and continue statement, the iterable statement point
/// must be known.
ASTStatement* 
IRBuilder::getCurIterablePoint()
{
    if (!m_iterablePoints.empty())
        return m_iterablePoints.back();
    else
        return NULL;
}
/// Clear all iterable points
void 
IRBuilder::clearIterablePoint()
{
    m_iterablePoints.clear();
}

int  
IRBuilder::getLinkAddress(ASTMethod &method) 
{
    return 0; // for dummy now
}


/// Main entry for IRBuilder.
/// build the abstract syntax tree and generate intermediate code
void 
IRBuilder::build(AST *ast, IRBlockList *blockList, TypeDomain *typeDomain) 
{
    // the parameter must not be null 
    Assert(ast != NULL);
    Assert(blockList != NULL);
    Assert(typeDomain != NULL);
    
    m_typeDomain = typeDomain; 
    m_blocks = blockList;
    // walk the root node using myself as ast walker
    ast->walk(this);
    // the class's global member variable will be saved when iterate variable
    // type, after the AST is completely iterated, all global variable will be
    // saved into tof(tlang object format).
    makeAllGlobalVariables();
}


void
IRBuilder::accept(ASTCompileUnit &unit)
{
    vector<AST*>::iterator ite = unit.m_childs.begin();
    for (; ite != unit.m_childs.end(); ite++)
        build(*ite);
}

void 
IRBuilder::accept(ASTDeclaration &decl)
{}
void 
IRBuilder::accept(ASTAnnotation &annotation)
{}
// TypeDeclifier
void 
IRBuilder::accept(ASTTypeDecl &type) 
{}

/// Handler for variable. 
/// global variable will be saved to deal in later
/// class's member varaible will not be dealed.
/// local Variable is dealed in localVariableDeclarationStatement
void 
IRBuilder::accept(ASTVariable &var) 
{
    // if the variable is global, variable should be added into global memory
    // according to wether it is initialized, the destional region is different.
    if (var.m_isGlobal) {
        Object *object = getObject(var.m_name);
        Assert(object != NULL);
        Assert(object->getStorage() == Object::HeapObject);
        // alloc the object in heap and get offset from heap  
        allocHeapObject(object);
        
        // push all global variable into one regin
        m_globalVars.push_back( &var);
    }
}

/// Make all class global variables in tof 
void 
IRBuilder::makeAllGlobalVariables()
{
    vector<ASTVariable *>::iterator ite = m_globalVars.begin();
    for (; ite != m_globalVars.end(); ite++) {
        // store the global variable in image file according to type
    }
}
/// Alloc object in heap in advanced. 
/// the offset in heap will be saved into object 
int 
IRBuilder::allocHeapObject(Object *object)
{
    return 0;
}

/// Mangle method name by specification.
/// class/methodName/returnType/formalParameterTypeName/...
/// Method will be called in following format
/// call(class, method, object, para1, para2,...)
void 
IRBuilder::mangleMethodName(ASTMethod &method, string &name) 
{
    name = method.m_name;
    name += "@";
    name += method.m_class;
    // return type managling 
    name += "@";
    if (method.m_retTypeDecl->m_name.empty())
        name += "void";
    else
        name += method.m_retTypeDecl->m_name;
    // parameter type managling 
    if (method.hasParamter()) {
        name += method.getParameterCount();
        for (int index = 0; index < method.getParameterCount(); index++) {
            ASTFormalParameter *parameter = method.getParameter(index);
            // get parameter type
            Type *type = getType(parameter->m_name);
            // Assert(type != NULL);
            name += "@";
            if (type)
                name += type->getName();
            else
                name += "Unknown";
        }
    }
}

/// Handler for method
void 
IRBuilder::accept(ASTMethod &method) 
{
    // if the method is only declaration, don't generate ir for this method 
    if (!method.m_block)
        return;
    
    // clear iterable point
    clearIterablePoint();
    // enter the method scope
    enterScope(method.m_name, dynamic_cast<Scope *>(&method));
    
    // get the method type
    // type information had been checked by type builder
    ClassType *clsType = (ClassType *)getType(method.m_class);
    Assert(clsType != NULL);
    VirtualTable *vtbl = clsType->getVirtualTable();
    MethodType *methodType = (MethodType *)vtbl->getSlot(method.m_name);
    Assert(methodType != NULL);

    // make specified method name according to method name and parameter type
    string methodName;
    mangleMethodName(method, methodName);
    
    // mark method label using the name
    Label label(methodName);
    m_ir.emitLabel(label);
    
    // get method regin information and update it in MethodType
    int linkAddr = getLinkAddress(method);
    methodType->setLinkAddress(linkAddr);

    // prepare method frame
    Value val1(IR_BP);
    Value val2(IR_SP);
    m_ir.emit(IR_LOAD, val1, val2);

    // walk through the parameter list
    build(method.m_paraList);
    // the local variable and statement will be processed in block 
    build(method.m_block);

    // generate return instruction 
    m_ir.emit(IR_RET);
    exitScope();
    // clear iterable point again 
    clearIterablePoint(); 
}

/// handle FormalParameterList
void 
IRBuilder::accept(ASTFormalParameterList &list) 
{}

/// handle FormalParameter
void 
IRBuilder::accept(ASTFormalParameter &parameter) 
{}

/// handle MethodBlock
void 
IRBuilder::accept(ASTMethodBlock &block) 
{
    build(block.m_block);
}

/// handle class declaration.   
void 
IRBuilder::accept(ASTClass &cls) 
{
    // create the tlang object file
    m_ir.createTof(cls.m_name);
    // in some case, the class declarations is nested 
    pushClass(&cls); 
    enterScope(cls.m_name, dynamic_cast<Scope *>(&cls));
    // iterate all declarations in this class
    vector<ASTDeclaration *>::iterator ite = cls.m_declarations.begin();
    for (; ite != cls.m_declarations.end(); ite++)
        build(*ite);

    exitScope();
    popClass();
    m_ir.closeTof();
}
void 
IRBuilder::accept(ASTPackageDecl &decl)
{}
void 
IRBuilder::accept(ASTArgumentList &arguments)
{}
void 
IRBuilder::accept(ASTIterableObjectDecl &decl)
{}
void 
IRBuilder::accept(ASTMapInitializer &mapInitializer)
{}
void 
IRBuilder::accept(ASTMapPairItemInitializer &pairItemInitializer)
{}
void 
IRBuilder::accpet(ASTArrayInitializer &arrayInitializer)
{}

/// handle the block
void 
IRBuilder::accept(ASTBlock &block)
{
    // adjust the locak stack pointer at first 
    int localObjectTotalSize = 0; 
    vector<ASTVariable *>::iterator itv = block.m_variables.begin();
    for (; itv != block.m_variables.end(); itv++) {
        ASTVariable *variable =  *itv; 
        Type *variableType = variable->m_type; 
        Assert(variableType != NULL);

        localObjectTotalSize += variableType->getSize(); 
    }
    // after the object total size is known, adjust the stack
    Value val1(IR_SP);
    Value val2(true, localObjectTotalSize);
    m_ir.emit(IR_SUB, val1, val2, val1);
    
    // construct the local object 
    itv = block.m_variables.begin();
    int offset = 0;
    for (; itv != block.m_variables.end(); itv++) {
        ASTVariable *variable =  *itv;
        Type *variableType = variable->m_type;
        constructObject(Object::LocalObject, offset, variableType);

        // update the local object information
        Object *object = getObject(variable->m_name);
        Assert(object->getStorage() == Object::LocalObject);
        object->setOffset(offset);

        // update offset for next variable
        offset += variableType->getSize();
    } 
    // build the statement in this block
    vector<ASTStatement *>::iterator ite = block.m_stmts.begin();
    for (; ite != block.m_stmts.end(); ite++)
        build(*ite);
}

///  IRBuilder handler for statement
void 
IRBuilder::accept(ASTStatement &stmt) 
{
    // Do nothing, this is base class for all statement
}

///  IRBuilder handler for include statement
void 
IRBuilder::accept(ASTImportDecl &decl) 
{
    // in ir generation phase, do nothing now    
}

///  IRBuilder handler for block statement
void 
IRBuilder::accept(ASTBlockStmt &stmt) 
{
    // Crate a new Block and insert it into blockList;
    m_ir.emitBlock(); 
    enterScope("blockStatement", dynamic_cast<Scope *>(&stmt));
    // Iterate all statement and generate intermeidate instructions
    vector<ASTStatement *>::iterator ite = stmt.m_stmts.begin();
    for (; ite != stmt.m_stmts.end(); ite++) 
        build(*ite);
    exitScope(); 
}

///  IRBuilder handler for variable decl statement
void 
IRBuilder::accept(ASTLocalVariableDeclarationStmt &stmt) 
{
    // get the local variable's Object
    Object *object = getObject(stmt.m_var->m_name);
    Assert(object != NULL);
    Assert(object->getStorage() == Object::LocalObject);

    // adjust the stack pointer for local object 
    Value val1(IR_SP);
    Value val2(true, object->getOffset());
    m_ir.emit(IR_ADD, val1, val2, val1);
   
    // call constructor for the object
    // Type * type = getType(stmt.m_var->m_name);
    // Assert(type->constructor != NULL);
    // callObjectMethod(object, "constructor");

    // if the declared variable is initialized
    Value result(true);
    if (stmt.m_expr) {
        build(stmt.m_expr);
        result = stmt.m_expr->m_value;
      //  callObjectMethod(object, "assign", result); 
    }
    
    // load the result into locals
   m_ir.emit(IR_LOAD, val1, result);
}

///  IRBuilder handler for if statement
void 
IRBuilder::accept(ASTIfStmt &stmt) 
{
    Assert(stmt.m_conditExpr != NULL);
    
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    
    // check wether there is only a if block
    if (stmt.m_elseBlockStmt == NULL) {
        build(stmt.m_conditExpr);
        if (!stmt.m_conditExpr->hasValidValue())
            return;
        Value value1(true);
        Value value2(false, 1); 
        m_ir.emit(IR_STORE, value1, stmt.m_conditExpr->m_value);
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
        m_ir.emit(IR_STORE, value1, stmt.m_conditExpr->m_value);
        m_ir.emitCMP(value1, value2, label1, label3);
        m_ir.emitLabel(label1);
        build(stmt.m_ifBlockStmt);
        
        m_ir.emitLabel(label3);
        build(stmt.m_elseBlockStmt);
    }
    
    m_ir.emitLabel(label2);
}

///  IRBuilder handler for while statement
void 
IRBuilder::accept(ASTWhileStmt &stmt) 
{ 
    Assert(stmt.m_conditExpr != NULL);
    
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
    m_ir.emit(IR_STORE, value1, stmt.m_conditExpr->m_value);
    m_ir.emitCMP(value1, value2, label2, label3);
    m_ir.emitLabel(label2);
    
    build(stmt.m_stmt);
    m_ir.emitJump(label1);
    m_ir.emitLabel(label3);
    
    // popup the current iterable statement
    popIterablePoint();
}

///  IRBuilder handler for do while statement
void 
IRBuilder::accept(ASTDoStmt &stmt) 
{
    Assert(stmt.m_conditExpr != NULL);
        
    pushIterablePoint( &stmt);

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
    m_ir.emit(IR_STORE, value1, stmt.m_conditExpr->m_value);
    m_ir.emitCMP(value1, value2, label1, label2);
    m_ir.emitLabel(label2);
    // popup the current iterable statement
    popIterablePoint();
}

///  IRBuilder handler for for statement
void 
IRBuilder::accept(ASTForStmt &stmt) 
{
    // now only support normal loop mode
    // for (expr1; expr2; exprlist) statements
    build(stmt.m_initializer);
    // push iterable statement into frame
    pushIterablePoint(&stmt);

    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Label label3 = Label::newLabel();
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);

    m_ir.emitLabel(label1);
        
    if (stmt.m_conditExpr) {
        build(stmt.m_conditExpr);
        Value value1(true);
        Value value2(false, 1);
        m_ir.emit(IR_STORE, value1, stmt.m_conditExpr->m_value);
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
void 
IRBuilder::accept(ASTForeachStmt &stmt) 
{
    // push iterable statement into frame
    pushIterablePoint(&stmt);

    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Label label3 = Label::newLabel();
    // for continue statement, set iterable start and end point
    stmt.setIterableStartPoint(label1);
    stmt.setIterableEndPoint(label3);
    
    Object *object = NULL;
    Type *type = NULL;

    Value indexValue(true); 
    m_ir.emitLabel(label1);
    switch (stmt.m_objectSetType) {
        case ASTForeachStmt::Object:
            object = getObject(stmt.m_objectSetName);
            type = getType(stmt.m_objectSetName);
            Assert(object != NULL);
            Assert(type != NULL);
            if (type  && isType(type, "set")) {
                Assert(stmt.m_varNumbers == 1);
                // get object element size
                vector<Value> arguments; 
                Value objectSelf(false, object->getOffset());
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
                Value val(true, object->getOffset());
                m_ir.emit(IR_LOAD, val, element);
                build(stmt.m_stmt); 
                // increase the index value
                m_ir.emit(IR_INC, indexValue);
                // jump to the loop start 
                m_ir.emitJump(label2); 
            }
            else if (type  && isType(type, "map")) {
                Assert(stmt.m_varNumbers == 2);
                // get object element size by calling enumerableObject::size() method
                vector<Value> arguments;
                Value objectSelf(false, object->getOffset());
                arguments.push_back(objectSelf);
                Value elementSize(true);
                string methodName = "size";
                callObjectMethod(stmt.m_objectSetName, methodName, arguments, elementSize);
                // compare the indexValue with elementSize
                m_ir.emitCMP(elementSize, indexValue, label2, label3);
                m_ir.emitLabel(label2);
                
                // get the indexed element
                Object *keyObject = getObject(stmt.m_id[0]);
                Object *valObject = getObject(stmt.m_id[1]);
                Value key(true, keyObject->getOffset());
                Value val(true, valObject->getOffset());
                
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
        case ASTForeachStmt::MapObject:
            break;
        case ASTForeachStmt::SetObject:
            break;
        default:
            break;
    }
    m_ir.emitJump(label1);
    m_ir.emitLabel(label3);
}

///  IRBuilder handler for switch statement
void 
IRBuilder::accept(ASTSwitchStmt &stmt) 
{
    // check parameter's validity
    Assert(stmt.m_conditExpr != NULL);
    
    // condition judge statement
    build(stmt.m_conditExpr);
    if (!stmt.m_conditExpr->hasValidValue())
        return;
    Value value1(true);
    m_ir.emit(IR_STORE, value1, stmt.m_conditExpr->m_value);
    Label label2 = Label::newLabel();
    
    // generate case statement
    vector< pair<vector<ASTExpr *>, ASTStatement *> >::iterator ite = stmt.m_cases.begin();
    for (; ite != stmt.m_cases.end(); ite++)  {
        //
        pair<vector<ASTExpr *>, ASTStatement *> &pt = *ite;
        vector<ASTExpr *> exprList = pt.first;
        ASTStatement *sts = pt.second;
        
        // case n
        // case n+1
        Label label1 = Label::newLabel();
        vector<ASTExpr *>::iterator ite = exprList.begin();
        for (; ite != exprList.end(); ite++) {
            // get case value
            ASTExpr *expr = *ite;
            build(expr);
            if (!expr->hasValidValue())
                continue;
            Value value2(true);
            m_ir.emit(IR_STORE, value2, expr->m_value);
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

///  IRBuilder handler for continue statement
void 
IRBuilder::accept(ASTContinueStmt &stmt) 
{
    ASTStatement *startStmt = getCurIterablePoint();
    if (startStmt  && startStmt->isIterable())
        m_ir.emitJump(startStmt->getIterableStartPoint());
}

///  IRBuilder handler for break statement
void 
IRBuilder::accept(ASTBreakStmt &stmt) 
{
    ASTStatement *startStmt = getCurIterablePoint();
    if (startStmt  && startStmt->isIterable())
        m_ir.emitJump(startStmt->getIterableEndPoint());
}


///  IRBuilder handler for return statement
void 
IRBuilder::accept(ASTReturnStmt &stmt) 
{
    Value ret(IR_R0);
    if (stmt.m_resultExpr) {
        build(stmt.m_resultExpr);
        if (stmt.m_resultExpr->hasValidValue()) {
            // store the result
            m_ir.emit(IR_LOAD, ret, stmt.m_resultExpr->m_value);
        }
        else {
            Value result(false, 0);
            m_ir.emit(IR_LOAD, ret, result); 
        }
    }
    m_ir.emit(IR_RET);
}

///  IRBuilder handler for assert statement
void 
IRBuilder::accept(ASTAssertStmt &stmt) 
{
    Assert(stmt.m_resultExpr != NULL);
    
    build(stmt.m_resultExpr);
    if (!stmt.m_resultExpr->hasValidValue())
        return;
    
    // judge the expression 
    Value local(true);
    m_ir.emit(IR_STORE, local, stmt.m_resultExpr->m_value);
    
    // generate the jump labe
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    m_ir.emitCMP(local, 1, label1, label2);
    
    m_ir.emitLabel(label2);
    m_ir.emitException();
    m_ir.emitLabel(label1);
}

///  IRBuilder handler for throw statement
/// the exception handler should be implement by method type
/// throw 'expression'
void 
IRBuilder::accept(ASTThrowStmt &stmt) 
{
    
}

///  IRBuilder handler for try statement
/// 'try' blockStatement ((catchPart+finallyPart)?|finallyPart)
void 
IRBuilder::accept(ASTTryStmt &stmt) 
{
    
}
///  IRBuilder handler for catch statement
void 
IRBuilder::accept(ASTCatchStmt &stmt) 
{
    
}

///  IRBuilder handler for finally catch satement
void 
IRBuilder::accept(ASTFinallyCatchStmt &stmt) 
{
    
}

void 
IRBuilder::accept(ASTExprStmt &stmt)
{

}
// expression
void 
IRBuilder::accept(ASTExpr &expr)
{
}
void IRBuilder::accept(ASTExprList &exprList)
{
    vector<ASTExpr *>::iterator ite = exprList.m_exprs.begin();
    for (; ite != exprList.m_exprs.end(); ite++) {
        ASTExpr *expr = *ite;
        build(expr);
    }
    
}
///  IRBuilder handler for AssignmentExpr
void 
IRBuilder::accept(ASTAssignmentExpr &expr) 
{
    Assert(expr.m_left != NULL);
    Assert(expr.m_right != NULL);
    
    build(expr.m_left);
    if (!expr.m_left->hasValidValue())
        return;
    Value left(true);
    m_ir.emit(IR_LOAD, left, expr.m_left->m_value);
    
    build(expr.m_right);
    if (!expr.m_left->hasValidValue()) {
        return;
    }
    Value right(true);
    m_ir.emit(IR_LOAD, right, expr.m_right->m_value);
    
    // convert the binary operatiorn
    int op = IR_INVALID;
    switch (expr.m_op) {
        case ASTAssignmentExpr::BOP_ADD:
            op = IR_ADD;
            break;
        case ASTAssignmentExpr::BOP_SUB:
            op = IR_SUB;
            break;
        case ASTAssignmentExpr::BOP_MUL:
            op = IR_MUL;
            break;
        case ASTAssignmentExpr::BOP_DIV:
            op = IR_DIV;
            break;
            
        default:
        // others operation will be added here
        break;
    }
    
    if (op != IR_INVALID) 
        m_ir.emit(op, left, right, left);
    expr.m_value = left;
}

///  IRBuilder handler for ConditionalExpr
void 
IRBuilder::accept(ASTConditionalExpr &expr) 
{

}

///  IRBuilder handler for LogicOrExpr
void 
IRBuilder::accept(ASTLogicOrExpr &expr)
{
    Assert(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;

    Value value1(true);
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    m_ir.emitCMP(value1, 1, label1, label2);
    m_ir.emitLabel(label2);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        m_ir.emit(IR_LOAD, value2, element->m_value);
        m_ir.emit(IR_OR, value1, value2, value1);
        
        Label label3 = Label::newLabel();
        m_ir.emitCMP(value1, 1, label1, label3);
        m_ir.emitLabel(label3);
    }
    
    // store the result into expr.result
    expr.m_value = value1;
    m_ir.emitLabel(label1);
}

///  IRBilder handler for LogicAndExpr
void 
IRBuilder::accept(ASTLogicAndExpr &expr)
{
    Assert(expr.m_target != NULL);
   
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
    Value value1(true);
    
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    m_ir.emitCMP(value1, 1, label1, label2);
    m_ir.emitLabel(label2);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        Label label3 = Label::newLabel();
        m_ir.emit(IR_LOAD, value2, element->m_value);
        m_ir.emit(IR_AND, value1, value2, value1);
        
        m_ir.emitCMP(value1, 1, label3, label1);
        m_ir.emitLabel(label3);
    }
    
    // store the result into expr.result
    expr.m_value = value1;
    m_ir.emitLabel(label1);
}


///  IRBilder handler for bitwise or expression
// BitwiseOrExpr : BitwiseXorExpr ( '|' bitwiseXorExpr) *
void 
IRBuilder::accept(ASTBitwiseOrExpr &expr) 
{
    Assert(expr.m_target != NULL);
   
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        m_ir.emit(IR_LOAD, value2, element->m_value);
        m_ir.emit(IR_BIT_OR, value1, value2, value1);
    }
    
    expr.m_value = value1; 
}

///  IRBuilder handler for bitwise xor expression
/// BitwiseXorExpr : BitwiseAndExpr ('^' bitwiseAndExpr) *
void 
IRBuilder::accept(ASTBitwiseXorExpr &expr) 
{
    Assert(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        Value value2(true);
        m_ir.emit(IR_STORE, value2, element->m_value);
        m_ir.emit(IR_BIT_XOR, value1, value2, value1);
    }
    expr.m_value = value1;
}

///  IRBuilder handler for bitwise and expression
/// BitwiseAndExpr : EqualityExpr (' &' EqualilityExpr) *
void 
IRBuilder::accept(ASTBitwiseAndExpr &expr) 
{
    Assert(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        Value value2(true);
        m_ir.emit(IR_STORE, value2, element->m_value);
        m_ir.emit(IR_BIT_AND, value1, value2, value1);
    }
    expr.m_value = value1;
}

///  IRBuilder handler for equality Expr
/// EqualityExpr : RelationalExpr (('==' | '!=') RelationalExpr) *
void 
IRBuilder::accept(ASTEqualityExpr &expr) 
{
    Assert(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == ASTEqualityExpr::OP_EQ)
            irt = IR_EQ;
        else if (expr.m_op == ASTEqualityExpr::OP_NEQ)
            irt = IR_NEQ;
        
        if (irt != IR_INVALID) { 
            Value value2(true);
            m_ir.emit(IR_STORE, value2, element->m_value);
            m_ir.emit(irt, value1, value2, value1);
        }
    }
    
    expr.m_value = value1;
}

///  IRBuilder handler ffor relational expression
/// RelationalExpr :
///  ShiftExpr (('>' | '<' | '>=' | '<=') ShiftExpr) *
void 
IRBuilder::accept(ASTRelationalExpr &expr) 
{
    Assert(expr.m_target != NULL);
    
    // build expression and get result into local
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    
    vector<ASTExpr *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        switch (expr.m_op) {
            case  ASTRelationalExpr::OP_LT:
                irt = IR_LT;
                break;
            case ASTRelationalExpr::OP_LTEQ:
                irt = IR_LTEQ;
                break;
            case ASTRelationalExpr::OP_GT:
                irt = IR_GT;
                break;
            case ASTRelationalExpr::OP_GTEQ:
                irt = IR_GTEQ;
                break;
            default:
                irt = IR_INVALID;
                break;
        }
        
        if (irt != IR_INVALID) {
            Value value2(true);
            m_ir.emit(IR_STORE, value2, element->m_value);
            m_ir.emit(irt, value1, value2, value1);
        }
    }
    expr.m_value = value1;
}

///  IRBuilder handler for shift expression
/// ShiftExpr : AdditiveExpr (('>>'|'<<') AdditiveExpr) *
void 
IRBuilder::accept(ASTShiftExpr &expr) 
{
    Assert(expr.m_target != NULL);

    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emit(IR_STORE, value1, expr.m_value);
    
    vector<ASTExpr * >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr * element = * ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == ASTShiftExpr::OP_LSHIFT)
            irt = IR_LSHIFT;
        else if (expr.m_op == ASTShiftExpr::OP_RSHIFT)
            irt = IR_RSHIFT;
        
        if (irt != IR_INVALID) {
            Value value2(true);
            m_ir.emit(IR_STORE, value2, element->m_value);
            m_ir.emit(irt, value1, value2, value1);
        }
    }
    expr.m_value = value1;
}

///  IRBuilder for multipicative expression
/// AdditiveExpr :
///  MultiplicativeExpr (('+' | '-') MultiplicativeExpr
void 
IRBuilder::accept(ASTAdditiveExpr &expr) 
{
    Assert(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
    
    vector<ASTExpr * >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr * element = * ite;
        build(element);
        if (!element->hasValidValue())
            continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == ASTAdditiveExpr::OP_ADD)
            irt = IR_ADD;
        else if (expr.m_op == ASTAdditiveExpr::OP_SUB)
            irt = IR_SUB;
        
        if (irt != IR_INVALID) {
            Value value2(true);
            m_ir.emit(IR_STORE, value2, element->m_value);
            m_ir.emit(irt, value1, value2, value1);
        }
    }
    expr.m_value = value1;
}


///  IRBuilder handler for multiplicative expression
/// MultiplicativeExpr :
/// UnaryExpresion ((' *' | '/' | '%') UnaryExpr) *
void 
IRBuilder::accept(ASTMultiplicativeExpr &expr) 
{
    Assert(expr.m_target != NULL);
    
    build(expr.m_target);
    if (!expr.m_target->hasValidValue())
        return;
    Value value1(true);
    m_ir.emit(IR_LOAD, value1, expr.m_target->m_value);
  
    vector<ASTExpr * >::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        ASTExpr * element = * ite; 
        build(element);
        if (!element->hasValidValue())
            continue;
    
        int irt = IR_INVALID;
        if (expr.m_op == ASTMultiplicativeExpr::OP_MUL)
            irt = IR_MUL;
        else if (expr.m_op == ASTMultiplicativeExpr::OP_DIV)
            irt = IR_DIV;
        else if (expr.m_op == ASTMultiplicativeExpr::OP_MODULO)
            irt = IR_MODULO;
        
        if (irt != IR_INVALID) {
            Value value2(true);
            m_ir.emit(IR_STORE, value2, element->m_value);
            m_ir.emit(irt, value1, value2, value1);
        }
        
    }
    expr.m_value = value1;
}

///  IRBuilder handler for unary expression
void 
IRBuilder::accept(ASTUnaryExpr &expr) 
{

    // the min and key expression, 
    // it must tell other expression wether it has valid value and the value
    Assert( expr.m_primary != NULL);
    Value * value = NULL;
    
    // first, process the basic unary expression
    switch (expr.m_primary->m_type) {
        case ASTPrimaryExpr::T_TRUE:
            value = new Value(false, 1);
            break;
        case ASTPrimaryExpr::T_FALSE:
            value = new Value(false, 0);
            break;
        case ASTPrimaryExpr::T_HEX_NUMBER:
            value = new Value(false, 0);
            break;
        case ASTPrimaryExpr::T_NULL:
            value = new Value(false, 0);
            break;
        case ASTPrimaryExpr::T_STRING:
            value = new Value(false, 0);
            break;
        case ASTPrimaryExpr::T_NUMBER:
            value = new Value(false, 0);
            break;
    }
    if (value) {
        expr.m_value = * value;
        delete value;
        return;
    }
    
    switch (expr.m_primary->m_type) {
        case ASTPrimaryExpr::T_SELF:
        case ASTPrimaryExpr::T_SUPER:
        case ASTPrimaryExpr::T_IDENTIFIER: 
            value = handleSelectorExpr( *expr.m_primary, expr.m_selectors);
            if (value) { 
                expr.m_value = * value;
                delete value;
            }
            break;
        case ASTPrimaryExpr::T_COMPOUND:
           build(expr.m_primary);
           expr.m_value = expr.m_primary->m_value;
           break;
        default:
           Error::complain(expr, "Unknow primary expression\n");
           break;
    }
}

Value*  
IRBuilder::handleSelectorExpr(
        ASTPrimaryExpr &primExpr, 
        vector<ASTSelectorExpr *> &elements)
{
    // only handle the following primaryExpression
    if (primExpr.m_type != ASTPrimaryExpr::T_IDENTIFIER ||
        primExpr.m_type != ASTPrimaryExpr::T_SELF ||
        primExpr.m_type != ASTPrimaryExpr::T_SUPER )
            return NULL;
    // get Object infor about the primExpr
    Object * Object = getObject(primExpr.m_text);
    Assert(Object != NULL);

    // load the Object address into register
    Value base(true);
    Value offset(false, Object->getOffset());
    m_ir.emit(IR_LOAD, base, offset);
    base = offset; 

    // get type of current primary test
    Type * type = getType(primExpr.m_text);
    string curText = primExpr.m_text;
    // for each selector
    vector<ASTSelectorExpr * >::iterator ite = elements.begin();
    for (; ite != elements.end(); ite++) {
        ASTSelectorExpr * selector = dynamic_cast<ASTSelectorExpr * >( *ite);
        Assert(selector != NULL);

        if (selector->m_type == ASTSelectorExpr::DOT_SELECTOR) {
            type = type->getSlot(selector->m_id);
            // adjust the offset
            Assert(type != NULL);
            Value val(false, type->getSize());
            m_ir.emit(IR_ADD, base, val, base);

            curText = selector->m_id;
        }

        else if (selector->m_type == ASTSelectorExpr::ARRAY_SELECTOR) {
            // if the selector is array, the subscriptor must be known
            Assert(selector->m_arrayExpr != NULL);
            // build the arrary subscrip expression to get index 
            build(selector->m_arrayExpr);
            // get the element type
            type = type->getSlot(0);
            Assert(type != NULL);

            Value val1(true);
            m_ir.emit(IR_LOAD, val1, selector->m_arrayExpr->m_value);
            // till now, the array index is loaded into val1
            Value val2(true, type->getSize());
            m_ir.emit(IR_MUL, val1, val2, val1);
            // now, the target element offset is loaded into val1
            // update the base
            m_ir.emit(IR_ADD, val1, base, base);
        }
        else if (selector->m_type == ASTSelectorExpr::METHOD_SELECTOR) {
            MethodType * methodType = (MethodType * )getType(curText);
            Assert(methodType != NULL);
            // if the method is member of class, the first parameter should be 
            // ref of class object which will be stored as the first parameter 
            if (methodType->isOfClassMember()) {
                Value val(true); 
                m_ir.emit(IR_LOAD, val, base);
                m_ir.emit(IR_PUSH, val); 
            }
            ASTMethodCallExpr * methodCallExpr = selector->m_methodCallExpr;
            build(methodCallExpr);
            type = methodType->getReturnType();

        }
        else
            Error::complain(primExpr, "Unknow selector\n");
    }
    return NULL;
}


///  IRBuilder handler for primary expression
void 
IRBuilder::accept(ASTPrimaryExpr &expr) 
{
    // do nothig    
}

void 
IRBuilder::accept(ASTSelectorExpr &expr)
{
   // do nothing now 
}

void 
IRBuilder::accept(ASTMethodCallExpr &expr) 
{
   vector<ASTExpr * >::iterator ite = expr.m_arguments.end();
   for (; ite != expr.m_arguments.begin(); ite--) {
        ASTExpr *argument = *ite;
        build(argument);
        // the argument shoud be push into stack
        Value val(true);
        m_ir.emit(IR_LOAD, val, argument->m_value);
        m_ir.emit(IR_PUSH, val);
   }
   string &methodName = expr.getMethodName();
   Object * Object = getObject(methodName);
   Assert(Object != NULL);

   Value methodAddr(true, Object->getOffset());
   m_ir.emitMethodCall(methodAddr);
}

// new
void 
IRBuilder::accept(ASTNewExpr &expr)
{
    
}

