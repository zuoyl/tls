//
//  tl-type-builder.cpp
//  A toyable language compiler (like a simple c++)


#include "tl-error.h"
#include "tl-os.h"
#include "tl-frame.h"
#include "tl-ast-decl.h"
#include "tl-ast-stmt.h"
#include "tl-ast-expr.h"
#include "tl-exception.h"
#include "tl-token.h"
#include "tl-lexer.h"
#include "tl-parser.h"
#include "tl-ast-builder.h"
#include "tl-type-builder.h"

using namespace tlang;

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
void TypeBuilder::enterScope(Scope *scope) 
{
    Assert(scope != NULL); 
    scope->setParentScope(m_curScope);
    m_curScope = scope;
}

/// @brief Exit the current scope
void TypeBuilder::exitScope() 
{
    m_curScope = m_curScope->getParentScope();
}

/// @brief Get Object by name 
Object* TypeBuilder::getObject(const string &name, bool nested) 
{
    Assert(m_curScope != NULL);

    Object *object = m_curScope->resolveObject(name, nested);
    return object;
}

/// @brief Define a new symbo in current scope
void TypeBuilder::defineObject(Object *object) 
{
    Assert(m_curScope != NULL); 
    Assert(object != NULL);     
    m_curScope->defineObject(object);
}

/// @brief get type by name
Type* TypeBuilder::getType(const string &name, bool nested) 
{
    Assert(m_typeDomain != NULL);  
    
    Type *type = NULL; 
    // check the builtin type domain 
    string domain = "builtin"; 
    m_typeDomain->getType(domain, name, &type);
    if (type)
        return type;
    // wethere the name is class 
    domain = "classType";
    m_typeDomain->getType(domain, name, &type);
    return type;
}

/// @brief Get type by type specifier
Type* TypeBuilder::getType(ASTTypeDecl *typeDecl, bool nested)
{
    if (typeDecl) {
        string typeName; 
        if (typeDecl->m_isQualified) {
            // now, only support non-qualifed name
            typeDecl->m_qualifiedName.getWholeName(typeName);  
            return getType(typeName);
        }
        else
            return getType(typeDecl->m_name);
    }
    else
        return NULL;
}
/// define a type in a specific domain
void TypeBuilder::defineType(const string &domain, Type *type)
{
    Assert(m_typeDomain != NULL);
    if (type)
        m_typeDomain->addType(domain, type->getName(), type);
}

