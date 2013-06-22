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
#include "Exception.h"
#include "Preproc.h"

/// @brief Constructor
TypeBuilder::TypeBuilder(const string &path, const string &file) 
{
    m_file = file;
    m_path = path;
    m_rootScope = new Scope(); 
    m_curScope = m_rootScope; 
    if (!m_path.empty()) {
        m_fullName = m_path;
        m_fullName += "/";
        m_fullName += m_file;
    }
    else
        m_fullName = m_file;
}

/// @brief Destructor
TypeBuilder::~TypeBuilder() 
{
    if (m_curScope != m_rootScope) {
       throw Exception::InvalidScope("root");
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
    Type *type = NULL; 
    if (m_typeDomain)
        m_typeDomain->getType(name, &type);
    return (type != NULL);
    
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
    if (m_typeDomain != NULL)
        m_typeDomain->getType(name, &type);
    return type;
}

void TypeBuilder::setWetherIncludedFile(bool w)
{
    m_isIncludedFile = w;
}
bool TypeBuilder::isIncludedFile()
{
    return m_isIncludedFile;
}

void TypeBuilder::build(AST* ast, TypeDomain *typeDomain)
{
    m_typeDomain = typeDomain; 
    walk(ast);
}

/// @brief Get type by type specifier
Type* TypeBuilder::getType(TypeSpec *typeSpec, bool nested)
{
    if (m_curScope && typeSpec)
        return m_curScope->resolveType(typeSpec->m_name);
    else
        return NULL;
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
    if (type && m_typeDomain)
        m_typeDomain->addType(type->getName(), type, m_fullName);
}

/// @brief helper Methodo walk ast node
void TypeBuilder::walk(AST *node) 
{
    if (node)
        node->walk(this);
}

///
bool TypeBuilder::isBuildComplete()
{
    return true; // temp
}
/// @brief Typebuilder handler for type specifier
void TypeBuilder::accept(TypeSpec &typeSpec) 
{
    Type *type = getType(typeSpec.m_name);
    if (!type) 
        Error::complain(typeSpec, "the type is not declared", typeSpec.m_name.c_str());        
}


/// @brief TypeBuilder handler for Variable
void TypeBuilder::accept(Variable &var) 
{
    Type *type = NULL;
    bool isvalid = true;
    
    // check to see wether the type of var is right
    if (var.m_typeSpec == NULL) {
        Error::complain(var,
                "the type of variable is not declared", var.m_name.c_str());
        isvalid = false;
    }
    else if ((type = getType(var.m_typeSpec)) == NULL) {
        Error::complain(var, 
                "the type %s is not declared", var.m_typeSpec->m_name.c_str());
        isvalid = false;    
    }
    
    // check to see wether the variable exist
    if (hasSymbol(var.m_name)) {
        Error::complain(var,
                "the variable %s is already declared", var.m_name.c_str());
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
                Error::complain(var,
                        "the global variable %s is initialized with non const value", 
                        var.m_name.c_str());
            }
            else if (!isTypeCompatible(type, var.m_expr->m_type)) {
                Error::complain(var,
                        "the global variable %s is initialized with no right type", var.m_name.c_str());
            }
            else
                var.m_initializedVal = var.m_expr->m_value;
        }
    }
    
    
    // if the variable is class variable
    else if (var.m_isOfClass) {
        // get class from the current scope
        if (!getType(var.m_typeSpec)) {
            if (var.m_isInitialized && var.m_expr) {
                walk(var.m_expr);
                // check to see wether the val is const
                if (!var.m_expr->m_value.isConst()) {
                    Error::complain(var,
                            "the class variable %s is initialized with non const value", 
                            var.m_name.c_str());
                }
                else if (!isTypeCompatible(type, var.m_expr->m_type)) {
                    Error::complain(var,
                            "the class variable %s is initialized with no right type",
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
                Error::complain(var,
                        "the local variable %s is initialized with non const value", 
                        var.m_name.c_str());
            }
            else if (!isTypeCompatible(type, var.m_expr->m_type)) {
                Error::complain(var,
                        "the local variable %s is initialized with no right type",
                        var.m_name.c_str());
            }
        }
    }
    // define symbol in current scope 
    Symbol *symbol = new Symbol();
    symbol->m_name = var.m_name;
    symbol->m_type = type;
    if (var.m_isGlobal)
        symbol->m_storage = Symbol::GlobalSymbol;
    else
        symbol->m_storage = Symbol::LocalStackSymbol;
    defineSymbol(symbol);
}

/// @brief Handler for method type builder
void TypeBuilder::accept(Method &method) 
{
    bool isvalid = true;
    Type *returnType = NULL;

    // check to see wether the return type of method is  declared
    if (!method.m_retTypeSpec) {
        Error::complain(method, "the method type is not declared");
        isvalid = false;
    }
    else if ((returnType = getType(method.m_retTypeSpec)) == NULL) {
        Error::complain(method,
                "the method '%s' return type '%s' is not declared", 
                method.m_name.c_str(), method.m_retTypeSpec->m_name.c_str());
        isvalid = false;
    }
    // check to see wether the method name has been declared
    MethodType *methodType = (MethodType *)getType(method.m_name, true);
    if (methodType) {
        Error::complain(method,
                "method %s is already declared", method.m_name.c_str());
        isvalid = false;
    }
    
	// set the current scope
	enterScope(method.m_name, dynamic_cast<Scope*>(&method));
    
    // if the method is  a member of class or interface,
    // the method will be in VTBL of the class and interface
    if (method.m_isOfClass || method.m_isOfProtocol) {
        // check to see wether there is the Methodn VTBL
        ClassType *clsType = (ClassType *)getType(method.m_class);
        if (!clsType) {
            Error::complain(method,
                    "the method '%s'is not member of class %s", 
                    method.m_name.c_str(), method.m_class.c_str());
            isvalid = false;
        }
        
        // get VTBL of the class
        if (clsType) {
            ObjectVirtualTable *vtbl = clsType->getVirtualTable();
            if (!vtbl) {
                Error::complain(method,
                    "The class %s has not VTBL", clsType->getName().c_str());
                isvalid = false;
            }
        
            // check to see wether the VTBL have the Method       
            if (vtbl) {
                MethodType *type = (MethodType*)vtbl->getSlot(method.m_name);
                if (!type) {
                    Error::complain(method,
                        "the class %s has not the Methods",
                        clsType->getName().c_str(),
                        method.m_name.c_str());
                        isvalid = false;
                }
            }
        }
    } 
    if (isvalid) {
        // define Methodye in current scope
        MethodType *methodType = new MethodType();
        methodType->setName(method.m_name);
        defineType(methodType);
        
        // define method symbol in current scope
        Symbol *symbol = new Symbol();
        symbol->m_type = methodType;
        symbol->m_name = method.m_name;
        defineSymbol(symbol);;
        
        // if the method is member of class
        if (method.m_isOfClass) {
            ClassType *clsType = (ClassType *)getType(method.m_class);
            if (clsType)
                clsType->addSlot(method.m_name, methodType);
            else
                Error::complain(method,
                        "the class %s is not declared", method.m_class.c_str());
        }
        
        // if the method is  member of interface
        else if (method.m_isOfProtocol) {
            ProtocolType *protocolType = (ProtocolType *)getType(method.m_protocol);
            if (protocolType)
                protocolType->addSlot(method.m_name, methodType);
            else
                Error::complain(method,
                        "the protocol %s is not declaired", method.m_protocol.c_str());
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
    for (; ite != list.m_parameters.end(); ite++) {
        // check the parameter
        MethodParameter *methodParameter = *ite;
        walk(methodParameter);
        
        // check wether there are same variable's name
        vector<MethodParameter *>::iterator ip;
        for (ip = list.m_parameters.begin(); ip != list.m_parameters.end(); ip++) {
            MethodParameter *second = *ip;
            if (ite != ip && methodParameter->m_name == second->m_name) {
                Error::complain(list,
                        "there are same variable's name %s", 
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
    if (!getType(para.m_typeSpec)) {
        Error::complain(para,
                "the parameter's type %s is not declared", 
                para.m_typeSpec->m_name.c_str());
        isvalid = false;
    }
    // check the parameter's name
    if (getSymbol(para.m_name)) {
        Error::complain(para,
                "the parameter %s is already declared in current scope", 
                para.m_name.c_str());
        isvalid = false;
    }
    
    // if the parameter has default value, 
    // check wethere the expression's type is same with variable's type
    if (para.m_hasDefault && para.m_default != NULL) {
        Type *type = getType(para.m_typeSpec);
        if (type && isTypeCompatible(type, para.m_default->m_type)) {
            Error::complain(para,
                    "the parameter %s is not rightly initialized",
                    para.m_name.c_str());
        }
    }
    // define the passed parameter in current symbol talbe
    Symbol *symbol = new Symbol();
    symbol->m_name = para.m_name;
    symbol->m_type = getType(para.m_typeSpec);
    // if the Methods called, all parameters are pushed by caller
    // so the address of each parameter must be knowned
    symbol->m_storage = Symbol::LocalStackSymbol;
    symbol->m_addr = para.m_index * 4;  // the index is offset 
    defineSymbol(symbol);
    
}

/// @brief TypeBuilder handler for MethodBlock
void TypeBuilder::accept(MethodBlock &block) 
{
    int index = 1;
    vector<Variable *>::iterator v = block.m_vars.begin();
    for (; v != block.m_vars.end(); v++) {
        Variable * var = *v;
        Symbol *symbol = getSymbol(var->m_name);
        symbol->m_storage = Symbol::LocalStackSymbol;
        symbol->m_addr = index *(-4);
        index++;
    }
    vector<Statement *>::iterator ite = block.m_stmts.begin();
    for (; ite != block.m_stmts.end(); ite++) 
        walk(*ite);
}


/// @brief TypeBuilder handler for Class
void TypeBuilder::accep(Class &cls) 
{
    bool isvalid = true;
    // check wether the class name exist?
	bool nested = (cls.m_isPublic == true)? true:false;
    if (hasSymbol(cls.m_name, nested)) {
        Error::complain(cls,
                "the class name %s is already defined", cls.m_name.c_str());
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
            Error::complain(cls, "the base class %s can not be same with class %s",
                        baseClass.c_str(), cls.m_name.c_str()); 
        ClassType *clsType = (ClassType *)getType(baseClass);                  
        if (!clsType)
            Error::complain(cls, "the base class  %s is not declared", baseClass.c_str());
        else if (clsType->isFinal())
            Error::complain(cls, "the base class %s is final, can not be inherited", baseClass.c_str());
    }   
    
    // check to see wether the class implements protocol exist
    for (ite = cls.m_protocols.begin(); ite != cls.m_protocols.end(); ite++) {
        string protocolName = *ite;
        if (protocolName == cls.m_name) {
            Error::complain(cls, "the protocol name can not be same  withe class %s",
                    protocolName.c_str(), cls.m_name.c_str());
        }
        // the methd exported by protocol must be implemented in class
        ProtocolType *protocolType = (ProtocolType *)getType(protocolName);
        if (!protocolType) 
            Error::complain(cls, "the protocol %s is not declared", protocolName.c_str());
        else {
            for (int index = 0; index < protocolType->getSlotCount(); index++) {
                // for each slot in protocol, to check wether it is in class
                Type *slot = protocolType->getSlot(index);
                if (!cls.getMethod(slot->getName())) {
                    Error::complain(cls, "the method %s exported by protocol %s is not implemented in class %s",
                                slot->getName().c_str(), protocolName.c_str(), cls.m_name.c_str());
                }
            }
        }  
    }
    
    // walk through the class block
    walk(cls.m_block);
    exitScope();
    popClass();

}

/// @brief TypeBuilder handler for ClassBlock
void TypeBuilder::accept(ClassBlock &block) 
{
    // iterate all variables
    vector<Variable*>::iterator v = block.m_vars.begin();
    while(v != block.m_vars.end()) {
        walk(*v);
        v++;
    }
   // iterate all Method    
    vector<Method*>::iterator m = block.m_methods.begin();
    while (m != block.m_methods.end()) {
        walk(*m);
        m++;
    }
}

/// @brief TypeBuildef handler for Interface
void TypeBuilder::accept(Protocol &protocol) 
{
	// check wether the protocol is alread declared
    if (hasSymbol(protocol.m_name)) {
        Error::complain(protocol,
                "the protocol name %s is already declared", 
                protocol.m_name.c_str());
    }
    
    // protocol is also a scope
	enterScope(protocol.m_name, dynamic_cast<Scope*>(&protocol));
	
    // put the interface type in the current scope
    ProtocolType *protocolType = new ProtocolType(protocol.m_name, protocol.m_isPublic);
    defineType(protocolType);
        
    // put the interface symbol in the current scope
    Symbol *symbol = new Symbol();
    symbol->m_name = protocol.m_name;
    symbol->m_type = protocolType;
    defineSymbol(symbol);
    
    // iterall all method of the protocol
    vector<Method*>::iterator ite = protocol.m_methods.begin();
    for (; ite != protocol.m_methods.end(); ite++) {
        Method *method = *ite;
        if (method && method->m_protocol != protocol.m_name) {
            Error::complain(protocol, "the method is not member of %s", 
                    method->m_name.c_str(), protocol.m_name.c_str());
        }
        walk(method);
    }
    // exit the protocol scope
    exitScope();
    
}

/// @brief TypeBuilder handler for Statement
void TypeBuilder::accept(Statement &stmt) 
{
}

/// @brief TypeBuilder handler for include statement
void TypeBuilder::accept(IncludeStatement &stmt) 
{
   string fileName = m_path;
   if (!fileName.empty())
       fileName += "/";
   fileName += stmt.m_fullName;
   if (fileName.empty()) {
       Error::complain(stmt, "the include file is not rightly specified ");
        return;
   }
   // the preprocessor will deal with the file
   Preproc preproc(stmt.m_fullName, m_path);
   if (m_typeDomain)
       preproc.build(*m_typeDomain);
   // after preproc is done, all class information is got
}

/// @brief TypeBuilder handler for Block Statement
void TypeBuilder::accept(BlockStatement &blockStmt) 
{
	// set the current scope
	enterScope("blockStatement", dynamic_cast<Scope*>(&blockStmt));
    
    vector<Statement *>::iterator ite = blockStmt.m_stmts.begin();
    for (; ite != blockStmt.m_stmts.end(); ite++) 
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
        Type *varType = getType(stmt.m_var->m_typeSpec);
        if (!varType || !isTypeCompatible(varType, stmt.m_expr->m_type))
            Error::complain(stmt, "the variable %s is initialize with wrong type",
                    stmt.m_var->m_name.c_str());
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
        Error::complain(stmt, "the if condition type is wrong");
    
    // the expression type shoud be checked
    walk(stmt.m_ifBlockStmt);
    walk(stmt.m_elseBlockStmt);
}

/// @brief TypeBuilder handler for while statement
void TypeBuilder::accept(WhileStatement &stmt) 
{
    pushIterableStatement(&stmt);
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain(stmt, "the while condition type is wrong");
    
    walk(stmt.m_stmt);
    popIterableStatement();
}

/// @brief TypeBuilder handler for do while statement
void TypeBuilder::accept(DoStatement &stmt) 
{
    pushIterableStatement(&stmt);
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &boolType))
        Error::complain(stmt, "the do condition type is wrong");
    
    walk(stmt.m_stmt);
    popIterableStatement();
}
/// @brief TypeBuilder handler for for statement
void TypeBuilder::accept(ForStatement &stmt)
{
    pushIterableStatement(&stmt);
    walk(stmt.m_expr1);
    walk(stmt.m_expr2);
    BoolType boolType;
    if (stmt.m_expr2->m_type && !isTypeCompatible(stmt.m_expr2->m_type, &boolType))
        Error::complain(stmt, "the for condtion expression type is wrong");
    walk(stmt.m_exprList);
    walk(stmt.m_stmt);
    popIterableStatement();
}

/// @brief TypeBuilder handler for foreach statement
// 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' (identifier|mapLiteral|setLitieral) ')' blockStatement
void TypeBuilder::accept(ForEachStatement &stmt) 
{
    pushIterableStatement(&stmt);
    
    for (int index = 0; index < stmt.m_varNumbers; index++) {
        walk(stmt.m_typeSpec[index]);
        if (!stmt.m_typeSpec[index] && !hasSymbol(stmt.m_id[index]))
            Error::complain(stmt, "the identifier %s is not declared", stmt.m_id[index].c_str());
    }
    walk(stmt.m_expr);
    
    Type *type = NULL;
    Symbol *symbol = NULL; 
    switch (stmt.m_objectSetType) {
        case ForEachStatement::Object:
            // get the symbol and type
            symbol = getSymbol(stmt.m_objectSetName);
            if (!symbol)
                Error::complain(stmt, "the symbol %s is not declared", stmt.m_objectSetName.c_str()); 
            type = getType(stmt.m_objectSetName);
            if (!type)
                Error::complain(stmt, "the symbol %s type is not declared in current scope",
                        stmt.m_objectSetName.c_str());
            // if the object set is map, check the var numbers
            if (type && isType(type, "map")){
                MapType *mapType = dynamic_cast<MapType *>(type);
                if(stmt.m_varNumbers != 2)
                    Error::complain(stmt, "var numbers mismatch in foreach statement");
                else {
                    Type *keyType = getType(stmt.m_typeSpec[0]);
                    Type *valType = getType(stmt.m_typeSpec[1]);
                    if (!isTypeCompatible(keyType, mapType->getKeyType()))
                        Error::complain(stmt, "the key variable and map key's type is mismatch");
                    if (!isTypeCompatible(valType, mapType->getValType()))
                        Error::complain(stmt, "the val variable and map val's type is mismatch");
                }
            }
            else if (type && isType(type, "set")) {
                if (stmt.m_varNumbers != 1)
                    Error::complain(stmt, "var numbers is too much in foreach statement");
                else {
                    Type *valType = getType(stmt.m_typeSpec[0]);
                    SetType *setType = dynamic_cast<SetType *>(type);
                    if (!isTypeCompatible(setType->getValType(), valType))
                        Error::complain(stmt, "val type is mismatched with set type");
                }
            }
            else 
                Error::complain(stmt, "the object %s is not set or map object", stmt.m_objectSetName.c_str());
            
            break;

        case ForEachStatement::SetObject: {
            SetExpr *setExpr = NULL;
            // example foreach(int var in [0, 1, 2])
            // check the variable numbers
            if (stmt.m_varNumbers > 1)
                Error::complain(stmt, "too many variables in foreach statement");
            // check wether the variable's type is matched with set type
            SetType *setType = (SetType*) getType(stmt.m_typeSpec[0]);
            setExpr = dynamic_cast<SetExpr *>(stmt.m_expr);
            if (!setExpr)
                Error::complain(stmt, "the set expression in foreach statement is null");
            else {
                setType = dynamic_cast<SetType *>(setExpr->m_type);
                if (!setType)
                    Error::complain(stmt, "the set expression type is null");
                else if (!isTypeCompatible(type, setType->getValType()))
                    Error::complain(stmt, "the tpe is mismatch between variable and set");
                else
                    Error::complain(stmt, "can not get the set type");
            }
            break;
        }
            
        case ForEachStatement::MapObject: {
            MapType *mapType = NULL;
            MapExpr *mapExpr = dynamic_cast<MapExpr*>(stmt.m_expr);
            
            if (stmt.m_varNumbers != 2)
                Error::complain(stmt, "less variables in foreach statement");
            else {
                Type *keyType = getType(stmt.m_typeSpec[0]);
                Type *valType = getType(stmt.m_typeSpec[1]);
                if (!mapExpr)
                    Error::complain(stmt, "the map expression in foreach statement is null");
                else {
                    MapType * mapType = dynamic_cast<MapType *>(mapExpr->m_type);
                    if (mapType && !isTypeCompatible(keyType, mapType->getKeyType()))
                        Error::complain(stmt, "the key variable and map key's type is mismatch");
                    else if (mapType && !isTypeCompatible(valType, mapType->getValType()))
                        Error::complain(stmt, "the val variable and map val's tpe is mismtach");
                    else
                        Error::complain(stmt, "can not get the map type");
                }
            }        
            break;
        }
        default:
            Error::complain(stmt, "unknow object set type in foreach statement");
            break;
    }
    // the expression type must be checked
    walk(stmt.m_stmt);
    popIterableStatement();
}

/// @brief TypeBuilder handler for switch statement
void TypeBuilder::accept(SwitchStatement &stmt) 
{
    pushBreakableStatement(&stmt);
    // check the condition type
    walk(stmt.m_conditExpr);
    IntType intType;
    if (!isTypeCompatible(stmt.m_conditExpr->m_type, &intType))
        Error::complain(stmt, "the switch condition type is wrongly declared");
       
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
                    Error::complain(stmt, "the case type is wrongly declared");
            }
            // check the statement block
            walk(block->second);
        }
    }
    walk(stmt.m_defaultStmt);
    popBreakableStatement();
}
/// @brief TypeBuilder handler for continue statement
void TypeBuilder::accept(ContinueStatement &stmt) 
{
    // in compile phase, the continue statement error should be checked
    if (!getCurrentIterableStatement())
        Error::complain(stmt, "the continue statment is not rightly declared");
}

/// @brief TypeBuilder handler for break statement
void TypeBuilder::accept(BreakStatement &stmt) 
{
    if (!getCurrentBreakableStatement())
        Error::complain(stmt, "the break statement is not rightly declared");
    // in compile phase, the break statement error should be checked
}

/// @brief TypeBuilder handler for return statement
void TypeBuilder::accept(ReturnStatement &stmt) 
{
    if (!getCurrentMethod())
        Error::complain(stmt, "the return statement is not declared in Method");
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
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_resultExpr->m_type, &boolType))
        Error::complain(stmt, "the assert expression shoul be bool type");        
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
                Error::complain(stmt, "the type is not declared", stmt.m_type.c_str());
    if (hasSymbol(stmt.m_id))
        Error::complain(stmt, "the symbol %s has been defined", stmt.m_id.c_str());
    
    walk(stmt.m_block);
}
/// @brief TypeBuilder handler for finallycatch statement
void TypeBuilder::accept(FinallyCatchStatement &stmt) 
{
    walk(stmt.m_block);
}

