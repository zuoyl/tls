//
//  TypeChecker.cpp
//  A toyable language compiler (like a simple c++)


#include "Runtime/Error.h"
#include "Runtime/OSWraper.h"
#include "Runtime/Frame.h"
#include "AST/Struct.h"
#include "AST/Variable.h"
#include "AST/Class.h"
#include "AST/Function.h"
#include "AST/Expression.h"
#include "AST/Statement.h"
#include "IRBuilder/TypeBuilder.h"

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



/// @brief TypeBuilder handler for Struct 
void TypeBuilder::accept(Struct &st) {
	// the struct is also a scope iteself
	enterScope(st.getName(), dynamic_cast<Scope*>(&st));
	
	// check to see wether the struct has beeb defined
    if (hasType(st.m_name), true) {
        Error::complain("the struct name %s has been used\n", st.m_name.c_str());
		exitScope();
		return;
    }
	
	// generate a new struct type in currrent scope
    StructType *pstType = new StructType();
    defineType(pstType);
    
    // check to see wether the member's type exist
    vector<Struct::Member>::iterator ite;
    for (ite = st.m_members.begin(); ite < st.m_members.end(); ite++) {
        Struct::Member member = *ite;
        
        if (!hasType(member.first)) {
            Error::complain("Type %s is not defined\n", member.first.c_str());
        }
            
        // check to see wether there are same members  
        vector<Struct::Member>::iterator ip;
        for (ip = st.m_members.begin(); ip < st.m_members.end(); ip++) {
            if (ip != ite  && ip->second == ite->second) {
                Error::complain("there are same identifier %s in struct %s",
                                st.m_name.c_str(),
                                ip->second.c_str());
            }
        }
        
        // member's type and name is checked, insert them into StructTypes
        Type *type = getType(member.first);
        pstType->addSlot(member.second, type);
    }
}

/// @brief TypeBuilder handler for Variable
void TypeBuilder::accept(Variable &var) {
    // wethee\r there is same variable in currrent scope
    if (hasSymbol(var.m_name)) {
        Error::complain("there is same variable %s in current scope\n",
                        var.m_name.c_str());
		return;
    }
        // wether the variable's type is defined
    if (!hasType(var.m_type)) {
        Error::complain("the variable %s's type %s is not defined\n",
                        var.m_name.c_str(),
                        var.m_type.c_str());
    }

	// generate a new symbol and type in curret scope
    Type *varType = getType(var.m_type);
    Symbol *symbol = new Symbol();
    symbol->m_name = var.m_name;
    symbol->m_type = varType;
    defineSymbol(symbol);
    
    // wether the variable is initialized
    if (var.m_isInitialized && !var.m_expr) {
        Error::complain("The variable %s is not rightly initialized\n", var.m_name.c_str());
    }
    
    // if the variable is initialized, must to check the type is same
    if (var.m_isInitialized && var.m_expr != NULL) {
        // get the var's type
        Type *type = getType(var.m_type);
        if (!type) {
            Error::complain("the variable %s's type is not declared\n", var.m_name.c_str());
        }
        else if (*type != *var.m_expr->m_type) {
            Error::complain("the variablle %s is not rightly initialzed\n", 
                            var.m_name.c_str());
        }
    }
    
    // if the variable is class's variable, the variable should be added into classType
    if (var.m_isOfClass) {
        ClassType *clsType = (ClassType*)getType(var.m_class);
        if (!clsType) {
            Error::complain("the class %s has not the variable %s\n", 
                            var.m_class.c_str(),
                            var.m_name.c_str());
        }
        else {
            Type * varType = getType(var.m_type);
            clsType->addSlot(var.m_name, varType);
        }
    }
    
}

/// @brief Handler for function type builder
void TypeBuilder::accept(Function &function) {
	
	// set the current scope
	enterScope(function.m_name, dynamic_cast<Scope*>(&function));
	
	// check the return type
    if (!hasType(function.m_returnType)) {
        Error::complain("the return type %s is not defined\n", 
                        function.m_returnType.c_str());
    }
    
    // check wether the function name exist
    if (hasSymbol(function.m_name)) {
        Error::complain("the function name %s already exist", function.m_name.c_str());
    }
    else {
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
        else if (function.m_isOfInterface) {
            InterfaceType *infType = (InterfaceType *)getType(function.m_interface);
            infType->addSlot(function.m_name, infType);
        }
        
    }
    
    // check the function parameter list
    if (function.m_paraList != NULL)
        function.m_paraList->walk(this);
    
    // check the function block
    if (function.m_block != NULL)
        function.m_block->walk(this);
	
	exitScope();
    
}