/// @brief Define a new type in current scope
void TypeBuilder::defineType(Type *type)
{
    Assert(m_typeDomain != NULL);
    if (type) {
        string domain = "classType";
        m_typeDomain->addType(domain, type->getName(), type);
    }
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

void TypeBuilder::build(AST *ast, TypeDomain *typeDomain)
{
    Assert(typeDomain != NULL); 
    m_typeDomain = typeDomain; 
    walk(ast);
}


/// Decls
void TypeBuilder::accept(ASTCompileUnit &unit)
{
    vector<AST*>::iterator ite = unit.m_childs.begin();
    for (; ite != unit.m_childs.end(); ite++) 
        walk(*ite);
}

void TypeBuilder::accept(ASTDeclaration &decl)
{}

void TypeBuilder::accept(ASTPackageDecl &decl)
{}

void TypeBuilder::accept(ASTImportDecl &decl)
{}

void TypeBuilder::accept(ASTAnnotation &annotation)
{}

/// @brief TypeBuilder handler for Class
void TypeBuilder::accept(ASTClass &cls) 
{
    // check wether the class name exist?
	bool nested = (cls.isPublic() == true)? true:false;
    if (getObject(cls.m_name, nested)) {
        Error::complain(cls,
                "class name '%s' is already defined", cls.m_name.c_str());
    }
    // set current class
    pushClass(&cls);

    // put the class Type int the current scope
    ClassType *clsType = new ClassType(cls.m_name, m_curScope, cls.isPublic());
    clsType->setFinal(cls.isFinal()); 
    defineType(clsType);

    // the class is also scope
	enterScope(dynamic_cast<Scope *>(&cls));
    
    // check wether the base class exist
    if (!cls.m_baseClsName.empty()) {
        string baseClsName;
        cls.m_baseClsName.getWholeName(baseClsName);
        if (baseClsName == cls.m_name)
            Error::complain(cls, 
                "base class '%s' can not be same with class '%s'",
                baseClsName.c_str(), cls.m_name.c_str()); 
        // check wether the base class is already declared 
        ClassType *baseClsType = (ClassType *)getType(baseClsName);                  
        if (!baseClsType)
            Error::complain(cls, 
                "base class  '%s' is not declared", 
                baseClsName.c_str());
        
        // check wether the base class is final, class can not inherit final class 
        else if (baseClsType->isFinal())
            Error::complain(cls, 
                "base class '%s' is final, can not be inherited", 
                baseClsName.c_str());
        // copy the data slots from base class into sub class
        else {
            int count = baseClsType->getSlotCount();
            for (int index = 0; index < count; index++) {
                string name;
                Type *type = NULL;
                baseClsType->getSlot(index, name, &type);
                if (name.empty() || !type)
                    Error::complain(cls, 
                            "slot is not right in class", baseClsName.c_str());
                // only the public slot is copied 
                else if (type->isPublic())
                    clsType->insertSlot(index, name, type);
            }
        }
    } 
    // check wether the abstract class is declared
    vector<QualifiedName>::iterator ite = cls.m_abstractClsList.begin();
    for (; ite != cls.m_abstractClsList.end(); ite++) {
        QualifiedName &qualifiedName =  *ite;
        string name;
        qualifiedName.getWholeName(name);
        if (name == cls.m_name) 
            Error::complain(cls, 
                    "abstract class name'%s' can not be same  withe class '%s'",
                    name.c_str(), cls.m_name.c_str());
        // the methd exported by abstract class must be implemented in class
        ClassType *aclsType = (ClassType *)getType(name);
        if (!aclsType) 
            Error::complain(cls, 
                    "abstract class '%s' is not declared", name.c_str());
    }
    
    // walk through the class Decl 
    vector<ASTDeclaration *>::iterator i = cls.m_declarations.begin();
    for (; i != cls.m_declarations.end(); i++)
        walk(*i);
   
    // check to see wether the class implements abstract methods 
    ite = cls.m_abstractClsList.begin();
    for (; ite != cls.m_abstractClsList.end(); ite++) {
        QualifiedName &qualifiedName =  *ite;
        string baseClassName;
        qualifiedName.getWholeName(baseClassName);
        // the methd exported by abstract class must be implemented in class
        ClassType *aclsType = (ClassType *)getType(baseClassName);
        if (!aclsType) 
            continue;
        int slotCount = aclsType->getSlotCount(); 
        for (int index = 0; index < slotCount; index++) {
            // for each slot in abstract class, to check wether it is in class
            string name;
            Type *slot = NULL;
            aclsType->getSlot(index, name, &slot);
            if (!clsType->getVirtualTable()->getSlot(slot->getName())) {
                Error::complain(cls, 
                        "method '%s' declared in abstract class '%s' "
                        "is not implemented in class '%s'",
                        slot->getName().c_str(), 
                        baseClassName.c_str(), 
                        cls.m_name.c_str());
            }
        }
    }
    exitScope();
    popClass();
}


/// @brief TypeBuilder handler for Variable
void TypeBuilder::accept(ASTVariable &var) 
{
    Type *type = NULL;
    
    // check to see wether the type of var is right
    if (var.m_typeDecl == NULL) {
        Error::complain(var,
                "type of variable '%s' is not declared", var.m_name.c_str());
    } 
    else {
        walk(var.m_typeDecl); 
        type = getType(var.m_typeDecl);
    }
    // check to see wether the variable exist
    if (getObject(var.m_name)) {
        Error::complain(var,
                "variable '%s' is already declared", var.m_name.c_str());
    }
    
    if (var.m_isGlobal) {
        m_globalVars.push_back(&var);
        
        // if he variable is global and it is initialized
        // the initialization express must be directy evaluated,
        // at the same time , we should check wether the initialization expression's
        // type is compatible withe the variable's type
        if (var.m_isInitialized && var.m_expr) {
            walk(var.m_expr);
            // check to see wether the val is const
            if (!var.m_expr->isConstant()) 
                Error::complain(var,
                        "global variable '%s' is initialized with non const value", 
                        var.m_name.c_str());
            
            if (!isTypeCompatible(type, var.m_expr->getType())) {
                Error::complain(var,
                        "global variable '%s' is initialized with no right type",
                        var.m_name.c_str());
            }
           //  else
           //     var.m_initializedVal = var.m_expr->m_value;
        }
        // for local object, define object in current scope 
        Object *object = new Object(var.m_name, type);
        object->setStorage(Object::HeapObject);
        defineObject(object);
    }
    
    
    // if the variable is class member variable
    else if (var.m_isOfClass) {
        // if the class type exist, add the variable into it
        ClassType *clsType = dynamic_cast<ClassType  *>(getType(var.m_class)); 
        if (!clsType)
            Error::complain(var, "class '%s' is not declared", var.m_class.c_str());
        else
            clsType->addSlot(var.m_name, type);

        // get class from the current scope
        if (var.m_isInitialized && var.m_expr) {
            walk(var.m_expr);
            // check to see wether the val is const
            if (!var.m_expr->m_value.isConst()) 
                Error::complain(var,
                        "class variable '%s' is initialized with non const value", 
                        var.m_name.c_str());
            else if (!isTypeCompatible(type, var.m_expr->getType())) 
                Error::complain(var,
                        "class variable '%s' is initialized with no right type",
                        var.m_name.c_str());
            // else
            //    var.m_initializedVal = var.m_expr->m_value;
        }
    }
    // local variable
    else {
        if (var.m_expr) {
            walk(var.m_expr);
            // check to see wether the val is const
            if (var.isConst() && !var.m_expr->isConstant()) 
                Error::complain(var,
                        "variable '%s' is initialized with non const value", 
                        var.m_name.c_str());
            if (!isTypeCompatible(type, var.m_expr->getType())) 
                Error::complain(var,
                        "variable '%s' is initialized with wrong type",
                        var.m_name.c_str());
        }
        // for local object, define object in current scope 
        Object *object = new Object(var.m_name, type);
        object->setStorage(Object::LocalObject);
        defineObject(object);
    }
}

/// @brief Typebuilder handler for type specifier
void TypeBuilder::accept(ASTTypeDecl &typeDecl) 
{
    if (typeDecl.m_type == ASTTypeDecl::TMap) {
        Type *type = getType(typeDecl.m_name); 
        MapType *mapType = dynamic_cast<MapType *>(type);
        Type *keyType = getType(typeDecl.m_type1);
        Type *valType = getType(typeDecl.m_type2);
       
        if (!keyType)
            Error::complain(typeDecl, 
                    "type '%s' is not declared", typeDecl.m_name1.c_str());
        if (!valType)
            Error::complain(typeDecl, 
                    "type '%s' is not declared", typeDecl.m_name2.c_str());
        if (keyType && valType) 
            mapType->setItemType(keyType, valType);
    }
    else if (typeDecl.m_type  == ASTTypeDecl::TArray) {
        Type *type = getType(typeDecl.m_name); 
        SetType *setType = dynamic_cast<SetType *>(type);
        Type *valType = getType(typeDecl.m_type1);
        if (!valType)
            Error::complain(typeDecl, 
                    "type '%s' is not declared", typeDecl.m_name1.c_str());
        else
            setType->setValType(valType);
    }
    else if (!typeDecl.m_isQualified) {
        Type *type = getType(typeDecl.m_name);
        if (!type) 
            Error::complain(typeDecl, "type '%s' is not declared", 
                    typeDecl.m_name.c_str());        
    } 
}
/// @brief Handler for method type builder
void TypeBuilder::accept(ASTMethod &method) 
{
    bool isValid = true;
    Type *returnType = NULL;

    // check to see wether the return type of method is  declared
    if (!method.m_retTypeDecl) {
        Error::complain(method, "method '%s' return type is not declared",
                method.m_name.c_str());
        isValid = false;
    }
    else if ((returnType = getType(method.m_retTypeDecl)) == NULL) {
        Error::complain(method,
                "method '%s' return type '%s' is not declared", 
                method.m_name.c_str(), method.m_retTypeDecl->m_name.c_str());
        isValid = false;
    }
	// set the current scope
    enterScope(dynamic_cast<Scope *>(&method));
    
    // check to see wether there is the methodn VTBL
    ClassType *clsType = (ClassType *)getType(method.m_class);
    if (!clsType) {
        Error::complain(method,
                "class '%s'is not declared", method.m_class.c_str());
        isValid = false;
    }
        
    // get VTBL of the class
    if (clsType) {
        VirtualTable *vtbl = clsType->getVirtualTable();
        Assert(vtbl != NULL); 
        // check to see wether the VTBL have the method       
        MethodType *type = (MethodType *)vtbl->getSlot(method.m_name);
        if (type) {
            Error::complain(method,
                "class '%s' already has  method '%s'",
                clsType->getName().c_str(),
                method.m_name.c_str());
                isValid = false;
        }
    }
    if (isValid) {
        // define Methodye in current scope
        MethodType *methodType = new MethodType(method.m_name, method.isPublic());
//        methodType->setName(method.m_name);
//        defineType(methodType);
        
        // if the method is member of class
        ClassType *clsType = (ClassType *)getType(method.m_class);
        VirtualTable *vtbl = clsType->getVirtualTable();
        Assert(vtbl != NULL);

        if (vtbl)
            vtbl->addSlot(method.m_name, methodType);
        else
            Error::complain(method,
                    "class '%s' is not declared", method.m_class.c_str());
    }
    
    
    // the method instance will be used in other ast node 
    pushMethod(&method); 
    // check the method darameter list
    walk(method.m_paraList);
    // check the method lock
    walk(method.m_block);
    popMethod();
   
    popClass();
    // exit the method cope
    exitScope();
    
}


/// @brief TypeBuilder handler for MethodBlock
void TypeBuilder::accept(ASTMethodBlock &block) 
{
    walk(block.m_block);
}

/// @brief Handler for FormalParameterList type builder
void TypeBuilder::accept(ASTFormalParameterList &list) 
{
    int index = 1;
    ASTMethod *method = (ASTMethod *)list.m_method;
    Assert(method != NULL); 
   
    vector<ASTFormalParameter *>::iterator ite = list.m_parameters.begin();
    for (; ite != list.m_parameters.end(); ite++) {
        
        // check the parameter
        ASTFormalParameter *formalParameter =  *ite;
        formalParameter->m_method = method;
        formalParameter->m_index = index++;
        walk(formalParameter);
        
        // check wether there are same variable's name
        vector<ASTFormalParameter *>::iterator ip = list.m_parameters.begin();
        for (; ip != list.m_parameters.end(); ip++) {
            ASTFormalParameter *second =  *ip;
            if (ite != ip && formalParameter->m_name == second->m_name) 
                Error::complain(list,
                        "there are same parameter's name '%s'", 
                        second->m_name.c_str());
        }
    }
    // the first parameter must be pointer of class object 
    Object *object = new Object("self", getType(method->m_class));
    object->setStorage(Object::LocalObject);
    object->setOffset(0);
    defineObject(object);
}

/// @brief Handler for FormalParameter type builder
void TypeBuilder::accept(ASTFormalParameter &para) 
{
    // check the parameter's type
    if (!getType(para.m_type)) {
        Error::complain(para, "parameter's type is not declared"); 
    }
    ASTMethod *method = para.m_method;
    Assert(method != NULL); 
    
    // check the parameter's name
    if (getObject(para.m_name)) {
        Error::complain(para,
                "parameter '%s' is already declared in current scope", 
                para.m_name.c_str());
    }
    
    // define the passed parameter in current Object talbe
    Object *object = new Object(para.m_name, getType(para.m_type));
    // if the Methods called, all parameters are pushed by caller
    // so the address of each parameter must be knowned
    object->setStorage(Object::LocalObject);
    object->setOffset(para.m_index  * 4); 
    defineObject(object);
}

void TypeBuilder::accept(ASTArgumentList &arguments)
{}
void TypeBuilder::accept(ASTIterableObjectDecl &decl)
{}
void TypeBuilder::accept(ASTMapInitializer &mapInitializer)
{}
void TypeBuilder::accept(ASTMapPairItemInitializer &pairItemInitializer)
{}
void TypeBuilder::accpet(ASTArrayInitializer &arrayInitializer)
{}

/// handlder for block
void TypeBuilder::accept(ASTBlock &block)
{
    // iterate variables and statments in block
    vector<ASTStatement *>::iterator its = block.m_stmts.begin();
    for (; its != block.m_stmts.end(); its++)
        walk(*its);
}

void TypeBuilder::accept(ASTStatement &stmt)
{}
/// @brief TypeBuilder handler for Block Stmt
void TypeBuilder::accept(ASTBlockStmt &blockStmt) 
{
    // set the current scope
	enterScope(dynamic_cast<Scope *>(&blockStmt));
    
    vector<ASTStatement *>::iterator ite = blockStmt.m_stmts.begin();
    for (; ite != blockStmt.m_stmts.end(); ite++) 
        walk(*ite);
    
    exitScope();
}

/// @brief TypeBuilder handler for Variable Decl Stmt
void TypeBuilder::accept(ASTLocalVariableDeclarationStmt &stmt) 
{
    walk(stmt.m_var);
}

/// @brief TypeBuilder handler for if Stmt
void TypeBuilder::accept(ASTIfStmt &stmt) 
{
    enterScope(dynamic_cast<Scope *>(&stmt)); 
    // walk and check the condition expression type
    assert(stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->getType(), &boolType))
        Error::complain(stmt, "if condition type is wrong");
    
    // the expression type shoud be checked
    walk(stmt.m_ifBlockStmt);
    walk(stmt.m_elseBlockStmt);
    exitScope();
}

