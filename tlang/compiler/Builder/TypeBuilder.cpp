//
//  TypeChecker.cpp
//  A toyable language compiler (like a simple c++)


#include "Error.h"
#include "OSWraper.h"
#include "Frame.h"
#include "Struct.h"
#include "Variable.h"
#include "Class.h"
#include "Method.h"
#include "Expression.h"
#include "Statement.h"
#include "TypeBuilder.h"

/// @brief Constructor
TypeBuilder::TypeBuilder() 
{
    string name = "Global";
    enterScope(name, NULL);
}

/// @brief Destructor
TypeBuilder::~TypeBuilder() 
{
    exitScope();
    if (m_curScope != m_rootScope) {
       // throw exception;
    }
    
}

/// @brief Enter a new scope
void TypeBuilder::enterScope(const string &name, Scope *scope) 
{
    if (m_curScope) {
        scope->setParentScope(m_curScope);
        m_curScope = scope;
    }
		else 
				m_curScope = scope;

    m_curScopeName = name;
    if (!m_rootScope)
        m_rootScope = scope;
}
/// @brief Exit the current scope
void TypeBuilder::exitScope() 
{
    if (m_curScope != NULL)
        m_curScope = m_curScope->getParentScope();
}

/// @brief Check to see wether the symbol specified by name exist
bool TypeBuilder::hasSymbol(const string &name, bool nested) 
{
    bool result = false;
    if (m_curScope && m_curScope->resolveSymbol(name, nested))
        result = true;

    return result;
}

/// @brief Check to see wether the type specified by name exist
bool TypeBuilder::hasType(const string &name, bool nested) 
{
    bool result = false;
    if (m_curScope && m_curScope->resolveType(name, nested))
        result = true;
    
    return result;
    
}

/// @brief Get symbol by name 
Symbol* TypeBuilder::getSymbol(const string &name, bool nested) 
{
    Symbol *symbol = NULL;
    if (m_curScope!= NULL)
        symbol = m_curScope->resolveSymbol(name, nested);
    
    return symbol;
}

/// @brief Get type by name
Type* TypeBuilder::getType(const string &name, bool nested) 
{
    Type *type = NULL;
    if (m_curScope != NULL)
        type = m_curScope->resolveType(name, nested);
    
    return type;
}

/// @brief Define a new symbo in current scope
void TypeBuilder::defineSymbol(Symbol *symbol) 
{
    if (symbol && m_curScope) {
        m_curScope->defineSymbol(symbol);
    }
}

/// @brief Define a new type in current scope
void TypeBuilder::defineType(Type *type)
{
    if (type && m_curScope)
        m_curScope->defineType(type);
}

/// @brief helper Methodo walk ast node
void TypeBuilder::walk(AST *node) 
{
    if (node)
        node->walk(this);
}

/// @brief Typebuilder handler for type specifier
void TypeBuilder::accept(TypeSpec &typeSpec) 
{
    Type *type = getType(typeSpec.m_name);
    if (!type) 
        Error::complain("the type is not declared\n", typeSpec.m_name.c_str());        
}

/// @brief TypeBuilder handler for Struct 
void TypeBuilder::accept(Struct &st) 
{
	// check to see wether the struct has beeb defined
    if (hasType(st.m_name), true) {
        Error::complain("the struct name %s has been used\n", st.m_name.c_str());
    }
	
	// generate a new struct type in currrent scope
    StructType *pstType = new StructType();
    defineType(pstType);

	// the struct is also a scope iteself
	enterScope(st.m_name, dynamic_cast<Scope*>(&st));
    
    // check to see wether the member's type exist
    vector<Struct::Member>::iterator ite;
    for (ite = st.m_members.begin(); ite < st.m_members.end(); ite++) {
        Struct::Member member = *ite;
        TypeSpec *typeSpec = member.first;
        walk(typeSpec);
        string name = member.second;
        
        // check to see wether there are same members  
        vector<Struct::Member>::iterator ip;
        for (ip = st.m_members.begin(); ip < st.m_members.end(); ip++) {
            if (ip != ite  && name == ip->second) {
                Error::complain("there are same identifier %s in struct %s", name.c_str(), st.m_name.c_str());
            }
        }
        pstType->addSlot(name, getType(typeSpec->m_name));
    }
    exitScope();

}

