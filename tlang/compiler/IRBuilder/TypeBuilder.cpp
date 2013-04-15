//
//  TypeChecker.cpp
//  A toyable language compiler (like a simple c++)


#include "compiler/Error.h"
#include "compiler/OSWraper.h"
#include "compiler/Frame.h"
#include "compiler/Struct.h"
#include "compiler/Variable.h"
#include "compiler/Class.h"
#include "compiler/Function.h"
#include "compiler/Expression.h"
#include "compiler/Statement.h"
#include "compiler/TypeBuilder.h"

/// @brief Constructor
TypeBuilder::TypeBuilder() {
    string name = "Global";
    enterScope(name, NULL);
}

/// @brief Destructor
TypeBuilder::~TypeBuilder() {
    exitScope();
    if (m_curScope != m_rootScope) {
       // throw exception;
    }
    
}

/// @brief Enter a new scope
void TypeBuilder::enterScope(const string &name, Scope *scope) {
    if (m_curScope) {
        scope->setParentScope(m_curScope);
        m_curScope = scope;
    } 
    m_curScopeName = name;
    if (!m_rootScope)
        m_rootScope = newScope;
}
/// @brief Exit the current scope
void TypeBuilder::exitScope() {
    if (m_curScope != NULL)
        m_curScope = m_curScope->getParent();
}

/// @brief Check to see wether the symbol specified by name exist
bool TypeBuilder::hasSymbol(const string &name, bool nested) {
    bool result = false;
    if (m_curScope && m_curScope->resolveSymbol(name, nested))
        result = true;

    return result;
}

/// @brief Check to see wether the type specified by name exist
bool TypeBuilder::hasType(const string &name, bool nested) {
    bool result = false;
    if (m_curScope && m_curScope->resolveType(name, nested))
        result = true;
    
    return result;
    
}

/// @brief Get symbol by name 
Symbol* TypeBBuilder::getSymbol(const string &name, bool nested) {
    Symbol *symbol = NULL;
    if (m_curScope!= NULL)
        symbol = m_curScope->resolveSymbol(name, nested);
    
    return symbol;
}

/// @brief Get type by name
Type* TypeBBuilder::getType(const string &name, bool nested) {
    Type *type = NULL;
    if (m_curScope != NULL)
        type = m_curScope->resolveType(name, nested);
    
    return type;
}

/// @brief Define a new symbo in current scope
void TypeBuilder::defineSymbol(Symbol *symbol) {
    if (symbol && m_curScope) {
        m_curScope->defineSymbol(symbol);
    }
}

/// @brief Define a new type in current scope
void TypeBBuilder::defineType(Type *type){
    if (type && m_curScope)
        m_curScope->defineType(type);
}

/// @brief helper function to walk ast node
void TypeBuilder::walk(AST *node) {
    if (node)
        node->walk(this);
}

/// @brief Typebuilder handler for type specifier
void TypeBuilder::accept(TypeSpec &type) {
    Type *type = getType(typeSpec.m_name);
    if (!type) 
        Error::complain("the type is not declared\n", typeSpec.m_name.c_str());        
}