/// @brief TypeBuilder handler for while Stmt
void TypeBuilder::accept(ASTWhileStmt &stmt) 
{
    enterScope(dynamic_cast<Scope *>(&stmt)); 
    pushIterableStatement(&stmt);
    // walk and check the condition expression type
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->getType(), &boolType))
        Error::complain(stmt, "while condition type is wrong");
    
    walk(stmt.m_stmt);
    popIterableStatement();
    exitScope();
}

/// @brief TypeBuilder handler for do while Stmt
void TypeBuilder::accept(ASTDoStmt &stmt) 
{
    pushIterableStatement(&stmt);
    // walk and check the condition expression type
    assert (stmt.m_conditExpr != NULL);
    walk(stmt.m_conditExpr);
    
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_conditExpr->getType(), &boolType))
        Error::complain(stmt, "do condition type is wrong");
    
    walk(stmt.m_stmt);
    popIterableStatement();
}

/// @brief TypeBuilder handler for for Stmt
void TypeBuilder::accept(ASTForStmt &stmt)
{
    
    enterScope(dynamic_cast<Scope* >(&stmt)); 
    pushIterableStatement(&stmt);
    walk(stmt.m_initializer);
    walk(stmt.m_conditExpr);
    BoolType boolType;
    if (stmt.m_conditExpr->getType() && 
            !isTypeCompatible(stmt.m_conditExpr->getType(), &boolType))
        Error::complain(stmt, "for condtion expression type is wrong");
    walk(stmt.m_exprList);
    walk(stmt.m_stmt);
    popIterableStatement();
    exitScope();
}

