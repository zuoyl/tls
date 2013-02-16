//
//  CodeBuilder.cpp
//  A toyable language compiler (like a simple c++)


#include "AST/AST.h"
#include "Runtime/Scope.h"
#include "Runtime/Error.h"
#include "Runtime/Label.h"
#include "Runtime/Local.h"
#include "Runtime/Frame.h"
#include "AST/Struct.h"
#include "AST/Variable.h"
#include "AST/Class.h"
#include "AST/Function.h"
#include "AST/Expression.h"
#include "AST/Statement.h"
#include "IRBuilder/IREmiter.h"
#include "IRBuilder/IRBuilder.h"

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
Local* IRBuilder::allocLocalInFrame(int size) {
	// get current frame
	Frame *frame = FrameStack::getCurrentFrame();
	return frame->allocLocal(size, true);
}

Local* IRBuilder::allocLocal(Type *type, bool inreg) {
	Local *local = NULL;
	int size = 0;
    
	if (type && !(size = type->getSize()))
		return NULL;
    
	if (!inreg) {
		local = allocLocalInFrame(size);
		local->initializeWithType(type);
	}
	else {
		// alloc locals from register window
		local = RegisterWindow::allocLocal(type);
	}
	return local;
    
}

int  IRBuilder::getLinkAddress(Function &func)
{
    return 0; // for dummy now
}

void IRBuilder::build(AST *ast)
{
    ast->walk(this);
}

// struct
void IRBuilder::accept(Struct &type) {
    // do nothing now
    
}

/// @brief IRBuidler handler for Variable
void IRBuilder::accept(Variable &var) {
    // get the scope and find wether the variable exist
    if (!hasSymbol(var.m_name)) {
        Error::complain("the variable %s doesn't exist\n", var.m_name.c_str());
        return;
    }
    
    // if the variable is global, the variable should be added into globaa memory
    // according to wether it is initialized, the destional region is different.
    if (var.m_isGlobal) {
        // push all global variable into one regin
        m_globalVars.push_back(&var);
       
        // if he variable is global and it is initialized
        // the initialization express must be directy evaluated,
        // at the same time , we should check wether the initialization expression's
        // type is compatible withe the variable's type
        if (var.m_isInitialized && var.m_expr) {
            build(var.m_expr);
            var.m_initializedVal = var.m_expr->m_value;
        }
    }
    
    // if the variable is class variable
    else if (var.m_isOfClass) {
        // get class from the current scope
        if (hasType(var.m_type)) {
            if (var.m_isInitialized && var.m_expr) {
                build(var.m_expr);
                var.m_initializedVal = var.m_expr->m_value; // MISS
            }
        }
    }
    // if the variable is function local variable
    // reserve the memory from the current frame and initialize
    else {
        Type *varType = getType(var.m_type);
        Local *local = allocLocal(varType);
        if (var.m_expr) {
            build(var.m_expr);
            Local *result = &var.m_expr->m_value;
            IREmiter::emitLoad(result, local);
        }
    }
    
}

/// @brief  Generate function name's specification
// functionName@class@retType@parametersCount@para1Type@para2Type...
void IRBuilder::makeFunctionName(Function &function, string &name) {
    name = function.m_name;
    name += "@";
    if (function.m_isOfClass)
        name += function.m_class;
    else if (function.m_isOfInterface)
        name += function.m_interface;
    
    name += "@";
    name += function.m_returnType;
    name += "@";
    if (function.hasParamter()) {
        name += function.getParameterCount();
        for (int index = 0; index < function.getParameterCount(); index++) {
            FunctionParameter *parameter = function.getParameter(index);
            name += "@";
            name += parameter->m_name;
        }
    }
}