/// @brief TypeBuilder handler for Struct 
void TypeBuilder::accept(Struct &st) {
	// check to see wether the struct has beeb defined
    if (hasType(st.m_name), true) {
        Error::complain("the struct name %s has been used\n", st.m_name.c_str());
    }
	
	// generate a new struct type in currrent scope
    StructType *pstType = new StructType();
    defineType(pstType);

	// the struct is also a scope iteself
	enterScope(st.getName(), dynamic_cast<Scope*>(&st));
    
    // check to see wether the member's type exist
    vector<Struct::Member>::iterator ite;
    for (ite = st.m_members.begin(); ite < st.m_members.end(); ite++) {
        Struct::Member member = *ite;
        TypeSpec *typeSpec = member->first;
        walk(typeSpec);
        string name = member->second;
        
        // check to see wether there are same members  
        vector<Struct::Member>::iterator ip;
        for (ip = st.m_members.begin(); ip < st.m_members.end(); ip++) {
            if (ip != ite  && name == ip->second) {
                Error::complain("there are same identifier %s in struct %s", name.c_str(),
            }
        }
        pstType->addSlot(name, getType(typeSpec->m_name));
    }
    exitScope();

}

/// @brief TypeBuilder handler for Variable
void TypeBuilder::accept(Variable &var) {
    Type *type = NULL;
    bool isvalid = true;
    
    // check to see wether the type of var is right
    if (var.m_typeSpec == NULL) {
        Error::complain("the type of variable is not declared\n", var.m_name.c_str());
        isvalid = false;
    }
    else if ((type = getType(var.m_typeSpec->m_name))) == NULL) {
        Error::complain("the type %s is not declared\n", var.m_typeSpec->m_name.c_str())
        isvalid = false;    
    }
    
    // check to see wether the variable exist
    if (hasSymbol(var.m_name)) {
        Error::complain("the variable %s is already declared\n", var.m_name.c_str());
        isvalid = false;
    }
    
    if (var.m_isGlobal) {
        // if he variable is global and it is initialized
        // the initialization express must be directy evaluated,
        // at the same time , we should check wether the initialization expression's
        // type is compatible withe the variable's type
        if (var.m_isInitialized && var.m_expr) {
            walk(var.m_expr);
            // check to see wether the val is const
            if (!var.m_expr->m_value.isConst()) {
                Error::complain("the global variable %s is initialized with non const value\n", 
                        var.m_name.c_str());
            }
            else if (!type->isCompatibleWith(var.m_expr->m_value.m_type)) {
                Error::complain("the global variable %s is initialized with no right type\n"),
                var.m_name.c_str());
            }
            else
                var.m_initializedVal = var.m_expr->m_value;
        }
    }
    
    
    // if the variable is class variable
    else if (var.m_isOfClass) {
        // get class from the current scope
        if (hasType(var.m_type)) {
            if (var.m_isInitialized && var.m_expr) {
                walk(var.m_expr);
                // check to see wether the val is const
                if (!var.m_expr->m_value.isConst()) {
                    Error::complain("the class variable %s is initialized with non const value\n", 
                            var.m_name.c_str());
                }
                else if (!type->isCompatibleWith(var.m_expr->m_value.m_type)) {
                    Error::complain("the class variable %s is initialized with no right type\n"),
                    var.m_name.c_str());
                }
                else
                    var.m_initializedVal = var.m_expr->m_value;
            }
        }
    }
    // local variable
    else {
        if (var.m_expr) {
            walk(var.m_expr);
            // check to see wether the val is const
            if (!var.m_expr->m_value.isConst()) {
                Error::complain("the local variable %s is initialized with non const value\n", 
                        var.m_name.c_str());
            }
            else if (!type->isCompatibleWith(var.m_expr->m_value.m_type)) {
                Error::complain("the local variable %s is initialized with no right type\n",
                        var.m_name.c_str());
            }
            else          
                Value *result = &var.m_expr->m_value;
        }
    }
    // define symbol in current scope 
    Symbol *symbol = new Symbol();
    symbol->m_name = var.m_name;
    symbol->m_type = type;
    defineSymbol(symbol);
}