/// @brief TypeBuilder handler for foreach Stmt
void TypeBuilder::accept(ASTForeachStmt &stmt) 
{
    enterScope(dynamic_cast<Scope* >(&stmt)); 
    
    pushIterableStatement(&stmt);
    walk(stmt.m_variable1);
    walk(stmt.m_variable2);
    walk(stmt.m_iterableObject);
    walk(stmt.m_stmt);
    
    Type *type = NULL;
    Object *object = NULL; 
    switch (stmt.m_objectSetType) {
        case ASTForeachStmt::Object:
            // get the Object and type
            object = getObject(stmt.m_objectSetName);
            if (!object)
                Error::complain(stmt, 
                        "object '%s' is not declared", 
                        stmt.m_objectSetName.c_str()); 
            else {
                type = object->getType();
                if (!type)
                    Error::complain(stmt, 
                            "object '%s'' type is not declared in current scope",
                            stmt.m_objectSetName.c_str());
            }
            // if the object set is map, check the var numbers
            if (type && isType(type, "map")){
                MapType *mapType = dynamic_cast<MapType*>(type);
                if(stmt.m_varNumbers != 2)
                    Error::complain(stmt, 
                            "var numbers mismatch in foreach Stmt");
                else {
                    Type *keyType = getType(stmt.m_typeDecl[0]);
                    Type *valType = getType(stmt.m_typeDecl[1]);
                    if (!isTypeCompatible(keyType, mapType->getKeyType()))
                        Error::complain(stmt, 
                                "key variable and map key's type is mismatch");
                    if (!isTypeCompatible(valType, mapType->getValType()))
                        Error::complain(stmt, 
                                "val variable and map val's type is mismatch");
                }
            }
            else if (type && isType(type, "set")) {
                if (stmt.m_varNumbers != 1)
                    Error::complain(stmt, 
                            "variable numbers is too much in foreach Stmt");
                else {
                    Type *valType = getType(stmt.m_typeDecl[0]);
                    SetType *setType = dynamic_cast<SetType*>(type);
                    if (!isTypeCompatible(setType->getValType(), valType))
                        Error::complain(stmt, 
                                "value type is mismatched with set type");
                }
            }
            else 
                Error::complain(stmt, 
                        "object '%s' is not set or map object", 
                        stmt.m_objectSetName.c_str());
            
            break;

        case ASTForeachStmt::SetObject: {
            ASTSetExpr *setExpr = NULL;
            // example foreach(int var in [0, 1, 2])
            // check the variable numbers
            if (stmt.m_varNumbers > 1)
                Error::complain(stmt, "too many variables in foreach Stmt");
            // check wether the variable's type is matched with set type
            SetType *setType = (SetType *) getType(stmt.m_typeDecl[0]);
            setExpr = dynamic_cast<ASTSetExpr* >(stmt.m_expr);
            if (!setExpr)
                Error::complain(stmt, 
                        "set expression in foreach Stmt is null");
            else {
                setType = dynamic_cast<SetType*>(setExpr->getType());
                if (!setType)
                    Error::complain(stmt, "set expression type is null");
                else if (!isTypeCompatible(type, setType->getValType()))
                    Error::complain(stmt, 
                            "tpe is mismatch between variable and set");
                else
                    Error::complain(stmt, "can not get the set type");
            }
            break;
        }
            
        case ASTForeachStmt::MapObject: {
            ASTMapExpr *mapExpr = dynamic_cast<ASTMapExpr*>(stmt.m_expr);
            
            if (stmt.m_varNumbers != 2)
                Error::complain(stmt, "less variables in foreach Stmt");
            else {
                Type *keyType = getType(stmt.m_typeDecl[0]);
                Type *valType = getType(stmt.m_typeDecl[1]);
                if (!mapExpr)
                    Error::complain(stmt, "map expression in foreach Stmt is null");
                else {
                    MapType *mapType = dynamic_cast<MapType *>(mapExpr->getType());
                    if (mapType && !isTypeCompatible(keyType, mapType->getKeyType()))
                        Error::complain(stmt, "key variable and map key's type is mismatch");
                    else if (mapType && !isTypeCompatible(valType, mapType->getValType()))
                        Error::complain(stmt, "val variable and map val's tpe is mismtach");
                    else
                        Error::complain(stmt, "can not get the map type");
                }
            }        
            break;
        }
        default:
            Error::complain(stmt, "unknow object set type in foreach Stmt");
            break;
    }
    // the expression type must be checked
    walk(stmt.m_stmt);
    popIterableStatement();
    exitScope(); 
}