/// @brief Function generator
void IRBuilder::generateFunction(Function &function) {
    FunctionType *funcType = (FunctionType *)getType(function.m_name);
    if (!funcType) {
        Error::complain("Function %s is not declared\n", function.m_name.c_str());
        return;
    }
    
    // make specified function name according to function name and parameter type
    string functName;
    makeFunctionName(function, functName);
    
    // mark funciton lable using the name
	Label label(functName);
	IREmiter::emitLabel(label);
    
    
	// get function regin information
	int linkAddr = getLinkAddress(function);
    
    
	// update the information into functionType
	funcType->setLinkAddress(linkAddr);
    
    
	if (function.m_paraList)
        function.m_paraList->walk(this);
		
	// get all locals and reserve memory for them
	int size = function.getLocalsSize();
	Frame *frame = FrameStack::allocNewFrame(size);
	FrameStack::push(frame);
		
    if (function.m_block)
        function.m_block->walk(this);
	
	FrameStack::pop();
	IREmiter::emit(IR_RET);
}

/// @brief Handler for Function IRBuilder
void IRBuilder::accept(Function &function) {
	// enter the function scope
	enterScope(function.getName(), dynamic_cast<Scope*>(&function));
    
	/// check to see wether the function has been declared
    FunctionType *funcType = (FunctionType *)getType(function.m_name, true);
    if (!funcType) {
        Error::complain("Function %s is not declared\n", function.m_name.c_str());
		exitScope();
        return;
    }
    
    // if the function is a member of class or interface,
    // the function must be in VTBL of the class and interface
    if (function.m_isOfClass || function.m_isOfInterface) {
        // check to see wether there is the function in VTBL
        ClassType *clsType = (ClassType *)getType(function.m_class);
        ASSERT (clsType != NULL);
        
        // get VTBL of the class
        ObjectVirtualTable *vtbl = clsType->getVirtualTable();
        if (!vtbl) {
            Error::complain("The class %s has not VTBL\n", clsType->getName().c_str());
			exitScope();
            return;
        }
        
        // check to see wether the VTBL have the function
        FunctionType *type = (FunctionType*)vtbl->getSlot(function.m_name);
        if (!type) {
            Error::complain("the class %s has not the function %s\n",
                             clsType->getName().c_str(),
                             function.m_name.c_str());
			exitScope();
            return;
        }
        
        // generate the code
        generateFunction(function);
        
    }
    
    // if the function is memeber of interface
    // the function must be in VTBL of the interface
    // do not generate instructions for the virtual funciton
    else if (function.m_isOfInterface) {
        
    }
    
    // the function is not class/interface's method
    // in this case, generate code directly,
    else {
        generateFunction(function);
    }
	exitScope();
    
}

/// @brief Handler for FunctionParameterList IRBuilder
void IRBuilder::accept(FunctionParameterList &list) {
	Function* func = (Function*)list.getParent();
    // iterate all parameters
    vector<FunctionParameter*>::iterator ite = list.m_parameters.begin();
    for (; ite != list.m_parameters.end(); ite++) {
        FunctionParameter *parameter = *ite;
		parameter->m_function = func;
        parameter->walk(this);
    }
}

/// @brief Handler for FunctionParameter IRBuilder
void IRBuilder::accept(FunctionParameter &para) {
	Function *func = para.m_function;
    // parameters should be passed to callee by reference
    // update the parameter's address information in symbol table
    
}

/// @brief Handler for FunctionBlock IRBuilder
void IRBuilder::accept(FunctionBlock &block) {
    // function block's prelog
    // adjust statc frame pointer according to all local's size
    Function* func = (Function*)block.getParent();
  
    // function block's body
    vector<Statement*>::iterator ite = block.m_stmts.begin();
    while (ite != block.m_stmts.end()) {
        Statement *stmt = *ite;
        build(stmt);
        ite++;
    }
}