/// @brief Handler for function type builder
void TypeBuilder::accept(Function &function) {
    bool isvalid = true;
    Type *returnType = NULL;

    // check to see wether the return type of function is declared
    if (!function.m_retTypeSpec) {
        Error::complain("the function type is not declared\n");
        isvalid = false;
    }
    else if ((returnType = getType(function.m_retTypeSpec->m_name)) == NULL) {
        Error::complain("the function type %s is not declared\n", returnType->m_name.c_str());
        isvalid = false;
    }
    /// check to see wether the function name has been declared
    FunctionType *funcType = (FunctionType *)getType(function.m_name, true);
    if (funcType) {
        Error::complain("Function %s is already declared\n", function.m_name.c_str());
        isvalid = false;
    }
    
	// set the current scope
	enterScope(function.m_name, dynamic_cast<Scope*>(&function));
	
	// check the return type
    if (!hasType(function.m_returnType)) {
        Error::complain("the return type %s is not defined\n", function.m_returnType.c_str());
        isvalid = false;
        
    }
    
    // check wether the function name exist
    if (hasSymbol(function.m_name)) {
        Error::complain("the function name %s already exist", function.m_name.c_str());
        isvalid = false;
    }
    
    
    // if the function is a member of class or interface,
    // the function must be in VTBL of the class and interface
    if (function.m_isOfClass || function.m_isOfProtocol) {
        // check to see wether there is the function in VTBL
        ClassType *clsType = (ClassType *)getType(function.m_class);
        if (!clsType) {
            Error::complain("the function %s is not member of class %s\n", 
                    function.m_name.c_str(), function.m_class.c_str());
            isvalid = false;
        }
        
        // get VTBL of the class
        ObjectVirtualTable *vtbl = clsType->getVirtualTable();
        if (!vtbl) {
            Error::complain("The class %s has not VTBL\n", clsType->getName().c_str());
            isvalid = false;
        }
        
        // check to see wether the VTBL have the function
        FunctionType *type = (FunctionType*)vtbl->getSlot(function.m_name);
        if (!type) {
            Error::complain("the class %s has not the function %s\n",
                             clsType->getName().c_str(),
                             function.m_name.c_str());
            isvalid = false;
        }
    }
    
    if (isvalid) {
        // define function tye in current scope
        FunctionType *funcType = new FunctionType();
        funcType->setName(function.m_name);
        defineType(funcType);
        
        // define function symbol in current scope
        Symbol *symbol = new Symbol();
        symbol->m_type = funcType;
        symbol->m_name = function.m_name;
        defineSymbol(symbol);;
        
        // if the function is member of class
        if (function.m_isOfClass) {
            ClassType *clsType = (ClassType *)getType(function.m_class);
            clsType->addSlot(function.m_name, funcType);
        }
        
        // if the function is member of interface
        else if (function.m_isOfProtocol) {
            InterfaceType *protocolType = (ProtocolType *)getType(function.m_protocol);
            protocolype->addSlot(function.m_name, funcType);
        }
    }
    
    // check the function parameter list
    walk(function.m_paraList);
    // check the function block
    walk(function.m_block);
    // exit the function scope
	exitScope();
    
}

/// @brief Handler for FunctionParameterList type builder
void TypeBuilder::accept(FunctionParameterList &list) {
    vector<FunctionParameter *>::iterator ite =list.m_parameters.begin();
    for (ite != list.m_parameters.end(); ite++) {
       
        // check the parameter
        FunctionParameter *functionParameter = *ite;
        functionParameter->walk(this);
        
        // check wether there are same variable's name
        vector<FunctionParameter *>::iterator ip;
        for (ip = list.m_parameters.begin(); ip != list.m_parameters.end(); ip++) {
            FunctionParameter *second = *ip;
            if (ite != ip && functionParameter->m_name == second->m_name) {
                Error::complain("there are same variable's name %s\n", 
                                second->m_name.c_str());
            }
        }
    }
}

/// @brief Handler for FunctionParameter type builder
void TypeBuilder::accept(FunctionParameter &para) {
    bool isvalid = true;
  
    // check the parameter's type
    if (!getType(para.m_type)) {
        Error::complain("the parameter's type %s is not declared\n", 
                para.m_type.c_str());
        isvalid = false;
    }
    // check the parameter's name
    if (getSymbol(para.m_name)) {
        Error::complain("the parameter %s is already declared in current scope\n", 
                para.m_name.c_str());
        isvalid = false;
    }
    
    // if the parameter has default value, 
    // check wethere the expression's type is same with variable's type
    if (para.m_hasDefault && para.m_default != NULL) {
        Type * type = getType(para.m_type);
        if (type && !type->isCompatibleWith(para.m_default->m_type)) {
            Error::complain("the parameter %s is not rightly initialized\n",
                            para.m_name.c_str());
        }
    }
    // define the passed parameter in current symbol talbe
    Symbol *symbol = new Symbol();
    symbol->m_name = para.m_name;
    symbol->m_type = para.m_type;
    // if the function is called, all parameters are pushed by caller
    // so the address of each parameter must be knowned
    symbol->m_storage = LocalStackSymbol;
    symbol->m_addr = para.m_index * 4;  // the index is offset 
    defineSymbol(symbol);
    
}