/// @brief TypeBuilder handler for switch Stmt
void TypeBuilder::accept(ASTSwitchStmt &stmt) 
{
    enterScope(dynamic_cast<Scope *>(&stmt)); 
    pushBreakableStatement(&stmt);
    // check the condition type
    walk(stmt.m_conditExpr);
    IntType intType;
    if (!isTypeCompatible(stmt.m_conditExpr->getType(), &intType))
        Error::complain(stmt, "switch condition type is wrongly declared");
       
    // for each case, iterate
    for (int index = 0; index < (int)stmt.m_cases.size(); index++) {
        pair<vector<ASTExpr *>, ASTStatement* > *block = &stmt.m_cases[index];
        if (block) {
            vector<ASTExpr *> exprList = block->first;
            // iterate the expression list
            vector<ASTExpr *>::iterator ite = exprList.begin();
            for (; ite != exprList.end(); ite++) {
                ASTExpr *expr = *ite;
                walk(expr);
                if (!isTypeCompatible(expr->getType(), &intType))
                    Error::complain(stmt, "case type is wrongly declared");
            }
            // check the Stmt block
            walk(block->second);
        }
    }
    walk(stmt.m_defaultStmt);
    popBreakableStatement();
    exitScope(); 
}
/// @brief TypeBuilder handler for continue Stmt
void TypeBuilder::accept(ASTContinueStmt &stmt) 
{
    // in compile phase, the continue Stmt error should be checked
    if (!getCurrentIterableStatement())
        Error::complain(stmt, "continue statment is not rightly declared");
}