/// @brief IRBuilder handler for Class
void IRBuilder::accep(Class &cls) {
	
    
}
/// @brief IRBuilder handler for ClassBlock
void IRBuilder::accept(ClassBlock &block) {
    
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
    Local *local1 = allocLocal(type);
    Local *result = NULL;
    
    // if the declared variable is initialized
    if (stmt.m_expr) {
        build(stmt.m_expr);
        result = &stmt.m_expr->m_value;
    }
    
    // load the result into locals
    IREmiter::emitLoad(local1, result);
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
		Local *local1 = allocLocal(true);
		IREmiter::emitStore(local1, stmt.m_conditExpr->m_value);
		IREmiter::cmitCMP(local1, 1, label1, label2);
		IREmiter::emitLabel(label1);
        build(stmt.m_ifBlockStmt);
		delete local1;
    }
    
    else {
        build(stmt.m_conditExpr);
		if (!stmt.m_conditExpr->hasValidValue())
			return;
		Label label3 = Label::newLabel();
		Local *local1 = allocLocal(true);
		
		IREmiter::emitStore(local1, stmt.m_conditExpr->m_value);
		IREmiter::cmitCMP(local1, 1, label1, label3);
		IREmiter::emitLabel(label1);
        build(stmt.m_ifBlockStmt);
   		
		IREmiter::emitLabel(label3);
        build(stmt.m_elseBlockStmt);
		delete local1;
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
	Local *local1 = allocLocal(true);
	IREmiter::emitStore(local1, stmt.m_conditExpr->m_value);
	IREmiter::emitCMP(local1, 1, label2, label3);
	delete local1;
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
	Local *local1 = allocLocal(true);
	IREmiter::emitStore(local1, stmt.m_conditExpr->m_value);
	IREmiter::emitCMP(local1, 1, label1, label2);
	IREmiter::emitLabel(label2);
	delete local1;
	// popup the current iterable statement
	frame->popIterablePoint();
}

/// @brief IRBuilder handler for for statement
void IRBuilder::accept(ForStatement &stmt) {
	// now only support normal loop mode
	if (stmt.m_mode == ForStatement::FORNORMAL) {
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
			Local *local2 = allocLocal();
			IREmiter::emitStore(local2, stmt.m_expr->m_value);
			IREmiter::emitCMP(local2, 1, labe2, label3);
			delete local2;
		}
		IREmiter::emitLable(label2);
		build(stmt.m_stmt);
		
		if (stmt.m_exprList) 
			build(stmt.m_exprList);
		IREmiter::emit(IR_JUMP, label1);
		IREmiter::emitLabel(labe3);
		// popup the current iterable point
		frame->popIterablePoint();
	}
	IREmiter::emitLabel(stmt.m_nextLabel);
}