/// @brief TypeBuilder handler for FunctionBlock
void TypeBuilder::accept(FunctionBlock &block) {
    vector<Statement *>::iterator ite;
    for (ite = block.m_stmts.begin(); ite != block.m_stmts.end(); ite++) 
        walk(*ite);
}


/// @brief TypeBuilder handler for Class
void TypeBuilder::accep(Class &cls) {
    bool isvalid = true;
    // check wether the class name exist?
	bool nested = (cls.m_isPublic == true)? true:false;
    if (hasSymbol(cls.m_name, nested)) {
        Error::complain("the class name %s is already defined\n", cls.m_name.c_str());
		isvalid = false;
    }
	// the class is also scope
	enterScope(cls.m_name, dynamic_cast<Scope*>(&cls));
	
    // put the class Type int the current scope
    ClassType *clsType = new ClassType(cls.m_name, m_curScope, cls.m_isPublic);
    defineType(clsType);
        
    // puth the class symbo in the current scope
    Symbol *symbol = new Symbol();
    symbol->m_type = clsType;
    symbol->m_name = cls.m_name;
    defineSymbol(symbol);
    
    // check wether the base class exist
    vector<string>::iterator ite;
    for (ite = cls.m_base.begin(); ite != cls.m_base.end(); ite++) {
        string baseClass = *ite;
        if (baseClass == cls.m_name)
            Error::complain("the base class %s can not be same with class %s\n",
                        baseClass.c_str(), cls.c_str());                    
        if (!getType(baseClass))
            Error::complain("the class  %s is not declared\n", baseClass.c_str());
    }   
    
    // check to see wether the class implements protocol exist
    for (ite = cls.m_protocols.begin(); ite != cls.m_protocols.end(); ite++) {
        string protocolName = *ite;
        if (protocolName == cls.m_name) {
            Error::complain("the protocol name can not be same  withe class %s\n",
                    protocolName.c_str(), cls.c_str());
        }
        // the methd exported by protocol must be implemented in class
        ProtocolType *protocolType = getType(protocolName);
        if (!protocolType) 
            Error::complain("the protocol %s is not declared\n", protocolName.c_str());
        for (int index = 0; index < protoclType->getSlotCount(); index++) {
            // for each slot in protocol, to check wether it is in class
            Type *slot = protocolType->getSlot(index);
            if (!cls.getFunction(slot->m_name)) {
                Error::complain("the method %s exported by protocol %s is not implemented in class %s",
                            slot->m_name.c_str(), protocolName.c_str(), cls.m_name.c_str());
            }
        }  
    }
    
    // walk through the class block
    build(cls.m_block);
    exitScope(); 
}

/// @brief TypeBuilder handler for ClassBlock
void TypeBuilder::accept(ClassBlock &block) {
    // iterate all variables
    vector<Variable*>::iterator varIteator = block.m_vars.begin();
    while(varIteator != block.m_vars.end()) {
        walk(*varIteator);
        varIteator++;
    }
   // iterate all functions
    vector<Function*>::iterator funcIteator = block.m_functions.begin();
    while (funcIteator != block.m_functions.end()) {
        walk(*funcIteator);
        funcIteator++;
    }
}