void TypeBuilder::accept(ExprStatement &stmt)
{
    walk(stmt.m_target);
    vector<pair<string, Expr *> >::iterator ite;
    for (ite = stmt.m_elements.begin(); ite != stmt.m_elements.end(); ite++) {
        pair<string, Expr *> &item = *ite;
        walk(item.second);
    }
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
        Error::complain(expr, "type mismatch for binary expression");
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
            Error::complain(expr, "expression type is not right, expected bool type");
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
            Error::complain(expr, "expression type is not right, expected bool type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
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
            Error::complain(expr, "expression type is not right, expected int type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
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
            !isTypeCompatible(subExpr->m_type, &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// helper method
/// @brief compile an continue selector with an id, or methods call
/// @param curType: the current type, which is an unaryExpr
/// @param curID: current id
/// @param elements: an consecutive selectors
void TypeBuilder::handleSelectorExpr(PrimaryExpr &primExpr,
                    std::vector<SelectorExpr *> &elements) 
{
    if (primExpr.m_type != PrimaryExpr::T_IDENTIFIER ||
        primExpr.m_type != PrimaryExpr::T_SELF ||
        primExpr.m_type != PrimaryExpr::T_SUPER )
            return;
     
    // check wether the id is declared in current scope
    Type * type = getType(primExpr.m_text);
    if (!type) {
        Error::complain(primExpr, "the identifier %s is not declared %s", 
                primExpr.m_text.c_str());
        return;
    }
    
    std::vector<SelectorExpr *>::iterator ite;
    string curText = primExpr.m_text;
    for (ite = elements.begin(); ite != elements.end(); ite++) {        
        SelectorExpr *selector = static_cast<SelectorExpr *>(*ite);
        
        if (selector->m_type == SelectorExpr::DOT_SELECTOR) {
            // check wether the member is in current scope
            if (type && type->getSlot(selector->m_id)) {
                Error::complain(primExpr, "the identifier %s is not declared in %s scope", 
                                selector->m_id.c_str(),
                                type->getName().c_str());
                type = type->getSlot(selector->m_id);
                curText = selector->m_id;
            }
            else {
                Error::complain(primExpr, "current type is null");
            }
        }
        else if (selector->m_type == SelectorExpr::ARRAY_SELECTOR) {
            if (curText == "self" || curText == "super")
                Error::complain(primExpr, "it is not right to apply array selector to self or super keyword");
            else {
                if (!type)
                    Error::complain(primExpr, "the %s is not declared", curText.c_str());
                else if (type && !type->isEnumerable())
                    Error::complain(primExpr, "the %s is not enumerable object", curText.c_str());
                else
                    type = type->getSlot(0);
        
            }
        }
        else if (selector->m_type == SelectorExpr::METHOD_SELECTOR) {
            if (curText == "self" || curText == "super")
                Error::complain(primExpr, "it is not right to take self as method");
            else {
                // check wether the method call is defined in current scope
                MethodType * methodType = (MethodType *) getType(curText);
                if (!methodType) {
                    Error::complain(primExpr, "the method %s is not defined", curText.c_str());
                }
                MethodCallExpr * methodCallExpr = selector->m_methodCallExpr;
                methodCallExpr->setMethodName(curText);
                walk(methodCallExpr);
                // for next selector, the method return type must be known
                type = methodType->getReturnType();
            }
        }
        else
            Error::complain(primExpr, "unknow selector");
    }    
}
   
/// @brief TypeBuilder handler for unary expression    
void TypeBuilder::accept(UnaryExpr &expr) 
{
    // if the primary expression is constant value, just return
    vector<Expr *>::iterator ite;
    PrimaryExpr *primExpr = dynamic_cast<PrimaryExpr *>(expr.m_primary);
    if (!primExpr){
        Error::complain(expr, "unaryExpr has not a primaryExpr");
        return;
    }
    
    switch (primExpr->m_type) {
        case PrimaryExpr::T_NUMBER:
        case PrimaryExpr::T_NULL:
        case PrimaryExpr::T_TRUE:
        case PrimaryExpr::T_FALSE:
        case PrimaryExpr::T_STRING:
        case PrimaryExpr::T_HEX_NUMBER:
            if (expr.m_selectors.size() > 0)
                Error::complain(expr, "the constant expression can not have a selector expression");
            return; 
        case PrimaryExpr::T_IDENTIFIER: {
            // check to see wether the identifier is defined in current scope
            if (!hasSymbol(primExpr->m_text)) {
                Error::complain(expr, "the symbol %s is not defined in current scope",
                                primExpr->m_text.c_str());
            }
            Type *type = getType(primExpr->m_text);
            handleSelectorExpr(*primExpr, expr.m_selectors);
            break;
        }
            
        case PrimaryExpr::T_SELF: {
            Class * cls = getCurrentClass();
            ClassType *clsType = NULL;
            if (!cls)
                Error::complain(expr, "the self keyword can not be used in class context");
            else if ((clsType = (ClassType *)getType(cls->m_name)) == NULL)
                Error::complain(expr, "the class %s is not declared", cls->m_name.c_str());
            handleSelectorExpr(*primExpr, expr.m_selectors);
            break;
        }
            
        case PrimaryExpr::T_SUPER: {
            Class *cls = getCurrentClass();
            if (!cls)
                Error::complain(expr, "the super keyword is not used in class context"); 
            else if (!cls->isInheritClass())
                Error::complain(expr, "the class has not base class");
            handleSelectorExpr(*primExpr, expr.m_selectors);
            break;
        }
  
        case PrimaryExpr::T_COMPOUND:
            walk(expr.m_primary);
            break;
        default:
            break;
    }
}
void TypeBuilder::accept(MethodCallExpr &expr) 
{
    vector<Expr *>::iterator ite;
    for (ite = expr.m_arguments.begin(); ite != expr.m_arguments.end(); ite++)
        walk(*ite);    
}

/// @brief TypeBuilder handler for primary expression
void TypeBuilder::accept(PrimaryExpr &expr) 
{
}

/// @brief TypeBuilder handler for selector expression
void TypeBuilder::accept(SelectorExpr &expr) 
{
}

/// @brief TypeBilder handler for new expression
void TypeBuilder::accept(NewExpr &expr) 
{
    // first, check wether the type is right
    if (!hasType(expr.m_type))
        Error::complain(expr, "the type %s doesn't exit", expr.m_type.c_str());
    
    // check wether the arguments is right
    vector<Expr*>::iterator i = expr.m_arguments.begin();
    for (; i != expr.m_arguments.end(); i++) 
        walk(*i);
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


void TypeBuilder::pushMethod(Method *method)
{
    m_methods.push_back(method);
}

void TypeBuilder::popMethod()
{
    if (!m_methods.empty())
        m_methods.pop_back();
}

Method* TypeBuilder::getCurrentMethod()
{
    if (!m_methods.empty())
        return m_methods.back();
    else
        return NULL;
}

void TypeBuilder::pushIterableStatement(Statement *stmt)
{
    if (stmt)
        m_iterableStmts.push_back(stmt);
}
void TypeBuilder::popIterableStatement()
{
    if (!m_iterableStmts.empty())
        m_iterableStmts.pop_back();
}
Statement* TypeBuilder::getCurrentIterableStatement()
{
    if (!m_iterableStmts.empty())
        return m_iterableStmts.back();
    else
        return NULL;
}
    
void TypeBuilder::pushBreakableStatement(Statement *stmt)
{
    if (stmt)
        m_breakableStmts.push_back(stmt);
}
void TypeBuilder::popBreakableStatement()
{
    if (!m_breakableStmts.empty())
        m_breakableStmts.pop_back();
}
Statement* TypeBuilder::getCurrentBreakableStatement()
{
    if (!m_breakableStmts.empty())
        return m_breakableStmts.back();
    else
        return NULL;
}
    
void TypeBuilder::pushClass(Class *cls)
{
    if (cls)
        m_clss.push_back(cls);
}
void TypeBuilder::popClass()
{
    if (!m_clss.empty())
        m_clss.pop_back();
}

Class* TypeBuilder::getCurrentClass()
{
    if (!m_clss.empty())
        return m_clss.back();
    else
        return NULL;
}