/// @brief TypeBuilder handler for Variable
void TypeBuilder::accept(Variable &var) 
{
    Type *type = NULL;
    bool isvalid = true;
    
    // check to see wether the type of var is right
    if (var.m_typeSpec == NULL) {
        Error::complain("the type of variable is not declared\n", var.m_name.c_str());
        isvalid = false;
    }
    else if ((type = getType(var.m_typeS)) == NULL) {
        Error::complain("the type %s is not declared\n", var.m_typeSpec->m_name.c_str());
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
            else if (!isTypeCompatible(type, var.m_expr->m_type)) {
                Error::complain("the global variable %s is initialized with no right type\n", var.m_name.c_str());
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
                else if (!isTypeCompatible(type, var.m_expr->m_value.m_type)) {
                    Error::complain("the class variable %s is initialized with no right type\n",
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
            else if (!isTypeCompatible(type, var.m_expr->m_value.m_type)) {
                Error::complain("the local variable %s is initialized with no right type\n",
                        var.m_name.c_str());
            }
        }
    }
    // define symbol in current scope 
    Symbol *symbol = new Symbol();
    symbol->m_name = var.m_name;
    symbol->m_type = type;
    defineSymbol(symbol);
}

/// @brief Handler for method type builder
void TypeBuilder::accept(Method &method) 
{
    bool isvalid = true;
    Type *returnType = NULL;

    // check to see wether the return type of method is  declared
    if (!method.m_retTypeSpec) {
        Error::complain("the Methodype is not declared\n");
        isvalid = false;
    }
    else if ((returnType = getType(method.m_retTypeSpec->m_name)) == NULL) {
        Error::complain("the Methodype %s is not declared\n", returnType->m_name.c_str());
        isvalid = false;
    }
    /// check to see wether the method name has been declared
    MethodType *methodType = (MethodType *)getType(method.m_name, true);
    if (methodType) {
        Error::complain("Method %s is already declared\n", method.m_name.c_str());
        isvalid = false;
    }
    
	// set the current scope
	enterScope(method.m_name, dynamic_cast<Scope*>(&method);
	// check the return type
    if (!hasType(method.m_returnType)) {
        Error::complain("the return type %s is not defined\n", Method_returnType.c_str());
        isvalid = false;
        
    }
    
    // check wether the Methodame exist
    if (hasSymbol(method.m_name)) {
        Error::complain("the Methodame %s already exist", meMethodame.c_str());
        isvalid = false;
    }
    
    
    // if the method is  a member of class or interface,
    // the method will be in VTBL of the class and interface
    if (method.m_isOfClass || method.m_isOfProtocol) {
        // check to see wether there is the Methodn VTBL
        ClassType *clsType = (ClassType *)getType(method.m_isOfclass);
        if (!clsType) {
            Error::complain("the Methods is not member of class %s\n", 
                    method.m_name.c_str(), method.m_class.c_str());
            isvalid = false;
        }
        
        // get VTBL of the class
        ObjectVirtualTable *vtbl = clsType->getVirtualTable();
        if (!vtbl) {
            Error::complain("The class %s has not VTBL\n", clsType->getName().c_str());
            isvalid = false;
        }
        
        // check to see wether the VTBL have the Method       
        MethodType *type = (MethodType*)vtbl->getSlot(method.m_name);
        if (!type) {
            Error::complain("the class %s has not the Methods\n",
                             clsType->getName().c_str(),
                             method.m_name.c_str());
            isvalid = false;
        }
    }
    
    if (isvalid) {
        // define Methodye in current scope
        MethodType *funcType = new MethodType();
        funcType->setName(method.m_name);
        defineType(funcType);
        
        // define method symbol in current scope
        Symbol *symbol = new Symbol();
        symbol->m_type = funcType;
        symbol->m_name = method.m_name;
        defineSymbol(symbol);;
        
        // if the method is member of class
        if (method.m_isOfClass) {
            ClassType *clsType = (ClassType *)getType(method.m_class);
            if (clsType)
                clsType->addSlot(method.m_name, funcType);
            else
                Error::complain("the class %s is not declared\n", method.m_class.c_str());
        }
        
        // if the method is  member of interface
        else if (method.m_isOfProtocol) {
            InterfaceType *protocolType = (ProtocolType *)getType(method.m_protocol);
            if (protocolType)
                protocolype->addSlot(method.m_name, funcType);
            else
                Error::complain("the protocol %s is not declaired\n", method.m_protocol.c_str());
        }
    }
    
    // check the method darameter list
    walk(method.m_paraList);
    // check the method lock
    walk(method.m_block);
    // exit the method cope
	exitScope();
    
}

/// @brief Handler for MethodParameterList type builder
void TypeBuilder::accept(MethodParameterList &list) 
{
    vector<MethodParameter *>::iterator ite =list.m_parameters.begin();
    for (ite != list.m_parameters.end(); ite++) {
        // check the parameter
        MethodParameter *methodParameter = *ite;
        walk(methodParameter);
        
        // check wether there are same variable's name
        vector<MethodParameter *>::iterator ip;
        for (ip = list.m_parameters.begin(); ip != list.m_parameters.end(); ip++) {
            MethodParameter *second = *ip;
            if (ite != ip && methodParameter->m_name == second->m_name) {
                Error::complain("there are same variable's name %s\n", 
                                second->m_name.c_str());
            }
        }
    }
}

/// @brief Handler for MethodParameter type builder
void TypeBuilder::accept(MethodParameter &para) 
{
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
        if (type && isCompatibleWith(type, para.m_default->m_type)) {
            Error::complain("the parameter %s is not rightly initialized\n",
                            para.m_name.c_str());
        }
    }
    // define the passed parameter in current symbol talbe
    Symbol *symbol = new Symbol();
    symbol->m_name = para.m_name;
    symbol->m_type = para.m_type;
    // if the Methods called, all parameters are pushed by caller
    // so the address of each parameter must be knowned
    symbol->m_storage = LocalStackSymbol;
    symbol->m_addr = para.m_index * 4;  // the index is offset 
    defineSymbol(symbol);
    
}

/// @brief TypeBuilder handler for MethodBlock
void TypeBuilder::accept(MethodBlock &block) 
{
    vector<Statement *>::iterator ite;
    for (ite = block.m_stmts.begin(); ite != block.m_stmts.end(); ite++) 
        walk(*ite);
}


/// @brief TypeBuilder handler for Class
void TypeBuilder::accep(Class &cls) 
{
    bool isvalid = true;
    // check wether the class name exist?
	bool nested = (cls.m_isPublic == true)? true:false;
    if (hasSymbol(cls.m_name, nested)) {
        Error::complain("the class name %s is already defined\n", cls.m_name.c_str());
		isvalid = false;
    }
	// the class is also scope
	enterScope(cls.m_name, dynamic_cast<Scope*>(&cls));
	pushClass(&cls);
        
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
        ClassType *clsType = getType(baseClass);                  
        if (!clsType)
            Error::complain("the base class  %s is not declared\n", baseClass.c_str());
        else if (clsType->isFrozen())
            Error::complain("the base class %s is frozen, can not be inherited\n", baseClass.c_str())
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
        else {
            for (int index = 0; index < protoclType->getSlotCount(); index++) {
                // for each slot in protocol, to check wether it is in class
                Type *slot = protocolType->getSlot(index);
                if (!cls.getMethod(slot->m_name)) {
                    Error::complain("the method %s exported by protocol %s is not implemented in class %s",
                                slot->m_name.c_str(), protocolName.c_str(), cls.m_name.c_str());
                }
            }
        }  
    }
    
    // walk through the class block
    build(cls.m_block);
    exitScope();
    popClass();

}

/// @brief TypeBuilder handler for ClassBlock
void TypeBuilder::accept(ClassBlock &block) 
{
    // iterate all variables
    vector<Variable*>::iterator varIteator = block.m_vars.begin();
    while(varIteator != block.m_vars.end()) {
        walk(*varIteator);
        varIteator++;
    }
   // iterate all Method    vector<Method*>::iterator funcIteator = block.m_Methodbegin();
    while (funcIteator != block.m_Methodend()) {
        walk(*funcIteator);
        funcIteator++;
    }
}

/// @brief TypeBuildef handler for Interface
void TypeBuilder::accept(Protocol &protocol) 
{
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
    
    // iterall all Methodof the protocol
    vector<Method*>::iterator ite = protocol.m_Methodbegin();
    for (; ite != protocol.m_Methodend(); ite++) {
        Method *func = *ite;
        if (func && func->m_name != protocol.m_name) {
            Error::complain("the Methods is not member of %s\n", 
                    func->m_name.c_str(), protocol.m_name.c_str());
        }
        walk(func);
    }
    // exit the protocol scope
    exitScope();
    
}

/// @brief TypeBuilder handler for Statement
void TypeBuilder::accept(Statement &stmt) 
{
}

/// @brief TypeBuilder handler for import statement
void TypeBuilder::accept(ImportStatement &stmt) 
{
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
void TypeBuilder::accept(BlockStatement &blockStmt) 
{
	// set the current scope
	enterScope("blockStatement", dynamic_cast<Scope*>(&blockStmt));
    
    vector<Statement *>::iterator ite = blockStmt.m_statements.begin();
    for (; ite != blockStmt.m_statements.end(); ite++) 
        walk(*ite);
    
    exitScope();
}
/// @brief TypeBuilder handler for Variable Declaration statement
void TypeBuilder::accept(VariableDeclStatement &stmt) 
{
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
void TypeBuilder::accept(IfStatement &stmt) 
{
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
void TypeBuilder::accept(WhileStatement &stmt) 
{
    pushIteralbeStatement(&stmt);
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain("the while condition type is wrong\n");
    
    walk(stmt.m_stmt);
    popIteralbeStatement();
}

/// @brief TypeBuilder handler for do while statement
void TypeBuilder::accept(DoStatement &stmt) 
{
    pushIteralbeStatement(&stmt);
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain("the do condition type is wrong\n");
    
    walk(stmt.m_stmt);
    popIterableStatement();
}
/// @brief TypeBuilder handler for for statement
void TypeBuilder::accept(ForStatement &stmt)
{
    pushIteralbeStatement(&stmt);
    walk(stmt.m_expr1);
    walk(stmt.m_expr2);
    BoolType boolType;
    if (stmt.m_expr->m_type && !isTypeCompatible(stmt.m_expr->m_type, &boolType))
        Error::complain("the for condtion expression type is wrong\n");
    walk(stmt.m_exprList);
    walk(stmt.m_stmt);
    popIterableStatement();
}

/// @brief TypeBuilder handler for foreach statement
// 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' (identifier|mapLiteral|setLitieral) ')' blockStatement
void TypeBuilder::accept(ForEachStatement &stmt) 
{
    pushIteralbeStatement(&stmt);
    
    for (int index = 0; index < stmt.m_varNumbers; index++) {
        walk(stmt.m_typeSpec[index]);
        walk(stmt.m_id[index]);
    }
    walk(stmt.m_expr);
    
    Type *type = NULL;
    Symbol *symbol = NULL; 
    switch (stmt.m_objectSetType) {
        case ForEachStatement::Object:
            // get the symbol and type
            symbol = getSymbol(stmt.m_objectSetName);
            if (!symbol)
                Error::complain("the symbol %s is not declared\n", stmt.m_objectSetName.c_str()); 
            type = getType(stmt.m_bojectSetName);
            if (!type)
                Error::complain("the symbol %s type is not declared in current scope\n",
                        stmt.m_objectSetName);
            // if the object set is map, check the var numbers
            if (type && isType(type, "map") {
                if(stmt.m_varNumbers != 2)
                    Error::complain("var numbers mismatch in foreach statement\n");
                else {
                    Type *keyType = getTypeBySpec(stmt.m_typeSpec[0]);
                    Type *valType = getTypeBySpec(stmt.m_typeSpec[1]);
                    if (!isTypeCompatible(keyType, type->getKeyType()))
                        Error::complain("the key variable and map key's type is mismatch\n");
                    if (!isTypeCompatible(valType, type->getValType()))
                        Error::complain("the val variable and map val's type is mismatch\n");
                }
            }
            else if (type && isType("set")) {
                if (stmt.m_varNumber != 1)
                    Error::complain("var numbers is too much in foreach statement\n");
                else {
                    Type *valType = getTypeBySpec(stmt.m_typeSpec[0]);
                    if (!isTypecompatible(type->getValType()), valType))
                        Error::complain("val type is mismatched with set type\n");
                }
            }
            else 
                Error::complain("the object %s is not set or map object\n", stmt.m_objectSetName.c_str());
            
            break;

        case ForEachStatement::SetObject: {
            SetExpr *setExpr = NULL;
            // example foreach(int var in [0, 1, 2])
            // check the variable numbers
            if (stmt.m_varNumbers > 1)
                Error::complain("too many variables in foreach statement\n");
            // check wether the variable's type is matched with set type
            type = getTypeBySpec(stmt.m_typeSpec[0]);
            setExpr = dynamic_cast<SetExpr *>(stmt.m_expr);
            if (!setExpr)
                Error::complain("the set expression in foreach statement is null\n");
            else {
                setType = dynamic_cast<SetType *>(setExpr->m_type);
                if (!setType)
                    Error::complain("the set expression type is null\n");
                else if (!isTypeCompatible(type, setType->getValType()))
                    Error::complain("the tpe is mismatch between variable and set\n");
                else
                    Error::complain("can not get the set type\n");
            }
            break;
        }
            
        case ForEachStatement::MapObject: {
            MapType *mapType = NULL;
            Type *keyType = NULL;
            Type *valTpe = NULL;
            MapExpr *mapExpr = dynamic_cast<MapExpr*>(stmt.m_expr);
            
            if (stmt.m_varNumbers != 2)
                Error::complain("less variables in foreach statement\n");
            else {
                keyType = getTypeBySpec(stmt.m_typeSpec[0]);
                valType = getTypeBySpec(stmt.m_typeSpec[1]);
                if (!mapExpr)
                    Error::complain("the map expression in foreach statement is null\n");
                else {
                    mapType = dynamic_cast<MapType *>(mapEpxr->m_type);
                    if (mapType && !isTypeCompatible(keyType, mapType->getKeyType()))
                        Error::complain("the key variable and map key's type is mismatch\n");
                    else if (mapType && !isTypeCompatible(valType, mapType->getValType()))
                        Error::complain("the val variable and map val's tpe is mismtach\n");
                    else
                        Error::complain("can not get the map type\n");
                }
            }        
            break;
        }
        default:
            Error::complain("unknow object set type in foreach statement\n");
            break;
    }
    // the expression type must be checked
    walk(stmt.m_stmt);
    pouIterableStatement();
}

/// @brief TypeBuilder handler for switch statement
void TypeBuilder::accept(SwitchStatement &stmt) 
{
    pushBreakableStatement(&stmt);
    // check the condition type
    walk(stmt.m_conditExpr);
    IntType intType;
    if (!isTypeCompatible(stmt.m_conditExpr, &intType))
        Error::complain("the switch condition type is wrongly declared\n");
       
    // for each case, iterate
    for (int index = 0; index < (int)stmt.m_cases.size(); index++) {
        std::pair<vector<Expr *>, Statement *> *block = &stmt.m_cases[index];
        if (block) {
            vector<Expr *> exprList = block->first;
            // iterate the expression list
            vector<Expr *>::iterator ite = exprList.begin();
            for (; ite != exprList.end(); ite++) {
                Expr *expr = *ite;
                walk(expr);
                if (!isTypeCompatible(expr->m_type, &intType))
                    Error::complain("the case type is wrongly declared\n");
            }
            // check the statement block
            walk(block->second);
        }
    }
    walk(stmt._mdefaultStmt);
    popBreakableStatement();
}
/// @brief TypeBuilder handler for continue statement
void TypeBuilder::accept(ContinueStatement &stmt) 
{
    // in compile phase, the continue statement error should be checked
    if (!getCurrrentIterableStatement())
        Error::complain("the continue statment is not rightly declared\n");
}

/// @brief TypeBuilder handler for break statement
void TypeBuilder::accept(BreakStatement &stmt) 
{
    if (!getCurrentBreakableStatement())
        Error::complain("the break statement is not rightly declared\n ")
    // in compile phase, the break statement error should be checked
}

/// @brief TypeBuilder handler for return statement
void TypeBuilder::accept(ReturnStatement &stmt) 
{
    if (!getCurrentMethod())
        Error::complain("the return statement is not declared in Method");
    // the expression type shoud be checked
    walk(stmt.m_resultExpr);
    // the return type and the Methodtype must be compatible
}

/// @brief TypeBuilder handler for throw statement
void TypeBuilder::accept(ThrowStatement &stmt) 
{
    walk(stmt.m_resultExpr);
}

/// @brief TypeBuilder handler for assert statement
void TypeBuilder::accept(AssertStatement &stmt) 
{
    walk (stmt.m_resultExpr);
    Bool boolType;
    if (!isTypeCompatible(stmt.m_resultExpr->m_type, &boolType))
        Error::complain("the assert expression shoul be bool type\n")        
}

/// @brief TypeBuilder handler for try statement
void TypeBuilder::accept(TryStatement &stmt) 
{
    walk(stmt.m_blockStmt);    
    vector<CatchStatement *>::iterator ite;
    for (ite = stmt.m_catchStmts.begin(); ite != stmt.m_catchStmts.end(); ite++) 
        walk(*ite);
    
    walk(stmt.m_finallyStmt);
}

/// @brief TypeBuilder handler for catch statement
void TypeBuilder::accept(CatchStatement &stmt) 
{
    if (!hasSymbol(stmt.m_type)) 
                Error::complain("the type is not declared\n", stmt.m_type.c_str());
    if (hasSymbol(stmt.m_id))
        Error::complain("the symbol %s has been defined\n", stmt.m_id.c_str());
    
    walk(stmt.m_block);
}
/// @brief TypeBuilder handler for finallycatch statement
void TypeBuilder::accept(FinallyCatchStatement &stmt) 
{
    walk(stmt.m_block);
}



/// @brief TypeBuilder handler for expression
void TypeBuilder::accept(Expr &expr)
{
    
}
/// @brief TypeBuilder handler for expression list expression
void TypeBuilder::accept(ExprList &list) 
{
    vector<Expr *>::iterator ite;
    for (ite = list.m_exprs.begin(); ite != list.m_exprs.end(); ite++)
        walk(*ite);
}

/// @brief TypeBuilder handler for binary op expression
void TypeBuilder::accept(BinaryOpExpr &expr) 
{
    walk(expr.m_left);
    walk(expr.m_right);
    if (!isTypeCompatible(expr.m_left->m_type, expr.m_right->m_type))
        Error::complain("type mismatch for binary expression\n");
}

/// @brief TypeBuilder handler for conditional expression
void TypeBuilder::accept(ConditionalExpr &expr) 
{
    
}

/// @brief TypeBuilder handler for logic or expression
void TypeBuilder::accept(LogicOrExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    BoolType boolType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &boolType))
            Error::complain("expression type is not right, expected bool type\n");
    }
}

/// @brief TypeBuilder handler for logic and expression
void TypeBuilder::accept(LogicAndExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    BoolType boolType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &boolType))
            Error::complain("expression type is not right, expected bool type\n");
    }
}