/// @brief TypeBuildef handler for Interface
void TypeBuilder::accept(Protocol &protocol) {
	// check wether the protocol is alread declared
    if (hasSymbol(protocol.getName())) {
        Error::complain("the protocol name %s is already declared\n", 
                        protocol.m_name.c_str());
    }
	// protocol is also a scope
	enterScope(protocol.m_name, dynamic_cast<Scope*>(&protocol));
	
    // put the interface type in the current scope
    ProtocolType *protocolType = new ProtocolType(protocol.m_name, m_curScope, protocol.m_isPublic);
    defineType(protocolType);
        
    // put the interface symbol in the current scope
    Symbol *symbol = new Symbol();
    symbol->m_name = protocol.m_name;
    symbol->m_type = protocolType;
    defineSymbol(symbol);
    
    // iterall all functions of the protocol
    vector<Function*>::iterator ite = protocol.m_functions.begin();
    for (; ite != protocol.m_functions.end(); ite++) {
        Function *func = *ite;
        if (func->m_name != protocol.m_name) {
            Error::complain("the function %s is not member of %s\n", 
                    func->m_name.c_str(), protocol.m_name.c_str());
        }
        walk(func);
    }
    // exit the protocol scope
    exitScope();
    
}

/// @brief TypeBuilder handler for Statement
void TypeBuilder::accept(Statement &stmt) {
}

/// @brief TypeBuilder handler for import statement
void TypeBuilder::accept(ImportStatement &stmt) {
    string fullPath = "";
    OS::getWorkPath(fullPath);
    
    // check to check wether the imported package exist
    for (vector<string>::size_type index = 0; 
         index < stmt.m_packages.size() - 1; index++) {
        fullPath += '/';
        fullPath += stmt.m_packages[index];
        if (!OS::isFolderExist(fullPath)) {
            Error::complain("the specified package path %s does't exist\n", 
                            stmt.m_packages[index].c_str());
        }
    }
    // the last package must be file
    string fullPathFile = fullPath;
    fullPathFile += '/';
    fullPathFile += stmt.m_packages[stmt.m_packages.size() - 1];
    
    if (!OS::isFilesExist(fullPathFile)) {
        Error::complain("the specified package %s does't exist\n", fullPathFile.c_str());
    }
}