/// @brief TypeBuilder handler for break Stmt
void TypeBuilder::accept(ASTBreakStmt &stmt) 
{
    if (!getCurrentBreakableStatement())
        Error::complain(stmt, "break Stmt is not rightly declared");
    // in compile phase, the break Stmt error should be checked
}

/// @brief TypeBuilder handler for return Stmt
void TypeBuilder::accept(ASTReturnStmt &stmt) 
{
    if (!getCurrentMethod())
        Error::complain(stmt, "return Stmt is not declared in method");
    // the expression type shoud be checked
    walk(stmt.m_resultExpr);
    // the return type and the Methodtype must be compatible
}

/// @brief TypeBuilder handler for throw Stmt
void TypeBuilder::accept(ASTThrowStmt &stmt) 
{
    walk(stmt.m_resultExpr);
}

/// @brief TypeBuilder handler for assert Stmt
void TypeBuilder::accept(ASTAssertStmt &stmt) 
{
    walk (stmt.m_resultExpr);
    BoolType boolType;
    if (!isTypeCompatible(stmt.m_resultExpr->getType(), &boolType))
        Error::complain(stmt, "assert expression shoul be bool type");        
}

/// @brief TypeBuilder handler for try Stmt
void TypeBuilder::accept(ASTTryStmt &stmt) 
{
    walk(stmt.m_blockStmt);    
    vector<ASTCatchStmt *>::iterator ite;
    for (ite = stmt.m_catchStmts.begin(); ite != stmt.m_catchStmts.end(); ite++) 
        walk(*ite);
    
    walk(stmt.m_finallyStmt);
}

/// @brief TypeBuilder handler for catch Stmt
void TypeBuilder::accept(ASTCatchStmt &stmt) 
{
    if (!getObject(stmt.m_type)) 
                Error::complain(stmt, "type '%s' is not declared", stmt.m_type.c_str());
    if (getObject(stmt.m_id))
        Error::complain(stmt, "object '%s' has been defined", stmt.m_id.c_str());
    
    walk(stmt.m_block);
}

/// @brief TypeBuilder handler for finallycatch Stmt
void TypeBuilder::accept(ASTFinallyCatchStmt &stmt) 
{
    walk(stmt.m_block);
}

void TypeBuilder::accept(ASTExprStmt &stmt)
{
    walk(stmt.m_target);
    vector<pair<string, ASTExpr *> >::iterator ite;
    for (ite = stmt.m_elements.begin(); ite != stmt.m_elements.end(); ite++) {
        pair<string, ASTExpr *> &item = *ite;
        walk(item.second);
    }
}