/// @brief TypeBuilder handler for bitwise or expression
void TypeBuilder::accept(BitwiseOrExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// @brief TypeBuilder handler for bitwise xor expression
void TypeBuilder::accept(BitwiseXorExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// @brief TypeBuilder handler for bitwise expression
void TypeBuilder::accept(BitwiseAndExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// @brief TypeBuilder handler for equality expression
void TypeBuilder::accept(EqualityExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// @brief TypeBuilder handler for relational expression
void TypeBuilder::accept(RelationalExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    BoolType boolType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &boolType))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// @brief TypeBuilder handler for shift expression
void TypeBuilder::accept(ShiftExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// @brief TypeBuilder handler for additive expression
void TypeBuilder::accept(AdditiveExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}
    
/// @brief TypeBuilder handler for multiplicative expression    
void TypeBuilder::accept(MultiplicativeExpr &expr) 
{
    walk(expr.m_target);
    
    vector<Expr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        Expr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->m_type &&
            !isTypeCompatible(subExpr->m_type, &intype))
            Error::complain("expression type is not right, expected int type\n");
    }
}

/// helper method
/// @brief compile an continue selector with an id, or methods call
/// @param curType: the current type, which is an unaryExpr
/// @param curID: current id
/// @param elements: an consecutive selectors
void TypeBuilder::handleSelectorExpr(PrimaryExpr *primExpr,
                    std::vector<SelectorExpr *> elements) 
{
    if (primExpr->m_type != PrimaryExpr::T_IDENTIFIER ||
        primExpr->m_type != PrimaryExpr::T_SELF ||
        primExpr->m_type != PrimaryExpr::T_SUPER )
            return;
     
    // check wether the id is declared in current scope
    Type * type = getType(primExpr->m_text);
    if (!type) {
        Error::complain("the identifier %s is not declared %s\n", 
                primExpr->m_text.c_str());
        return;
    }
    
    std::vector<SelectorExpr *>::iterator ite;
    string curText = primExpr->m_text;
    for (ite = elements.begin(); ite != elements.end(); ite++) {        
        SelectionExpr *selector = static_cast<SelectorExpr *>(*ite);
        
        if (selector->m_type == SelectionExpr::T_DOT) {
            // check wether the member is in current scope
            if (type && type->getSlot(selector->m_text)) {
                Error::complain("the identifier %s is not declared in %s scope\n", 
                                selector.m_text.c_str(),
                                type->getName().c_str());
                type = type->getSlot(selector->m_text);
                curText = selector->m_text;
            }
            else {
                Error::complain("current type is null\n");
            }
        }
        else if (selector->m_type == SelectionExpr::T_ARRAY) {
            if (curText == "self" || curText == "super")
                Error::complain("it is not right to apply array selector to self or super keyword\n");
            else {
                if (!type)
                    Error::complain("the %s is not declared\n", curText.c_str());
                else if (type && !type->isEnumerable())
                    Error::complain("the %s is not enumerable object\n", curText.c_str());
                else
                    type = type->getSlot(0);
        
            }
        }
        else if (selector->m_type == SelectionExpr::T_ARGUMENTS) {
            if (curText == "self" || curText == "super")
                Error::complain("it is not right to take self as Method");
            else {
                // check wether the method call is defined in current scope
                MethodType * methodType = getType(curText);
                if (!methodType) {
                    Error::complain("the method %s is not defined\n", curText.m_cstr())
                }
                MethodCallExpr * methodCallExpr = selector->m_methodCallExpr;
                methodCallExpr->setMethodName(curText);
                walk(methodCallExpr);
                // for next selector, the method return type must be known
                type = methodType->getReturnType();
            }
        }
        else
            Error::complain("unknow selector\n");
    }    
}
   
/// @brief TypeBuilder handler for unary expression    
void TypeBuilder::accept(UnaryExpr &expr) 
{
    // if the primary expression is constant value, just return
    vector<Expr *>::iterator ite;
    PrimaryExpr *primExpr = dynamic_cast<PrimayExpr *>(expr.m_target);
    if (!primExpr){
        Error::complain("unaryExpr has not a primaryExpr\n");
        return;
    }
    
    switch (primExpr->m_type) {
        case PrimaryExpr::T_NUMBER:
        case PrimaryExpr::T_NULL:
        case PrimaryEpxression::T_TRUE:
        case PrimaryExpr::T_FALSE:
        case PrimaryExpr::T_STRING:
        case PrimaryExpr::T_HEX_NUMBER:
            if (m_elements.size() > 0)
                Error::complain("the constant expression can not have a selector expression\n");
            return; 
        case PrimaryExpr::T_IDENTIFER:
            // check to see wether the identifier is defined in current scope
            if (!hasSymbol(primExpr->m_text)) {
                Error::complain("the symbol %s is not defined in current scope \n",
                                expr.m_text.c_str());
            }
            Type *type = getType(primExpr->m_text);
            handleSelectorExpr(type, primExpr->m_text, expr.m_elements);
            break;
            
        case PrimaryExpr::T_SELF: {
            Class * cls = getCurrentClass();
            ClassType *clsType = NULL;
            if (!cls)
                Error::complain("the self keyword can not be used in class context\n");
            else if ((clsType = getType(cls->m_name)) == NULL)
                Error::complain("the class %s is not declared\n", cls->m_name.c_str());
            handleSelectorExpr(clsType, primExpr->m_text, expr.m_elements);
            break;
        }
            
        case PrimaryExpr::T_SUPER: {
            Class *cls = getCurrentClass();
            if (!cls)
                Error::complain("the super keyword is not used in class context\n"); 
            else if (!cls->isInheritClass())
                Error::complain("the class has not base class\n");
            handleSelectorExpr(type, primExpr->m_text, expr.m_elements);
            break;
        }
  
        case PrimaryExpr::T_COMPOUND:
            walk(expr.m_expr);
            break;
        default:
            break;
    }
}
void TypeBuilder::accept(MethodCallExpr &expr) 
{
    vector<Expr *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++)
        walk(*ite);    
}

/// @brief TypeBuilder handler for primary expression
void TypeBuilder::accept(PrimaryExpr &expr) 
{
}

/// @brief TypeBuilder handler for selector expression
void TypeBuilder::accept(SelectorExpr &expr) 
{
    vector<Expr *>::iterator ite;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++)
        walk(*ite);
    
}

/// @brief TypeBilder handler for new expression
void TypeBuilder::accept(NewExpr &expr) 
{
    // first, check wether the type is right
    if (!hasType(expr.m_type))
        Error::complain("the type %s doesn't exit\n", expr.m_type.c_str());
    
    // check wether the arguments is right
    vector<Expr*>::iterator i = expr.m_arguments.begin();
    for (; i != expr.m_aguments.end(); i++) 
        walk(*ite);
}

// @brief TypeBuilder handler for map, such as map b = {0:1, 1:1} 
void TypeBuilder::accept(SetExpr &expr) 
{
    walk(expr.m_exprList);
    // TODO set the expression type 
}

// @brief TypeBuilder handler for map, such as map<int,int> b = {0:1, 1:1} 
void TypeBuilder::accept(MapExpr &expr) 
{
    vector<MapItemExpr*>::iterator i = expr.m_items.begin();
    while (i != expr.m_items.end()) {
        MapItemExpr *item = *i;
        walk(item);
    }
    // TODO set the expression type
}

/// @breif Typebuilder handler for map item
void TypeBuilder::accept(MapItemExpr &expr) 
{
    walk(expr.m_key);
    walk(expr.m_val);
    // TODO: set the expression type
}