/// @brief IRBuilder handler for switch statement
void IRBuilder::accept(SwitchStatement &stmt) {
	// check parameter's validity
	ASSERT(stmt.m_conditExpr != NULL);
    
// condition judge statement
	build(stmt.m_conditExpr);
	if (!stmt.m_conditExpr->hasValidValue())
		return;
	Local *local1 = allocLocal(true);
	IREmiter::emitStore(local1, stmt.m_conditExpr->m_value);
	Label label2 = Label::newLabel();
    
	// generate case statement
	vector< std::pair<vector<Expression*>, Statement *> >::iterator ite = stmt.m_cases.begin();
	for (; ite != stmt.m_cases.end(); ite++)  {
	//
		std::pair<vector<Expression *>, Statement*>  &pt = *ite;
		std::vector<Expression *> exprList = pt.first;
		Statement *sts = pt.second;
        
		// case n
		// case n+1
		Label label1 = Label::newLabel();
		std::vector<Expression *>::iterator ite = exprList.begin();
		for (; ite != exprList.end(); ite++) {
		    // get case value
			Expression * expr = *ite;
			build(expr);
			if (!expr->hasValidValue())
				continue;
			Local *local2 = allocLocal(true);
	        IREmiter::emitStore(local2, expr->m_value);
	        IREmiter::emitCMP(local2, local1, label1, label2);
			delete local2;
	    }
        
		IREmiter::emitLabel(label1);
	    // build statement
	    build(sts);
		IREmiter::emit(IR_JUMP, label2);
    }
    
	if (stmt.m_defaultStmt)
		build(stmt.m_defaultStmt);
    
	IREmiter::emitLabel(label2);
	delete local1;
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
			Local *local = allocLocal(true);
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
	Local *local = allocLocal(true);
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
/// the exception handler should be implement by function type
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
void IRBuilder::accept(Expression &expr)
{
}
void IRBuilder::accept(ExpressionList &exprList)
{
    vector<Expression *>::iterator ite = exprList.m_exprs.begin();
    for (; ite != exprList.m_exprs.end(); ite++) 
    {
        Expression *expr = *ite;
        build(expr);
    }
    
}
/// @brief IRBuilder handler for BinaryOpExpression
void IRBuilder::accept(BinaryOpExpression &expr) {
    ASSERT(expr.m_left != NULL);
    ASSERT(expr.m_right != NULL);
    
    build(expr.m_left);
	if (!expr.m_left->hasValidValue())
		return;
	Local *left = allocLocal(true);
    IREmiter::emitLoad(left, expr.m_left->m_value);
    
    build(expr.m_right);
	if (e!expr.m_left->hasValieValue()) {
		delete left;
		return;
	}
    Local *right = allocLocal(true);
    IREmiter::emitLoad(right, expr.m_right->m_value);
    
    // convert the binary operatiorn
    int op = IR_INVALID;
    switch (expr.m_op) {
        case BinaryOpExpression::BOP_ADD:
            op = IR_ADD;
            break;
        case BinaryOpExpression::BOP_SUB:
            op = IR_SUB;
            break;
        case BinaryOpExpression::BOP_MUL:
            op = IR_MUL;
            break;
        case BinaryOpExpression::BOP_DIV:
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

/// @brief IRBuilder handler for ConditionalExpression
void IRBuilder::accept(ConditionalExpression &expr) {
}

/// @brief IRBuilder handler for LogicOrExpression
void IRBuilder::accept(LogicOrExpression &expr) {
    ASSERT(expr.m_target != NULL);
	
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	Local *local = allocLocal(true);
	Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
	
	IREmiter::emitLoad(local1, expr.m_target->m_value);
	IREmiter::emitCMP(local, 1, label1, label2);
	IREmiter::emitLable(labe2);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			continue;
		
		Local *local2 = allocLocal(true);
		IREmiter::emitLoad(local2, element->m_value);
		IREmiter::emitBinOP(IR_OR, local1, loca2, local1);
		delete local2;
		
		Label label3 = Label::newLabel();
		IREmiter::emitCMP(local1, 1, nextLable, label3);
		IREmiter::emitLable(label3);
    }
	
	// store the result into expr.result
	expr.m_value = *local1;
	IREmiter::emitLable(label1);
	delete local1;
}

/// @brief IRBilder handler for LogicAndExpression
void IRBuilder::accept(LogicAndExpression &expr) {
    ASSERT(expr.m_target != NULL);
   
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	Label label1 = Label::newLabel();
    Label label2 = Label::newLabel();
	Local *local1 = allocLocal(true);
   	
	IREmiter::emitLoad(local1, expr.m_target->m_value);
	IREmiter::emitCMP(local, 1, label1, label2);
    IREmiter::emitLabel(label2);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			continue;
		
		Local *local2 = allocLocal(true);
		Label label3 = Label::newLabel();
		IREmiter::emitLoad(local2, element->m_value);
		IREmiter::emitBinOP(IR_AND, local1, local2, local1);
		delete local2;
		
		IREmiter::emitCMP(local1, 1, label3, label1);
        IREmiter::emitLabel(label3);
    }
	
	// store the result into expr.result
	expr.m_value = *local1;
	IREmiter::emitLable(label1);
	delete local1;
}


/// @brief IRBilder handler for bitwise or expression
// BitwiseOrExpression : BitwiseXorExpression ( '|' bitwiseXorExpression)*
void IRBuilder::accept(BitwiseOrExpression &expr) {
    ASSERT(expr.m_target != NULL);
   
    build(expr.m_target);
	if (!expr.m_target->hasValieValue())
		return;
    
    Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, &expr.m_target->m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			continue;
		
		Local *local2 = allocal(true);
		IREmiter::emitLoad(local2, element->m_value);
        IREmiter::emitBinOP(IR_BIT_OR, local1, local2, local1);
		delete local2;
    }
	
	expr.m_value = *local1; 
	delete local1;
}