/// @brief Handler for FunctionParameterList type builder
void TypeBuilder::accept(FunctionParameterList &list) {
    vector<FunctionParameter *>::iterator ite;
    
    for (ite = list.m_parameters.begin(); ite != list.m_parameters.end(); ite++) {
       
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
    // wether the variable's type is rightly defined
    if (!hasType(para.m_type)) {
        Error::complain("the type %s is not defined\n", para.m_type.c_str());
    }
    
    // if the parameter has default value, 
    // check wethere the expression's type is same with variable's type
    if (para.m_hasDefault && para.m_default != NULL) {
        Type * type = getType(para.m_type);
        if (type && *type != *para.m_default->m_type) {
            Error::complain("the parameter %s is not rightly initialized\n",
                            para.m_name.c_str());
        }
    }
}

/// @brief TypeBuilder handler for FunctionBlock
void TypeBuilder::accept(FunctionBlock &block) {
    vector<Statement *>::iterator ite;
    for (ite = block.m_stmts.begin(); ite != block.m_stmts.end(); ite++) {
        Statement *stmt = *ite;
        if (stmt != NULL)
            stmt->walk(this);
    }
}


/// @brief TypeBuilder handler for Class
void TypeBuilder::accep(Class &cls) {
	// the class is also scope
	enterScope(cls.getName(), dynamic_cast<Scope*>(&cls));
	
    // check wether the class name exist?
	bool nested = (cls.m_isPublic == true)? true:false;
    if (hasSymbol(cls.m_name, nested)) {
        Error::complain("the class name %s is already defined\n", cls.m_name.c_str());
		return;
    }
    // put the class Type int the current scope
    ClassType *clsType = new ClassType(cls.m_name, m_curScope, cls.m_isPublic);
    defineType(clsType);
        
    // puth the class symbo in the current scope
    Symbol *symbol = new Symbol();
    symbol->m_type = clsType;
    symbol->m_name = cls.m_name;
    defineSymbol(symbol);
    
    // check wether the base class exist?
    vector<string>::iterator ite;
    vector<InterfaceType *> ifset;
    for (ite = cls.m_base.begin(); ite != cls.m_base.end(); ite++) {
        string baseClass = *ite;
        Type *type = getType(baseClass);
        if (!type) {
            Error::complain("the class  %s is not declared\n", baseClass.c_str());
        }
        else {
            InterfaceType *ifbase = dynamic_cast<InterfaceType*>(type);
            if (ifbase)
                ifset.push_back(ifbase);
        }
    }   
    // check to see if the base class is interface, 
    // the methd exported by interface must be implemented
    vector<InterfaceType *>::iterator ite2;
    for (; ite2 != ifset.end(); ite2++) {
        InterfaceType *ifbase = *ite2;
        for (int index = 0; index < ifbase->getSlotCount(); index++) {
            Type *slot = ifbase->getSlot(index);
            if (slot) {
                Type *slot2 = slot->getSlot(slot->getName());
                if (!slot2) 
                    Error::complain("the method %s is not implement in class %s\n",
                                    slot->getName().c_str(),
                                    cls.m_name.c_str());
            }
        }
        
    }   
}

/// @brief TypeBuilder handler for ClassBlock
void TypeBuilder::accept(ClassBlock &block) {
    vector<Variable *>::iterator ite;
    for (ite = block.m_vars.begin(); ite != block.m_vars.end(); ite++) {
        Variable *var = *ite;
        if (var)
            var->walk(this);
    }
    
    vector<Function *>::iterator it;
    for (it = block.m_functions.begin(); it != block.m_functions.end(); ite++) {
        Function *function = *it;
        if (function)
            function->walk(this);
    }
}

/// @brief TypeBuildef handler for Interface
void TypeBuilder::accept(Interface &interface) {
	// interface is also a scope
	enterScope(interface.getName, dynamic_cast<Scope*>(&interface));
	
    if (hasSymbol(interface.getName())) {
        Error::complain("the interface name %s is already defined\n", 
                        interface.m_name.c_str());
		exitScope();
		return;
    }
	
    // put the interface type in the current scope
    InterfaceType *infType = new InterfaceType(interface.m_name, m_curScope, interface.m_isPublic);
    defineType(infType);
        
    // put the interface symbol in the current scope
    Symbol *symbol = new Symbol();
    symbol->m_name = interface.m_name;
    symbol->m_type = infType;
    defineSymbol(symbol);
    
    vector<Function *>::iterator ite;
    for (ite = interface.m_functions.begin(); ite != interface.m_functions.end(); ite++) {
        Function *function = *ite;
        if (function)
            function->walk(this);
    }
    
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
    ASTVisitor *visitor = dynamic_cast<ASTVisitor*>(this);
    
    vector<Statement *>::iterator ite = blockStmt.m_statements.begin();
    for (; ite != blockStmt.m_statements.end(); ite++) {
        Statement *stmt = *ite;
        if (stmt)
            stmt->walk(visitor);
    }
}
/// @brief TypeBuilder handler for Variable Declaration statement
void TypeBuilder::accept(VariableDeclStatement &stmt) {
    ASTVisitor *visitor = dynamic_cast<ASTVisitor*>(this);
    
    if (stmt.m_var)
        stmt.m_var->walk(visitor);
    
    if (stmt.m_expr)
        stmt.m_expr->walk(visitor);    
}
/// @brief TypeBuilder handler for if statement
void TypeBuilder::accept(IfStatement &stmt) {
    ASTVisitor *visitor = dynamic_cast<ASTVisitor*>(this);
    
    if (stmt.m_conditExpr)
        stmt.m_conditExpr->walk(visitor);
    
    // the expression type shoud be checked
    // TODO
    
    if (stmt.m_ifBlockStmt)
        stmt.m_ifBlockStmt->walk(visitor);
    
    if (stmt.m_elseBlockStmt)
        stmt.m_elseBlockStmt->walk(visitor);
}

/// @brief TypeBuilder handler for while statement
void TypeBuilder::accept(WhileStatement &stmt) {
    ASTVisitor *visitor = dynamic_cast<ASTVisitor*>(this);

    // the expression type shoud be checked
    // TODO
    
    if (stmt.m_conditExpr)
        stmt.m_conditExpr->walk(visitor);
    
    if (stmt.m_stmt)
        stmt.m_stmt->walk(visitor);
}

/// @brief TypeBuilder handler for do while statement
void TypeBuilder::accept(DoStatement &stmt) {
    ASTVisitor *visitor = dynamic_cast<ASTVisitor*>(this);
    
    // the expression type shoud be checked
    // TODO
    
    if (stmt.m_conditExpr)
        stmt.m_conditExpr->walk(visitor);
    
    if (stmt.m_stmt)
        stmt.m_stmt->walk(visitor);
    
}
/// @brief TypeBuilder handler for for statement
void TypeBuilder::accept(ForStatement &stmt){
    
}

/// @brief TypeBuilder handler for switch statement
void TypeBuilder::accept(SwitchStatement &stmt) {
    // check the condition
    if (stmt.m_conditExpr)
        stmt.m_conditExpr->walk(this);
    
    // for each case, iterate
    for (int index = 0; index < (int)stmt.m_cases.size(); index++) {
        std::pair<vector<Expression *>, Statement *> *block = &stmt.m_cases[index];
        if (block) {
            vector<Expression *> exprList = block->first;
            // iterate the expression list
            vector<Expression *>::iterator ite = exprList.begin();
            for (; ite != exprList.end(); ite++) {
                Expression *expr = *ite;
                if (expr)
                    expr->walk(this);
            }
            // check the statement block
            Statement *stmt2 = block->second;
            if (stmt2)
                stmt2->walk(this);
        }
        
    }
    
    if (stmt.m_defaultStmt)
        stmt.m_defaultStmt->walk(this);
    
}
/// @brief TypeBuilder handler for continue statement
void TypeBuilder::accept(ContinueStatement &stmt) {
    
}

/// @brief TypeBuilder handler for break statement
void TypeBuilder::accept(BreakStatement &stmt) {
    
}

/// @brief TypeBuilder handler for return statement
void TypeBuilder::accept(ReturnStatement &stmt) {
    // the expression type shoud be checked
    // TODO
    
    if (stmt.m_resultExpr)
        stmt.m_resultExpr->walk(this);
}

/// @brief TypeBuilder handler for throw statement
void TypeBuilder::accept(ThrowStatement &stmt) {
    // the expression type shoud be checked
    // TODO
    
    if (stmt.m_resultExpr)
        stmt.m_resultExpr->walk(this);
}

/// @brief TypeBuilder handler for assert statement
void TypeBuilder::accept(AssertStatement &stmt) {
    // the expression type shoud be checked
    // TODO
    
    if (stmt.m_resultExpr)
        stmt.m_resultExpr->walk(this);
    
}

/// @brief TypeBuilder handler for try statement
void TypeBuilder::accept(TryStatement &stmt) {
    if (stmt.m_blockStmt)
        stmt.m_blockStmt->walk(this);
    
    vector<CatchStatement *>::iterator ite;
    for (ite = stmt.m_catchStmts.begin(); ite != stmt.m_catchStmts.end(); ite++) {
        CatchStatement *catchStmt = *ite;
        if (catchStmt)
            catchStmt->walk(this);
    }
    
    if (stmt.m_finallyStmt)
        stmt.m_finallyStmt->walk(this);
    
}

/// @brief TypeBuilder handler for catch statement
void TypeBuilder::accept(CatchStatement &stmt) {
    if (!hasSymbol(stmt.m_type)) {
        Error::complain("the type is not defined\n", stmt.m_type.c_str());
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
    if (stmt.m_block)
        stmt.m_block->walk(this);
}



/// @brief TypeBuilder handler for expression
void TypeBuilder::accept(Expression &expr){
    
}
/// @brief TypeBuilder handler for expression list expression
void TypeBuilder::accept(ExpressionList &list) {
    vector<Expression *>::iterator ite;
  
    for (ite = list.m_exprs.begin(); ite != list.m_exprs.end(); ite++) {
        Expression *expr = *ite;
        if (expr)
            expr->walk(this);
    }
}

/// @brief TypeBuilder handler for binary op expression
void TypeBuilder::accept(BinaryOpExpression &expr) {
    if (expr.m_left)
        expr.m_left->walk(this);
    
    if (expr.m_right)
        expr.m_right->walk(this);
    
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

// map & list
void TypeBuilder::accept(MapExpression &expr)
{
    
}
void TypeBuilder::accept(ListExpression &expr)
{
    
}