/// @brief TypeBuilder handler for Block Statement
void TypeBuilder::accept(BlockStatement &blockStmt) {
    vector<Statement *>::iterator ite = blockStmt.m_statements.begin();
    for (; ite != blockStmt.m_statements.end(); ite++) 
        walk(*ite);
}
/// @brief TypeBuilder handler for Variable Declaration statement
void TypeBuilder::accept(VariableDeclStatement &stmt) {
    walk(stmt.m_var);
    walk(stmt.m_expr);
    // check the type comatibliity
    if (stmt.m_expr) {
        Type *varType = getType(var.m_name);
        if (!varType || !varTyype->isCompatibliWith(stmt.m_expr->m_type))
            Error::complain("the variable %s is initialize with wrong type\n",
                    var.m_name.c_str());
    }
}
/// @brief TypeBuilder handler for if statement
void TypeBuilder::accept(IfStatement &stmt) {
    // walk and check the condition expression type
    assert(stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain("the if condition type is wrong\n");
    
    // the expression type shoud be checked
    walk(stmt.m_ifBlockStmt);
    walk(stmt.m_elseBlockStmt);
}

/// @brief TypeBuilder handler for while statement
void TypeBuilder::accept(WhileStatement &stmt) {
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain("the while condition type is wrong\n");
    
    walk(stmt.m_stmt);
}

/// @brief TypeBuilder handler for do while statement
void TypeBuilder::accept(DoStatement &stmt) {
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain("the do condition type is wrong\n");
    
    walk(stmt.m_stmt);
}
/// @brief TypeBuilder handler for for statement
void TypeBuilder::accept(ForStatement &stmt){
    
    
}

/// @brief TypeBuilder handler for foreach statement
void TypeBuilder::accept(ForEachStatement &stmt) {
    
}

/// @brief TypeBuilder handler for switch statement
void TypeBuilder::accept(SwitchStatement &stmt) {
    // check the condition type
    IntType intType;
    wall(stmt.m_conditExpr);
    if (!isTypeCompatible(stmt.m_conditExpr, &intType))
        Error::complain("the switch condition type is wrongly declared\n");
       
    // for each case, iterate
    for (int index = 0; index < (int)stmt.m_cases.size(); index++) {
        std::pair<vector<Expression *>, Statement *> *block = &stmt.m_cases[index];
        if (block) {
            vector<Expression *> exprList = block->first;
            // iterate the expression list
            vector<Expression *>::iterator ite = exprList.begin();
            for (; ite != exprList.end(); ite++) {
                Expression *expr = *ite;
                walk(expr);
                if (!isTypeCompatible(expr->m_type, &intType))
                    Error::complain("the case type is wrongly declared\n");
            }
            // check the statement block
            walk(block->second);
        }
    }
    walk(stmt._mdefaultStmt);
}
/// @brief TypeBuilder handler for continue statement
void TypeBuilder::accept(ContinueStatement &stmt) {
    // in compile phase, the continue statement error should be checked
}

/// @brief TypeBuilder handler for break statement
void TypeBuilder::accept(BreakStatement &stmt) {
    // in compile phase, the break statement error should be checked
}

/// @brief TypeBuilder handler for return statement
void TypeBuilder::accept(ReturnStatement &stmt) {
    // the expression type shoud be checked
    walk(stmt.m_resultExpr);
    // the return type and the function' type must be compatible
}

/// @brief TypeBuilder handler for throw statement
void TypeBuilder::accept(ThrowStatement &stmt) {
    walk(stmt.m_resultExpr);
}

/// @brief TypeBuilder handler for assert statement
void TypeBuilder::accept(AssertStatement &stmt) {
    walk (stmt.m_resultExpr);
    Bool boolType;
    if (!isTypeCompatible(stmt.m_resultExpr->m_type, &boolType))
        Error::complain("the assert expression shoul be bool type\n")        
}

/// @brief TypeBuilder handler for try statement
void TypeBuilder::accept(TryStatement &stmt) {
    walk(stmt.m_blockStmt);
    
    vector<CatchStatement *>::iterator ite;
    for (ite = stmt.m_catchStmts.begin(); ite != stmt.m_catchStmts.end(); ite++) 
        walk(*ite);
    
    walk(stmt.m_finallyStmt);
}

/// @brief TypeBuilder handler for catch statement
void TypeBuilder::accept(CatchStatement &stmt) {
    if (!hasSymbol(stmt.m_type)) {
        Error::complain("the type is not declared\n", stmt.m_type.c_str());
    }
    
    enterScope(stmt.m_id, ST_OTHER);
    
    if (hasSymbol(stmt.m_id)) {
        Error::complain("the symbol %s has been defined\n", stmt.m_id.c_str());
    }
    else {
        // defineSymbol(makeSymbol(stmt.m_type,stmt.m_id));
    }
    if (stmt.m_block)
        stmt.m_block->walk(this);
    exitScope();
}
/// @brief TypeBuilder handler for finallycatch statement
void TypeBuilder::accept(FinallyCatchStatement &stmt) {
    walk(stmt.m_block);
}



/// @brief TypeBuilder handler for expression
void TypeBuilder::accept(Expression &expr){
    
}
/// @brief TypeBuilder handler for expression list expression
void TypeBuilder::accept(ExpressionList &list) {
    vector<Expression *>::iterator ite;
    for (ite = list.m_exprs.begin(); ite != list.m_exprs.end(); ite++)
        walk(*ite);
}

/// @brief TypeBuilder handler for binary op expression
void TypeBuilder::accept(BinaryOpExpression &expr) {
    walk(expr.m_left);
    walk(expr.m_right)
}

/// @brief TypeBuilder handler for conditional expression
void TypeBuilder::accept(ConditionalExpression &expr) {
    
}

/// @brief TypeBuilder handler for logic or expression
void TypeBuilder::accept(LogicOrExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);
    }
}