/// @brief IRBuilder handler for bitwise xor expression
/// BitwiseXorExpression : BitwiseAndExpression ('^' bitwiseAndExpression)*
void IRBuilder::accept(BitwiseXorExpression &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	
    Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, expr.m_target->m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
		build(element);
		if (!element->hasValidValue())
			continue;
		Local *local2 = allocLocal(true);
        IREmiter::emitStore(local2, element->m_value);
        IREmiter::emitBinOP(IR_BIT_XOR, local1, local2, local1);
		delete local2;
    }
	expr.m_value = *local1;
	delete local1;
}

/// @brief IRBuilder handler for bitwise and expression
/// BitwiseAndExpression : EqualityExpression ('&' EqualilityExpression)*
void IRBuilder::accept(BitwiseAndExpression &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		continue;
	
    Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, expr.m_target->m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			continue;
        
		Local *local2 = allocLocal(true);
        IREmiter::emitStore(local2, element->m_value);
        IREmiter::emitBinOP(IR_BIT_AND, local1, local2, local1);
		delete local2;
    }
	expr.m_value = *local1;
	delete local1;
}

/// @brief IRBuilder handler for equality Expression
/// EqualityExpression : RelationalExpression (('==' | '!=') RelationalExpression)*
void IRBuilder::accept(EqualityExpression &expr) {
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
	if (!expr.m_target->hasValidValud())
		return;
	Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, expr.m_target->m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			continue;
        
        int irt = IR_INVALID;
        if (expr.m_op == EqualityExpression::OP_EQ)
            irt = IR_EQ;
        else if (expr.m_op == EqualityExpression::OP_NEQ)
            irt = IR_NEQ;
		
		if (irt != IR_INVALID) { 
			Local *local2 = allocLocal(true);
	        IREmiter::emitStore(local2, element->m_value);
       	 	IREmiter::emitBinOP(irt, local1, local2, local1);
			delete local2; 
		}
    }
	
	expr.m_value = *local1;
	delete local1;
}

/// @brief IRBuilder handler ffor relational expression
/// RelationalExpression :
///  ShiftExpression (('>' | '<' | '>=' | '<=') ShiftExpression)*
void IRBuilder::accept(RelationalExpression &expr) {
    ASSERT(expr.m_target != NULL);
    
	// build expression and get result into local
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, expr.m_target->m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
		build(element);
		if (!elment->hasValidValue())
			continue;
		
        int irt = IR_INVALID;
        switch (expr.m_op) {
            case  RelationalExpression::OP_LT:
                irt = IR_LT;
                break;
            case RelationalExpression::OP_LTEQ:
                irt = IR_LTEQ;
                break;
            case RelationalExpression::OP_GT:
                irt = IR_GT;
                break;
            case RelationalExpression::OP_GTEQ:
                irt = IR_GTEQ;
                break;
            default:
                irt = IR_INVALID;
                break;
        }
		
		if (irt != IR_INVALID) {
		    Local *local2 = allocLocal(true);
	        IREmiter::emitStore(local2, element->m_value);
	        IREmiter::emitBinOP(irt, local1, local2, local1);
			delete local2;
		}
    }
	expr.m_value = *local1;
	delete local1;
}

/// @brief IRBuilder handler for shift expression
/// ShiftExpression : AdditiveExpression (('>>'|'<<') AdditiveExpression)*
void IRBuilder::accept(ShiftExpression &expr) {
    ASSERT(expr.m_target != NULL);

    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	Local *local1 = allocLocal(true);
	IREmiter::emitStore(local1, expr.m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			continue;
		
	    int irt = IR_INVALID;
        if (expr.m_op == ShiftExpression::OP_LSHIFT)
            irt = IR_LSHIFT;
        else if (expr.m_op == ShiftExpression::OP_RSHIFT)
            irt = IR_RSHIFT;
		
		if (irt != IR_INVALID) {
			Local *local2 = allocLocal(true);
       	 	IREmiter::emitStore(local2, element->m_value);
        	IREmiter::emitBinOP(irt, local1, local2, local1);
			delete local2;
 		}
	}
	expr.m_value = *local1;
	delete local1;
    
}