/// @brief TypeBuilder handler for expression
void TypeBuilder::accept(ASTExpr &expr)
{
    
}
/// @brief TypeBuilder handler for expression list expression
void TypeBuilder::accept(ASTExprList &list) 
{
    vector<ASTExpr *>::iterator ite;
    for (ite = list.m_exprs.begin(); ite != list.m_exprs.end(); ite++)
        walk(*ite);
}

/// @brief TypeBuilder handler for binary op expression
void TypeBuilder::accept(ASTAssignmentExpr &expr) 
{
    walk(expr.m_left);
    walk(expr.m_right);
    if (!isTypeCompatible(expr.m_left->getType(), expr.m_right->getType()))
        Error::complain(expr, "type mismatch for binary expression");
}

/// @brief TypeBuilder handler for conditional expression
void TypeBuilder::accept(ASTConditionalExpr &expr) 
{
    
}

/// @brief TypeBuilder handler for logic or expression
void TypeBuilder::accept(ASTLogicOrExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    BoolType boolType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &boolType))
            Error::complain(expr, "expression type is not right, expected bool type");
    }
}

/// @brief TypeBuilder handler for logic and expression
void TypeBuilder::accept(ASTLogicAndExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    BoolType boolType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &boolType))
            Error::complain(expr, "expression type is not right, expected bool type");
    }
}