/// @brief TypeBuilder handler for logic and expression
void TypeBuilder::accept(LogicAndExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);
    }    
}

/// @brief TypeBuilder handler for bitwise or expression
void TypeBuilder::accept(BitwiseOrExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);
    }    
}

/// @brief TypeBuilder handler for bitwise xor expression
void TypeBuilder::accept(BitwiseXorExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);
    }    
}

/// @brief TypeBuilder handler for bitwise expression
void TypeBuilder::accept(BitwiseAndExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);
    }    
}

/// @brief TypeBuilder handler for equality expression
void TypeBuilder::accept(EqualityExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);    
    }
}

/// @brief TypeBuilder handler for relational expression
void TypeBuilder::accept(RelationalExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression *subExpr = *ite;
        if (subExpr)
            subExpr->walk(this);
    }    
}

/// @brief TypeBuilder handler for shift expression
void TypeBuilder::accept(ShiftExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression* element = *ite;
        if (element)
            element->walk(this);
    }    
}

/// @brief TypeBuilder handler for additive expression
void TypeBuilder::accept(AdditiveExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression* element = *ite;
        if (element)
            element->walk(this);    
    }
}
    
/// @brief TypeBuilder handler for multiplicative expression    
void TypeBuilder::accept(MultiplicativeExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expression* element = *ite;
        if (element)
            element->walk(this);
    }      
}
   
/// @brief TypeBuilder handler for unary expression    
void TypeBuilder::accept(UnaryExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *selector = *ite;
        if (selector)
            selector->walk(this);
    }
}

/// @brief TypeBuilder handler for primary expression
void TypeBuilder::accept(PrimaryExpression &expr) {
    switch (expr.m_type) {
        case PrimaryExpression::T_IDENTIFIER:
            // check to see wether the identifier is defined in current scope
            if (!hasSymbol(expr.m_text)) {
                Error::complain("the symbol %s is not defined in current scope \n",
                                expr.m_text.c_str());
            }
            break;
        case PrimaryExpression::T_COMPOUND:
            expr.m_expr->walk(this);
            break;
        default:
            break;
                
    }
}

/// @brief TypeBuilder handler for selector expression
void TypeBuilder::accept(SelectorExpression &expr) {
    if (expr.m_target)
        expr.m_target->walk(this);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *selector = *ite;
        if (selector)
            selector->walk(this);
    }
    
}

/// @brief TypeBilder handler for new expression
void TypeBuilder::accept(NewExpression &expr) {
    // first, check wether the type is right
    if (!hasType(expr.m_type)) {
        Error::complain("the type %s doesn't exit\n", expr.m_type.c_str());
    }
    
    // check wether the arguments is right
    vector<Expression*>::iterator i = expr.m_arguments.begin();
    for (; i != expr.m_aguments.end(); i++) {
        Expression *expr = *i;
        expr->walk(this);
    }
}

// @brief TypeBuilder handler for map, such as map b = {0:1, 1:1} 
void TypeBuilder::accept(SetExpression &expr) {
    if (expr.m_exprList)
        expr.m_exprList->walk(this);
    
    // TODO set the expression type    
}

// @brief TypeBuilder handler for map, such as map<int,int> b = {0:1, 1:1} 
void TypeBuilder::accept(MapExpression &expr) {
    vector<MapItemExpression*>::iterator i = expr.m_items.begin();
    while (i != expr.m_items.enf()) {
        MapItemExpression *item = *i;
        item->walk(this);
        i++;
    }
    // TODO set the expression type
}

/// @breif Typebuilder handler for map item
void TypeBuilder::accept(MapItemExpression &expr) {
    if (expr.m_key)
        expr.m_key->walk(this);
    if (expr.m_val)
        expr.m_key->walk(this);
    // TODO: set the expression type
}