// AdditiveExpression :
//  MultiplicativeExpression (('+' | '-') MultiplicativeExpression
void IRBuilder::accept(AdditiveExpression &expr)
{
    ASSERT(expr.m_target != NULL);
    
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, expr.m_target->m_value);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite;
        build(element);
		if (!element->hasValidValue())
			cointinue;
        
        int irt = IR_INVALID;
        if (expr.m_op == AdditiveExpression::OP_PLUS)
            irt = IR_ADD;
        else if (expr.m_op == AdditiveExpression::OP_SUB)
            irt = IR_SUB;
		
		if (irt != IR_INVALID) {
			Local *local2 = allocLocal(true);
	        IREmiter::emitStore(local2, element->m_value);
			IREmiter::emitBinOP(irt, local1, local2, local1);
			delete local2;
		}
    }
	expr.m_value = *local1;
	delete local1;
}


/// @brief IRBuilder handler for multiplicative expression
/// MultiplicativeExpression :
/// UnaryExpresion (('*' | '/' | '%') UnaryExpression)*
void IRBuilder::accept(MultiplicativeExpression &expr) {
	ASSERT(expr.m_target != NULL);
	
    build(expr.m_target);
	if (!expr.m_target->hasValidValue())
		return;
	Local *local1 = allocLocal(true);
    IREmiter::emitLoad(local1, expr.m_target->m_value);
  
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        Expression *element = *ite; 
        build(element);
		if (!element->hasValidValue())
			continue;
	
	    int irt = IR_INVALID;
        if (expr.m_op == MultiplicativeExpression::OP_MUL)
            irt = IR_MUL;
        else if (expr.m_op == MultiplicativeExpression::OP_DIV)
            irt = IR_DIV;
        else if (expr.m_op == MultiplicativeExpression::OP_MODULO)
            irt = IR_MODULO;
		
		if (irt != IR_INVALID) {
			Local *local2 = allocLocal(true);
	        IREmiter::emitStore(local2, element->m_value);
        	IREmiter::emitBinOP(irt, local1, local2, local1);
			delete local2;
      	}
        
    }
	expr.m_value = *local1;
	delete local1;
}


// unaryExpression
// | primary selector*
// ;

// selector
// : assignableSelector
// | arguments
// ;

// identifier
// : IDENTIFIER

void IRBuilder::accept(UnaryExpression &expr)
{
    build(expr.m_target);
    
    vector<Expression *>::iterator ite = expr.m_elements.begin();
    for (; ite != expr.m_elements.end(); ite++) {
        build(*ite);
    }
}

/*
primary
: 'this'
| 'super'
| 'null'
| 'true'
| 'false'
| NUMBER
| HEX_NUMBER
| STRING
| mapLiteral
| listLiteral
| identifier
| '(' expression ')'
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

void IRBuilder::accept(PrimaryExpression &expr)
{
    switch (expr.m_type) {
        case PrimaryExpression::T_THIS:
            break;
        case PrimaryExpression::T_TRUE:
            break;
        case PrimaryExpression::T_FALSE:
            break;
        case PrimaryExpression::T_HEX_NUMBER:
            break;
        case PrimaryExpression::T_NULL:
            break;
        case PrimaryExpression::T_STRING:
            break;
        case PrimaryExpression::T_NUMBER:
            break;
        case PrimaryExpression::T_MAP:
            break;
        case PrimaryExpression::T_IDENTIFIER:
            break;
        case PrimaryExpression::T_SUPER:
            break;
        default:
            break;
    }
    
}
void IRBuilder::accept(SelectorExpression &expr)
{
    
}

// new
void IRBuilder::accept(NewExpression &expr)
{
    
}

// map & list
void IRBuilder::accept(MapExpression &expr)
{
    
}
void IRBuilder::accept(ListExpression &expr)
{
    
}