/// @brief TypeBuilder handler for bitwise or expression
void TypeBuilder::accept(ASTBitwiseOrExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// @brief TypeBuilder handler for bitwise xor expression
void TypeBuilder::accept(ASTBitwiseXorExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// @brief TypeBuilder handler for bitwise expression
void TypeBuilder::accept(ASTBitwiseAndExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// @brief TypeBuilder handler for equality expression
void TypeBuilder::accept(ASTEqualityExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// @brief TypeBuilder handler for relational expression
void TypeBuilder::accept(ASTRelationalExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    BoolType boolType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &boolType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// @brief TypeBuilder handler for shift expression
void TypeBuilder::accept(ASTShiftExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// @brief TypeBuilder handler for additive expression
void TypeBuilder::accept(ASTAdditiveExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}
    
/// @brief TypeBuilder handler for multiplicative expression    
void TypeBuilder::accept(ASTMultiplicativeExpr &expr) 
{
    walk(expr.m_target);
    
    vector<ASTExpr *>::iterator ite;
    IntType intType;
    for (ite = expr.m_elements.begin(); ite != expr.m_elements.end(); ite++) {
        ASTExpr *subExpr = *ite;
        walk(subExpr);
        if (subExpr->getType() &&
            !isTypeCompatible(subExpr->getType(), &intType))
            Error::complain(expr, "expression type is not right, expected int type");
    }
}

/// helper method
/// @brief compile an continue selector with an id, or methods call
/// @param curType: the current type, which is an unaryExpr
/// @param curID: current id
/// @param elements: an consecutive selectors
void TypeBuilder::handleSelectorExpr(
                ASTPrimaryExpr &primExpr,
                vector<ASTSelectorExpr *> &elements) 
{
    if (primExpr.m_type != ASTPrimaryExpr::T_IDENTIFIER ||
        primExpr.m_type != ASTPrimaryExpr::T_SELF ||
        primExpr.m_type != ASTPrimaryExpr::T_SUPER )
            return;
     
    // check wether the id is declared in current scope
    Type *type = getType(primExpr.m_text);
    if (!type) {
        Error::complain(primExpr, "identifier '%s' is not declared '%s'", 
                primExpr.m_text.c_str());
        return;
    }
    
    vector<ASTSelectorExpr *>::iterator ite;
    string curText = primExpr.m_text;
    for (ite = elements.begin(); ite != elements.end(); ite++) {        
        ASTSelectorExpr *selector = static_cast<ASTSelectorExpr *>(*ite);
        
        if (selector->m_type == ASTSelectorExpr::DOT_SELECTOR) {
            // check wether the member is in current scope
            if (type && type->getSlot(selector->m_id)) {
                Error::complain(primExpr, "identifier '%s' is not declared in '%s' scope", 
                                selector->m_id.c_str(),
                                type->getName().c_str());
                type = type->getSlot(selector->m_id);
                curText = selector->m_id;
            }
            else {
                Error::complain(primExpr, "current type is null");
            }
        }
        else if (selector->m_type == ASTSelectorExpr::ARRAY_SELECTOR) {
            if (curText == "self" || curText == "super")
                Error::complain(primExpr, "it is not right to apply array selector to self or super keyword");
            else {
                if (!type)
                    Error::complain(primExpr, "type '%s' is not declared", curText.c_str());
                else if (type && !type->isEnumerable())
                    Error::complain(primExpr, "'%s' is not enumerable object", curText.c_str());
                else
                    type = type->getSlot(0);
        
            }
        }
        else if (selector->m_type == ASTSelectorExpr::METHOD_SELECTOR) {
            if (curText == "self" || curText == "super")
                Error::complain(primExpr, "it is not right to take self as method");
            else {
                // check wether the method call is defined in current scope
                MethodType *methodType = (MethodType *) getType(curText);
                if (!methodType) {
                    Error::complain(primExpr, "method '%s' is not defined", curText.c_str());
                }
                ASTMethodCallExpr *methodCallExpr = selector->m_methodCallExpr;
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
void TypeBuilder::accept(ASTUnaryExpr &expr) 
{
    // if the primary expression is constant value, just return
    vector<ASTExpr *>::iterator ite;
    ASTPrimaryExpr *primExpr = dynamic_cast<ASTPrimaryExpr *>(expr.m_primary);
    if (!primExpr){
        Error::complain(expr, "unaryExpr has not a primaryExpr");
        return;
    }
    
    switch (primExpr->m_type) {
        case ASTPrimaryExpr::T_NUMBER:
        case ASTPrimaryExpr::T_NULL:
        case ASTPrimaryExpr::T_TRUE:
        case ASTPrimaryExpr::T_FALSE:
        case ASTPrimaryExpr::T_STRING:
        case ASTPrimaryExpr::T_HEX_NUMBER:
            if (expr.m_selectors.size() > 0)
                Error::complain(expr, "constant expression can not have a selector expression");
            return; 
        case ASTPrimaryExpr::T_IDENTIFIER: {
            // check to see wether the identifier is defined in current scope
            if (!getObject(primExpr->m_text)) {
                Error::complain(expr, "object '%s' is not defined in current scope",
                                primExpr->m_text.c_str());
            }
            handleSelectorExpr(*primExpr, expr.m_selectors);
            break;
        }
            
        case ASTPrimaryExpr::T_SELF: {
            handleSelectorExpr(*primExpr, expr.m_selectors);
            break;
        }
            
        case ASTPrimaryExpr::T_SUPER: {
            handleSelectorExpr(*primExpr, expr.m_selectors);
            break;
        }
  
        case ASTPrimaryExpr::T_COMPOUND:
            walk(expr.m_primary);
            break;
        default:
            break;
    }
}

void TypeBuilder::accept(ASTMethodCallExpr &expr) 
{
    vector<ASTExpr *>::iterator ite;
    for (ite = expr.m_arguments.begin(); ite != expr.m_arguments.end(); ite++)
        walk(*ite);    
}

/// @brief TypeBuilder handler for primary expression
void TypeBuilder::accept(ASTPrimaryExpr &expr) 
{
}

/// @brief TypeBuilder handler for selector expression
void TypeBuilder::accept(ASTSelectorExpr &expr) 
{
}

/// @brief TypeBilder handler for new expression
void TypeBuilder::accept(ASTNewExpr &expr) 
{
    // first, check wether the type is right
    if (!getType(expr.m_type))
        Error::complain(expr, "type '%s' is not declared", expr.m_type.c_str());
    walk(expr.m_arguments); 
}

void TypeBuilder::pushMethod(ASTMethod *method)
{
    m_methods.push(method);
}

void TypeBuilder::popMethod()
{
    if (!m_methods.empty())
        m_methods.pop();
}

ASTMethod* 
TypeBuilder::getCurrentMethod()
{
    if (!m_methods.empty())
        return m_methods.top();
    else
        return NULL;
}

void TypeBuilder::pushIterableStatement(ASTStatement *stmt)
{
    if (stmt)
        m_iterableStmts.push(stmt);
}

void TypeBuilder::popIterableStatement()
{
    if (!m_iterableStmts.empty())
        m_iterableStmts.pop();
}
ASTStatement* 
TypeBuilder::getCurrentIterableStatement()
{
    if (!m_iterableStmts.empty())
        return m_iterableStmts.top();
    else
        return NULL;
}
    
void TypeBuilder::pushBreakableStatement(ASTStatement *stmt)
{
    if (stmt)
        m_breakableStmts.push(stmt);
}

void TypeBuilder::popBreakableStatement()
{
    if (!m_breakableStmts.empty())
        m_breakableStmts.pop();
}

ASTStatement* 
TypeBuilder::getCurrentBreakableStatement()
{
    if (!m_breakableStmts.empty())
        return m_breakableStmts.top();
    else
        return NULL;
}
    
void TypeBuilder::pushClass(ASTClass *cls)
{
    if (cls)
        m_classes.push(cls);
}

void TypeBuilder::popClass()
{
    if (!m_classes.empty())
        m_classes.pop();
}

ASTClass* 
TypeBuilder::getCurrentClass()
{
    if (!m_classes.empty())
        return m_classes.top();
    else
        return NULL;
}

