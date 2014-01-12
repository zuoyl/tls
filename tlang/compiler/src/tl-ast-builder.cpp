//
//  tl-ast-builder.cpp
//  A toyable language compiler (like a simple c++)
//  @author:jenson.zuo@gmail.com 
// 

#include "tl-common.h"
#include "tl-ast-builder.h"
#include "tl-ast.h"
#include "tl-parser.h"
#include "tl-type.h"
#include "tl-ast-decl.h"
#include "tl-ast-stmt.h"
#include "tl-ast-expr.h"
#include "tl-exception.h"
#include "tl-ast-xml.h"
#include "tl-compile.h"
#include "tl-compile-option.h"
#include <algorithm>

using namespace tlang;

#define TTEXT(node) node->assic
#define TCHILD(node, n) node->childs[n]
#define TSIZE(node) node->count()
#define TLOCATION(node) node->location

// macro to check wether the node is specified name
// the macro is only used in ast handler method, so parameter node is default
#define AssertNode(name) Assert(TTEXT(node) == name) 

ASTBuilder::ASTBuilder(const string &path, const string& file) 
    :m_path(path), m_file(file)
{}

ASTBuilder::~ASTBuilder() 
{}

// method to convert a parse tree size_to an AST tree
AST* ASTBuilder::build(Node *parseTree) 
{
    if (!parseTree) 
        return NULL;
    // the root node of pareTree must be compile unit
    Location location;
    ASTCompileUnit *root = new ASTCompileUnit(location);
    // iterate parse tree to handle all declarations
    vector<Node*>::iterator ite = parseTree->childs.begin();
    for (; ite != parseTree->childs.end(); ite++) {
        AST *child = handleDeclarations(*ite);
        if (child) root->addAST(child);
    }
    // check wether output ast tree
    if (CompileOption::getInstance().isOutputAST()) {
        ASTXml xml(m_path, m_file);
        xml.build(root);
    }
    return root;
}

/// handle primitive type
AST* ASTBuilder::handlePrimitiveType(Node *node)
{
    AssertNode("primitiveType");

    // get sub type of the primitive type
    string typeName = node->childs[0]->assic;
    size_t type = ASTTypeDecl::TInvalid; 
    if (typeName == "bool")
        type = ASTTypeDecl::TBool;
    else if (typeName == "size_t")
        type = ASTTypeDecl::TInt;
    else if (typeName == "char")
        type = ASTTypeDecl::TChar; 
    else if (typeName == "byte")
        type = ASTTypeDecl::TByte;
    else if (typeName == "short")
        type = ASTTypeDecl::TShort;
    else if (typeName == "long")
        type = ASTTypeDecl::TLong;
    else if (typeName == "float")
        type = ASTTypeDecl::TFloat;
    else if (typeName == "double")
        type = ASTTypeDecl::TDouble;
    else if (typeName == "string")
        type = ASTTypeDecl::TString;
    else 
        Error::complain(node->location, "unknown type '%s'", typeName.c_str());
    
    return new ASTTypeDecl(type, typeName, node->location); 
}

/// handle class type
AST* ASTBuilder::handleClassTypeName(Node *node)
{ 
    AssertNode("classType");
    QualifiedName name;
    handleQualifiedName(node->childs[0], name);
    return new ASTTypeDecl(ASTTypeDecl::TClass, name, node->location);
}

/// handle map type
AST* ASTBuilder::handleMapType(Node *node)
{
    AssertNode("mapType");
    ASTTypeDecl *decl = new ASTTypeDecl(ASTTypeDecl::TMap, node->location); 
    if (node->childs[2]->childs[0]->assic == "primitiveType")
        decl->m_type1 = (ASTTypeDecl*)handlePrimitiveType(node->childs[2]->childs[0]);

    if (node->childs[4]->childs[0]->assic == "primitiveType")
        decl->m_type1 = (ASTTypeDecl*)handlePrimitiveType(node->childs[4]->childs[0]);
    return decl; 
}

/// handle type declaration such as size_t,
AST* ASTBuilder::handleType(Node *node)
{
    AssertNode("type");

    size_t  scalars = 0;
    ASTTypeDecl *decl = NULL; 
   
    // check wether the type is array 
    if (TSIZE(node) > 1) 
        scalars = (TSIZE(node) - 1) / 2;
        
    if (node->childs[0]->assic == "primitiveType") 
        decl = (ASTTypeDecl*)handlePrimitiveType(node->childs[0]); 
    else if (node->childs[0]->assic == "classType") 
        decl = (ASTTypeDecl*)handleClassTypeName(node->childs[0]); 
    else if (node->childs[0]->assic == "mapType") 
        decl = (ASTTypeDecl*)handleMapType(node->childs[0]);
    else
        Error::complain(node->location, 
                "unknown type '%s", node->childs[0]->assic.c_str());
    if (decl)
        decl->setScalars(scalars);
    
    return decl; 
}

/// handle the declaration
AST* ASTBuilder::handleDeclarations(Node *node)
{
    AssertNode("declaration");
    if (TTEXT(node) == "importDeclaration")
        return  handleImportDeclaration(node);
    else if (TTEXT(node) == "typeDeclaration")
        return handleTypeDeclaration(node);
    else
        Error::complain(TLOCATION(node), 
                "unknown declaration '%s'", TTEXT(node).c_str());
    return NULL; 
}

/// handle type declaration
AST* ASTBuilder::handleTypeDeclaration(Node *node)
{
    AssertNode("typeDeclaration");
    // process the class modifier at first
    ASTAnnotation *annotation = NULL;
    int attribute = ASTDeclaration::InvalidAttribute;
    // handle class modifier at first
    size_t index;
    for (index = 0; index < TSIZE(node) - 1; index++) 
        handleClassModifier(node->childs[index], attribute, &annotation); 

    ASTDeclaration *decl = NULL;
    if (TTEXT(TCHILD(node, index)) == "classDeclaration")
        decl = (ASTDeclaration*)handleClassDeclaration(node->childs[index]);
    else if (TTEXT(TCHILD(node, index)) == "enumDeclaration")
        decl = (ASTDeclaration*)handleEnumDeclaration(node->childs[index]);
    else if (TTEXT(TCHILD(node, index)) == "constantDeclaration")
        decl = (ASTDeclaration*)handleConstantDeclaration(node->childs[index]);
    else if (TTEXT(TCHILD(node, index)) == "packageDeclaration")
        return handlePackageDeclaration(node->childs[index]);
    else
        Error::complain(node->location, 
                "unknown declaraion '%s'", TTEXT(TCHILD(node, index)).c_str());
   if (decl) {
       decl->setAttribute(attribute);
       decl->setAnnotation(annotation);
   }
   return decl;
}

/// handle qualified name
void ASTBuilder::handleQualifiedName(Node *node, QualifiedName &names)
{
    AssertNode("qualifiedName");

    string name = TTEXT(TCHILD(node, 0));
    names.addElement(name);
    for (size_t index = 1; index < TSIZE(node); index++) {
        index++; // skip the ',' operator 
        name = TTEXT(TCHILD(node, index));
        names.addElement(name);
    }
}

/// handle qualifed name list
void ASTBuilder::handleQualifiedNameList(Node *node, vector<QualifiedName> &list)
{
    AssertNode("qualifiedNameList");
    
    for (size_t index = 0; index < TSIZE(node); index++) {
        QualifiedName name;
        handleQualifiedName(TCHILD(node, index), name);
        list.push_back(name);
        index++; // skip ','
    }
}

/// handle annotation declaration
AST* ASTBuilder::handleAnnotation(Node *node)
{
    AssertNode("annotation");

    ASTAnnotation *annotation = new ASTAnnotation(node->location);
    handleQualifiedName(node->childs[1]->childs[0], annotation->m_qualifiedName);
    if (TSIZE(node) == 4) {
        // there will be elmentPairs or  value
        if (TTEXT(TCHILD(node, 3)) == "elementValuePairs") 
           handleAnnotationElementValuePairs(TCHILD(node, 3), 
                   annotation->m_elementPairs); 
        // handle element value
        else
            handleAnnotationElementValue(TCHILD(node, 3),
                    annotation->m_elementValue);
    }

    return annotation; 
}

/// handle element value paris for annotation
void ASTBuilder::handleAnnotationElementValuePairs(
        Node *node,
        map<string, ASTAnnotation::ElementValue*> &elementValuePairs)
{
    AssertNode("elementValuePairs");
    
    for (size_t index = 0; index < TSIZE(node); index++) {
        // elementValuePair is node->childs[index];
        Node *elementValuePairNode = TCHILD(node, index);
        string identifier = elementValuePairNode->childs[0]->assic;
        ASTAnnotation::ElementValue *elementValue = new ASTAnnotation::ElementValue();
        handleAnnotationElementValue(elementValuePairNode->childs[2], *elementValue);
        elementValuePairs.insert(make_pair(identifier, elementValue)); 
        index++; //skip the ', operator
    }
}

/// handle element value
void ASTBuilder::handleAnnotationElementValue(
        Node *node,
        ASTAnnotation::ElementValue &elementValue)
{
    AssertNode("elementValue"); 
    
    if (TTEXT(TCHILD(node, 0))== "annotation")
        elementValue.annotation = 
            (ASTAnnotation*)handleAnnotation(node->childs[3]->childs[0]);
    else
        elementValue.expr = 
            (ASTExpr*)handleExpr(node->childs[3]->childs[0]);
}

/// handle package declaration
AST* ASTBuilder::handlePackageDeclaration(Node *node)
{
    AssertNode("packageDeclaration");
    
    ASTPackageDecl *decl = new ASTPackageDecl(node->location);
    size_t index = 0;
    for (; index < TSIZE(node) - 3; index++) {
        ASTAnnotation *annotation = (ASTAnnotation*)handleAnnotation(TCHILD(node, index));
        decl->addAnnotation(annotation); 
    }
    index++; // skip the 'package' keyword
    handleQualifiedName(TCHILD(node, index), decl->m_qualifiedName);
    return decl;
}

/// handle import declaration 
AST* ASTBuilder::handleImportDeclaration(Node *node) 
{
    AssertNode("importDeclaration");
    ASTImportDecl *decl = new ASTImportDecl(node->location);
    handleQualifiedName(TCHILD(node, 1), decl->m_qualifiedName); 
    if (TSIZE(node) == 5)
        decl->m_isImportAll = true;
    return decl;
}

/// handle class declaration 
AST* ASTBuilder::handleClassDeclaration(Node *node) 
{
    
    AssertNode("classDeclaration");

    size_t index = 1; // skip the 'class' keyword
    string clsName = node->childs[index++]->assic;

    // base class name 
    QualifiedName baseClsName;
    if (TTEXT(TCHILD(node, index)) == "extend") {
        index++;
        handleQualifiedName(TCHILD(node, index++), baseClsName);
        index++; 
    }
    // abstract class list 
    vector<QualifiedName> qualifiedNameList; 
    if (TTEXT(TCHILD(node, index)) == "implements") {
        index++;
        handleQualifiedNameList(node->childs[index++], qualifiedNameList);
        index++; 
    }
    
    ASTClass *cls = new ASTClass(clsName, baseClsName, qualifiedNameList, node->location);
   
    index++; // skip the '{' operator
    while (index < TSIZE(node) - 1) {
        ASTDeclaration* decl = 
            (ASTDeclaration*)handleClassBodyDeclaration(node->childs[index], clsName);
        if (decl)
            cls->addDeclaration(decl); 
        index++; 
    }
    return cls; 
}

/// handle class modifier
void ASTBuilder::handleClassModifier(
        Node *node,
        int &attribute, 
        ASTAnnotation **annotation)
{
    Assert(annotation != NULL);
    AssertNode("classModifier");

    if (TTEXT(TCHILD(node, 0)) == "annotation") {
        *annotation = (ASTAnnotation*)handleAnnotation(TCHILD(node, 0));
        attribute &= ASTDeclaration::AnnotationAttribute;
    }
    else if (TTEXT(TCHILD(node, 0)) == "public")
        attribute |= ASTDeclaration::PublicAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "private")
        attribute |= ASTDeclaration::PrivateAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "protected")
        attribute |= ASTDeclaration::ProtectedAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "static")
        attribute |= ASTDeclaration::StaticAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "abstract")
        attribute |= ASTDeclaration::AbstractAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "final")
        attribute |= ASTDeclaration::FinalAttribute; 
    else if (TTEXT(TCHILD(node,0)) == "const")
        attribute |= ASTDeclaration::ConstAttribute;
}

void ASTBuilder::handleModifier(
        Node *node,
        int &attribute,
        ASTAnnotation **annotation)
{
    AssertNode("modifier");
    
    if (node->childs[0]->assic == "classModifier")
        return handleClassModifier(node->childs[0], attribute, annotation);

    if (TTEXT(TCHILD(node, 0)) == "native")
        attribute |= ASTDeclaration::NativeAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "sychronized")
        attribute |= ASTDeclaration::SychronizedAttribute;

}

/// hanlde class body declaration 
AST* ASTBuilder::handleClassBodyDeclaration(Node *node, const string &cls) 
{
    AssertNode("classBodyDeclaration");

    if (TSIZE(node) == 1)
        return NULL;
    // process the class modifier at first
    ASTAnnotation *annotation = NULL;
    int attribute = ASTDeclaration::InvalidAttribute;
    size_t i = 0; 
    for (; i < TSIZE(node) - 1; i++) 
        handleModifier(TCHILD(node, i), attribute, &annotation);
    
        
    // handle the member declaration
    ASTDeclaration *decl = 
        (ASTDeclaration*)handleClassMemberDeclaration(TCHILD(node, i), cls);
    if (decl) {
        decl->setAttribute(attribute);
        decl->setAnnotation(annotation);
    }
    return decl;
}

/// handle class member declaration
AST* ASTBuilder::handleClassMemberDeclaration(Node *node, const string &clsName)
{
   AssertNode("memberDeclaration");
   if (TTEXT(TCHILD(node, 0)) == "methodDeclaration")
       return handleMethodDeclaration(TCHILD(node, 0), clsName);
   else if (TTEXT(TCHILD(node, 0)) == "fieldDeclaration")
        return handleFieldDeclaration(TCHILD(node, 0), clsName);
 //  else if (TTEXT(TCHILD(node, 0)) == "consructorDeclaration")
 //      return handleConstructorDeclaration(TCHILD(node, 0), clsName);
   else
       Error::complain(node->location, 
               "unkown member declaraion '%s'",
               TTEXT(TCHILD(node, 0)).c_str());

   return NULL;
}

/// handle enum declaration
AST* ASTBuilder::handleEnumDeclaration(Node *node)
{
    return NULL;
}

/// handle constant declaration
AST* ASTBuilder::handleConstantDeclaration(Node *node)
{
    return NULL;
}

/// handle class method
AST* ASTBuilder::handleMethodDeclaration(Node *node, const string &clsName) 
{
    AssertNode("methodDeclaration"); 
    size_t index = 0; 
    // return type and method name
    ASTTypeDecl *retType = NULL;
    if (node->childs[index]->assic != "void")
        retType = (ASTTypeDecl*)handleType(node->childs[index]);
    else
        retType = new ASTTypeDecl(ASTTypeDecl::TVoid, 
                        node->childs[index]->assic,
                        node->location);
    
    index++; 
    string methodName = node->childs[index++]->assic;
    // method parameter list
    ASTFormalParameterList *formalParameterList = 
        (ASTFormalParameterList*)handleFormalParameters(node->childs[index]);    
    index++; 
    // check to see wethe the exception is thrown
    vector<QualifiedName> qualifiedNameList;
    if (node->childs[index]->assic == "throw") {
        // handle qualified name list
        index++; 
        handleQualifiedNameList(node->childs[index++], qualifiedNameList);
    }
    // check the method body 
    ASTMethodBlock *methodBlock = NULL; 
    if (node->childs[index]->assic ==  "methodBody")
        methodBlock = (ASTMethodBlock*)handleMethodBlock(node->childs[index]);

    // create method instance 
    ASTMethod *method = new ASTMethod(retType, 
                                      methodName,
                                      clsName,
                                      formalParameterList, 
                                      node->location);
    // tell method wether it will throw exception 
    method->setWetherThrowException(true, qualifiedNameList); 
    method->m_block = methodBlock; 
    
    // update parameterlist's information
    if (formalParameterList)
        formalParameterList->m_method = method;
    return method;
}

/// handle variable declaration
AST* ASTBuilder::handleFieldDeclaration(Node *node, const string &clsName) 
{
    AssertNode("fieldDeclaration");

    ASTTypeDecl *type = (ASTTypeDecl*)handleType(node->childs[0]);
    return handleVariableDeclarators(node->childs[1], type); 
}

// handle variableDeclarators
AST* ASTBuilder::handleVariableDeclarators(Node *node, ASTTypeDecl *type)
{
    AssertNode("variableDeclarators");
    
    string varName;
    int scalars = 0;
    handleVariableDeclaratorId(node->childs[0], varName, scalars); 
    ASTVariable *var =  new ASTVariable(type, varName, node->location);  
    if (TSIZE(node) > 2)
        var->m_expr = (ASTExpr*)handleVariableInitializer(node->childs[2]);
    return var;
}

/// handle to generate an initializer
AST* ASTBuilder::handleVariableInitializer(Node *node)
{
    AssertNode("variableInitializer");
    if (node->childs[0]->assic == "expression")
        return handleExpr(node->childs[0]);
    else if (node->childs[0]->assic == "arrayInitializer")
        return handleArrayInitializer(node->childs[0]);
    else if (node->childs[0]->assic == "mapInitializer")
        return handleMapInitializer(node->childs[0]);
    else
        Error::complain(node->location,
                "unknow initializer", node->childs[0]->assic.c_str());
    return NULL;
}

/// handle array initializer
AST* ASTBuilder::handleArrayInitializer(Node *node)
{
    AssertNode("arrayInitializer");

    ASTArrayInitializer *arrayInitializer = new ASTArrayInitializer(node->location); 
    for (size_t index = 1; index < TSIZE(node) - 1; index++) {
        AST *initializer = handleVariableInitializer(node->childs[index]);
        arrayInitializer->addInitializer(initializer);
        if (node->childs[index]->assic == ",")
            continue;
    }
    return arrayInitializer; 
}

/// handle map initializer
AST* ASTBuilder::handleMapInitializer(Node *node)
{
    AssertNode("mapInitializer");
    
    ASTMapInitializer *mapInitializer = new ASTMapInitializer(node->location);
    for (size_t index = 1; index < TSIZE(node) - 1; index++) {
        ASTMapPairItemInitializer *initializer = (ASTMapPairItemInitializer*)
                         handleMapPairItemInitializer(node->childs[index]);
        mapInitializer->addInitializer(initializer);
        if (node->childs[index]->assic == ",")
            continue;
    }
    return mapInitializer; 
}
/// handle map item pair initializer
AST* ASTBuilder::handleMapPairItemInitializer(Node *node) 
{
    AssertNode("mapItemPairInitializer");
    
    ASTMapPairItemInitializer *mapPairItemInitializer = 
        new ASTMapPairItemInitializer(node->location);
    mapPairItemInitializer->m_key =
        handleVariableInitializer(node->childs[0]);
    mapPairItemInitializer->m_val =
        handleVariableInitializer(node->childs[2]);
    return mapPairItemInitializer;
}

/// handle formal parameters
AST* ASTBuilder::handleFormalParameters(Node *node)
{
    AssertNode("formalParameters");
    
    if (TSIZE(node) == 3)
        return handleFormalParameterList(node->childs[1]);
    return NULL;
}

/// handle formal parameter list
AST* ASTBuilder::handleFormalParameterList(Node *node) 
{
    AssertNode("formalParameterList");

    ASTFormalParameterList *formalParameterList =
        new ASTFormalParameterList(node->location);
    
    // handle normal parameter
    for (size_t index = 0; index < TSIZE(node); index++) {
        ASTFormalParameter *parameter = 
            (ASTFormalParameter*) handleFormalParameter(node->childs[index]);
        if (parameter)
            formalParameterList->addParameter(parameter);
        index++; // skop the token ','
    }
        
    return formalParameterList;
}

/// handle variable modifier
void ASTBuilder::handleVariableModifier(
                    Node *node,
                    int &attribute,
                    ASTAnnotation **annotation)
{
    AssertNode("varaibleModifier");
    Assert(annotation != NULL);

    if (node->childs[0]->assic == "final")
        attribute &= ASTDeclaration::FinalAttribute;
    else if (node->childs[0]->assic == "const")
        attribute &= ASTDeclaration::ConstAttribute;
    else if (node->childs[0]->assic == "annotation")
        *annotation = (ASTAnnotation*)handleAnnotation(node->childs[0]);
    else
        Error::complain(node->location,
                "unknown variable attribute '%s'",
                node->childs[0]->assic.c_str());
}

/// handle variable declarator id
void ASTBuilder::handleVariableDeclaratorId(
                    Node *node,
                    string &variableName,
                    int &scalars)
{
    AssertNode("variableDeclaratorId");

    variableName = node->childs[0]->assic;
    if (TSIZE(node) > 1)
        scalars = (TSIZE(node) - 1) / 2;
}

/// handle formal parameter
AST* ASTBuilder::handleFormalParameter(Node *node) 
{
    AssertNode("formalParameter");
    int attribute = ASTDeclaration::InvalidAttribute; 
    size_t index = 0;
    ASTAnnotation *annotation = NULL; 
    
    while (index < (TSIZE(node) - 2)){
        if (node->childs[index]->assic != "variableModifier")
            break;
        handleVariableModifier(node->childs[index], attribute, &annotation);
    }
    // get type name and id
    ASTTypeDecl *variableType = 
        (ASTTypeDecl*)handleType(node->childs[index++]);
    string variableName;
    int scalars = 0;
    handleVariableDeclaratorId(node->childs[index], variableName, scalars);
    ASTFormalParameter *formalParameter = 
        new  ASTFormalParameter(variableType, variableName, node->location);
    formalParameter->setAttribute(attribute);
    formalParameter->setScalars(scalars);
    return formalParameter;
}

/// handle method block
AST* ASTBuilder::handleMethodBlock(Node *node) 
{
    AssertNode("methodBody");
    
    ASTBlock *block = (ASTBlock*)handleBlock(node->childs[0]);
    ASTMethodBlock *methodBlock = new ASTMethodBlock(block, node->location);
    return methodBlock;
}

//
// Statements
//

/// handle block
AST* ASTBuilder::handleBlock(Node *node)
{
    AssertNode("block");
    
    ASTBlock *block = new ASTBlock(node->location);
    for (size_t index = 1; index < TSIZE(node) - 1; index++) {
        ASTStatement *stmt = 
            (ASTStatement*)handleBlockStatement(node->childs[index], block);
        block->addStatement(stmt);
    }
    return block;
}

/// handle block statement
AST* ASTBuilder::handleBlockStatement(Node *node, ASTBlock *block)
{
    AssertNode("blockStatement");
   
    if (node->childs[0]->assic == "localVariableDeclarationStatement")
        return handleLocalVariableDeclarationStatement(node->childs[0], block);
    else if (node->childs[0]->assic == "statement")
        return handleStatement(node->childs[0], block);
  //  else if (node->childs[0]->assic == "typeDeclaration")
  //      return handleTypeDeclaration(node->childs[0]);
    else
        Error::complain(node->location,
                "unknow statement '%s'",
                node->childs[0]->assic.c_str());
    return NULL;
}

/// handle statement
AST* ASTBuilder::handleStatement(Node *node, ASTBlock *block) 
{
    AssertNode("statement");

    string type = node->childs[0]->assic;
    
    if (type == "block")
        return handleBlock(node->childs[0]);
    else if (type == "ifStatement")
        return handleIfStatement(node->childs[0]);
    else if (type == "forStatement")
        return handleForStatement(node->childs[0]);
    else if (type == "foreachStatement")
        return handleForeachStatement(node->childs[0]);
    else if (type == "doStatement")
        return handleDoStatement(node->childs[0]);
    else if (type == "whileStatement")
        return handleWhileStatement(node->childs[0]);
    else if (type == "returnStatement")
        return handleReturnStatement(node->childs[0]);
    else if (type == "tryStatement")
        return handleTryStatement(node->childs[0]);
    else if (type == "throwStatement")
        return handleTryStatement(node->childs[0]);
    else if (type == "breakStatement")
        return handleBreakStatement(node->childs[0]);
    else if (type == "continueStatement")
        return handleContinueStatement(node->childs[0]);
    else if (type == "assertStatement")
        return handleAssertStatement(node->childs[0]);
    else if (type == "expressionStatement")
        return handleExprStatement(node->childs[0]);
    else if (type == "methodCallStatement")
        return handleMethodCallStatement(node->childs[0]);
    else 
        Error::complain(node->location,
                "unknown statement '%s'", type.c_str());
    return NULL;
}

/// handle local variable declaration statement
AST* ASTBuilder::handleLocalVariableDeclarationStatement(
                    Node *node,
                    ASTBlock *block) 
{
    AssertNode("localVariableDeclarationStatement");

    ASTVariable *variable = 
        (ASTVariable*)handleLocalVariableDeclaration(node->childs[0]);
    ASTLocalVariableDeclarationStmt* stmt = 
        new ASTLocalVariableDeclarationStmt(variable, variable->m_expr, node->location);

    return stmt;
}

/// handle local variable declaration
AST* ASTBuilder::handleLocalVariableDeclaration(Node *node)
{
    AssertNode("localVariableDeclaration");

    int attribute = ASTDeclaration::InvalidAttribute; 
    size_t index = 0;
    ASTAnnotation *annotation = NULL; 
    
    while (index < (TSIZE(node) - 2)){
        if (node->childs[index]->assic != "variableModifier")
            break;
        handleVariableModifier(node->childs[index], attribute, &annotation);
    }
    ASTTypeDecl *variableType = 
        (ASTTypeDecl*)handleType(node->childs[index++]);
    return handleVariableDeclarators(node->childs[index], variableType);
}

/// handle if statement
AST* ASTBuilder::handleIfStatement(Node *node) 
{
    AssertNode("ifStatement");

    ASTExpr *conditExpr = (ASTExpr*)handleCompareExpr(node->childs[2]);
    ASTStatement *stmt1 = NULL;
    ASTStatement *stmt2 = NULL;
    // if statement 
    stmt1 = (ASTStatement*)handleStatement(node->childs[4], NULL);
    // else part 
    if (node->count() == 7) 
        stmt2 = (ASTStatement*)handleStatement(node->childs[6], NULL);
    return new ASTIfStmt(conditExpr, stmt1, stmt2, node->location);
}

/// handle for initializer
AST* ASTBuilder::handleForInitializer(Node *node)
{
    AssertNode("forInitializer");
    
    if (node->childs[0]->assic == "localVariableDeclaration")
        return handleLocalVariableDeclaration(node->childs[0]);
    else if (node->childs[0]->assic == "expressionList")
        return handleExprList(node->childs[0]);
    else
        Error::complain(node->location, "unknown for statement initializer");
    return NULL;

}

/// handle for statement
AST* ASTBuilder::handleForStatement(Node *node) 
{
    AssertNode("forStatement");

    size_t index = 2; // skip the 'for' and '(' keyword
    AST *initializer = NULL;
    ASTExpr *loopCondition = NULL;
    ASTExprList *exprList = NULL;
    
    if (node->childs[index]->assic == "forInitializer")
        initializer = handleForInitializer(node->childs[index++]);
    index++;

    if (node->childs[index]->assic == "expression")
        loopCondition  = (ASTExpr*)handleExpr(node->childs[index++]);
    index++;

    if (node->childs[index]->assic == "expressionList")
        exprList = (ASTExprList*)handleExprList(node->childs[index++]);
    index++;

    ASTStatement *stmt = (ASTStatement*)handleStatement(node->childs[index], NULL);
    return new ASTForStmt(initializer, 
                    loopCondition, exprList, stmt, node->location);
}

/// handle foreach statement
AST* ASTBuilder::handleForeachStatement(Node *node) 
{
    AssertNode("foreachStatement"); 
    
    
    ASTForeachStmt *foreachStmt = new ASTForeachStmt(node->location); 

    // check the foreachVarItem
    foreachStmt->m_variable1 = (ASTVariable*)handleForeachVariable(node->childs[2]);
    size_t index = 3;

    if (node->childs[index]->assic == ",") {
        foreachStmt->m_variable2 = 
            (ASTVariable*)handleForeachVariable(node->childs[index]); 
        index++;
    }
    index++; // skip the 'in' keyword
    foreachStmt->m_iterableObject = 
        (ASTIterableObjectDecl*)handleIterableObject(node->childs[index++]); 
    index++; // skip ')' 
    foreachStmt->m_stmt = (ASTStatement*)handleStatement(node->childs[index], NULL);  
    return foreachStmt;
}

/// handle variable declaration in foreach statement
AST* ASTBuilder::handleForeachVariable(Node *node)
{
    AssertNode("foreachVariable");
    // the foreach variable may have type 
    if (TSIZE(node) == 2) {
        // the foreach variable have type declaration
        ASTTypeDecl *variableType = (ASTTypeDecl*)handleType(node->childs[0]);
        string variableName = node->childs[1]->assic;
        return new ASTVariable(variableType, variableName, node->location);
    }
    else {
        string variableName = node->childs[0]->assic;
        return new ASTVariable(NULL, variableName, node->location);
    }
}

/// handle iteralbe object declaration in foreach statement
AST* ASTBuilder::handleIterableObject(Node *node)
{
    AssertNode("iterableObject");

    ASTIterableObjectDecl *iterableObject = NULL;

    if (node->childs[0]->assic == "IDENTIFIER") {
        iterableObject =  new ASTIterableObjectDecl(node->location);
        iterableObject->m_identifier = node->childs[0]->assic;
    }
    else if (node->childs[0]->assic == "mapInitializer")
        return handleMapInitializer(node->childs[0]);
    else if (node->childs[0]->assic == "arrayInitializer")
        return handleArrayInitializer(node->childs[0]);
    else
        Error::complain(node->location, "unknown iterable object"); 
    
    return iterableObject;
}

/// handle while statement
AST* ASTBuilder::handleWhileStatement(Node *node) 
{
    ASTExpr *conditExpr = (ASTExpr*)handleCompareExpr(node->childs[2]);
    ASTStatement *stmt = (ASTStatement*)handleStatement(node->childs[4], NULL);
    return new ASTWhileStmt(conditExpr, stmt, node->location);
}

/// handle do while statement
AST* ASTBuilder::handleDoStatement(Node *node) 
{
    ASTExpr *conditExpr = (ASTExpr*)handleCompareExpr(node->childs[2]);
    ASTStatement *stmt = (ASTStatement*)handleStatement(node->childs[4], NULL);
    return new ASTDoStmt(conditExpr, stmt, node->location);
}

/// handler for switch statement
AST* ASTBuilder::handleSwitchStatement(Node *node) 
{
    ASTExpr *resultExpr = (ASTExpr*)handleExpr(node->childs[2]);
    ASTSwitchStmt *switchStmt  = new ASTSwitchStmt(resultExpr, node->location);
    
    for (size_t index = 5; index < node->count(); index++) {
        if (node->childs[index]->assic == "switchCase") {
            Node *subnode = node->childs[index];
            vector<ASTExpr*> *exprList = new vector<ASTExpr*>();
            for (size_t subIndex = 0; subIndex < subnode->count() - 1; subIndex += 3) {
                ASTExpr *caseExpr = 
                        (ASTExpr*)handleExpr(subnode->childs[subIndex + 1]);
                exprList->push_back(caseExpr);
            }
            ASTStatement *stmt = 
                (ASTStatement*)handleSwitchStatement(subnode->childs[subnode->count()-1]);
            switchStmt->addCaseStatement(exprList, stmt);
            
        }
        else if (node->childs[index]->assic == "defaultCase") {
            Node *subnode = node->childs[index];
            ASTStatement *stmt = (ASTStatement*)handleSwitchStatement(subnode->childs[2]);
            switchStmt->addDefaultStatement(stmt);
        }
        else {
            delete switchStmt;
            switchStmt = NULL;
            throw Exception::InvalidStatement(node->assic);
            break;
        }
    }
    return switchStmt;
}

/// hander for return statement
AST* ASTBuilder::handleReturnStatement(Node *node) 
{
    ASTExpr *expr = NULL;
    if (node->count() == 3)
        expr = (ASTExpr*)handleExpr(node->childs[1]);
    return new ASTReturnStmt(expr, node->location);
}

/// hander for assert statement
AST* ASTBuilder::handleAssertStatement(Node *node) 
{
    ASTExpr *expr = (ASTExpr*)handleExpr(node->childs[1]);
    return new ASTAssertStmt(expr, node->location);
}

/// hander for break statement
AST* ASTBuilder::handleBreakStatement(Node *node) 
{
    return new ASTBreakStmt(node->location);
}

/// hander for continue statement
AST* ASTBuilder::handleContinueStatement(Node *node) 
{
    return new ASTContinueStmt(node->location);
}

/// hander for throw statement
AST* ASTBuilder::handleThrowStatement(Node *node) 
{
    ASTExpr *expr = NULL;
    if (node->count() == 3)
        expr = (ASTExpr*)handleExpr(node->childs[1]);
    return new ASTThrowStmt(expr, node->location);
}

/// hander for try statement
AST* ASTBuilder::handleTryStatement(Node *node) 
{
    ASTBlockStmt *blockStmt = 
        (ASTBlockStmt*)handleBlockStatement(node->childs[1], NULL);
    ASTTryStmt *tryStmt = new ASTTryStmt(blockStmt, node->location);
    
    for (size_t index = 2; index < node->count(); index ++) {
        if (node->childs[index]->assic == "catchPart") {
            ASTCatchStmt *catchStmt = 
                        (ASTCatchStmt*)handleCatchStatement(node->childs[index]);
            tryStmt->addCatchPart(catchStmt);
        }
        else if (node->childs[index]->assic == "finallyPart") {
            ASTFinallyCatchStmt *finallyStmt = (ASTFinallyCatchStmt*)
                        handleFinallyCatchStatement(node->childs[index]);
            tryStmt->setFinallyCatchPart(finallyStmt);
        }
        else {
            throw Exception::InvalidStatement(node->assic);
            delete tryStmt;
            tryStmt = NULL;
            break;
        }
    }
    
    return tryStmt;
}

/// hander for catch statement
AST* ASTBuilder::handleCatchStatement(Node *node) 
{
    string type = node->childs[1]->childs[0]->assic;
    string id = node->childs[2]->childs[0]->assic;
    ASTBlockStmt *blockStmt = 
                (ASTBlockStmt*)handleBlockStatement(node->childs[5], NULL);
    
    return new ASTCatchStmt(type, id, blockStmt, node->location);
}

/// hander for finally catch statement
AST* ASTBuilder::handleFinallyCatchStatement(Node *node) 
{
    ASTBlockStmt *blockStmt = 
        (ASTBlockStmt*)handleBlockStatement(node->childs[1], NULL);
    return new ASTFinallyCatchStmt(blockStmt, node->location);
}

/// handler for expression statement
AST* ASTBuilder::handleExprStatement(Node *node) 
{
    ASTExprStmt *stmt = new ASTExprStmt(node->location);
    stmt->m_target = (ASTExpr*)handleExpr(node->childs[0]);
    return stmt;
}

/// hander for funcation statement
AST* ASTBuilder::handleMethodCallStatement(Node *node) 
{
    return NULL;
}

/// handler for expression list
AST* ASTBuilder::handleExprList(Node *node) 
{
    ASTExprList *exprList = new ASTExprList(node->location);
    ASTExpr *expr = NULL;
    
    for (size_t index = 0; index < node->count(); index++) {
        expr = (ASTExpr*)handleExpr(node->childs[index]);
        exprList->appendExpr(expr);
        index++;
    }
    return exprList;
}

/// handler for expression

AST* ASTBuilder::handleCompareExpr(Node *node)
{
    if (node->count() == 1)
        return handleExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleExpr(node->childs[0]);
    ASTComparisonExpr *comparExpr = new ASTComparisonExpr(leftExpr, node->location);
    
    for (size_t index = 2; index < node->count(); index ++ ) {
        string op = node->childs[index++]->assic; 
        ASTExpr *target = (ASTExpr*)handleExpr(node->childs[index]);
        comparExpr->appendElement(op, target);
    }
    return comparExpr;
}

AST* ASTBuilder::handleExpr(Node *node) 
{
    AssertNode("expression");

    if (node->childs[0]->assic == "assignmentExpr")
        return handleAssignmentExpr(node->childs[0]);
    else if (node->childs[0]->assic == "newExpr")
        return handleNewExpr(node->childs[0]);
    else if (node->childs[0]->assic == "conditionalExpr")
        return handleConditionalExpr(node->childs[0]);
    else
        return NULL;
}

/// handler for assignalbe expression
AST* ASTBuilder::handleAssignmentExpr(Node *node) 
{
    if (node->count() == 1)
        return handleUnaryExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleUnaryExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    ASTExpr *rightExpr = (ASTExpr*)handleExpr(node->childs[2]);
    return new ASTAssignmentExpr(op, leftExpr, rightExpr, node->location);
}

/// handler for conditional expression
AST* ASTBuilder::handleConditionalExpr(Node *node) 
{
    AssertNode("conditionalExpr");
    return handleLogicOrExpr(node);
}

/// handler for logic or expression
AST* ASTBuilder::handleLogicOrExpr(Node *node) 
{
    AssertNode("logicOrExpr");

    if (node->count() == 1)
        return handleLogicAndExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleLogicAndExpr(node->childs[0]);
    ASTLogicOrExpr *logicOrExpre = new ASTLogicOrExpr(leftExpr, node->location);
    
    for (size_t index = 2; index < node->count(); index += 2) {
        ASTExpr *target = (ASTExpr*)handleLogicAndExpr(node->childs[index]);
        logicOrExpre->appendElement(target);
    }
    return logicOrExpre;
}

/// handler for logic and expression
AST* ASTBuilder::handleLogicAndExpr(Node *node) 
{
    AssertNode("logicAndExpr");

    if (node->count() == 1)
        return handleBitwiseOrExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleBitwiseOrExpr(node->childs[0]);
    ASTLogicAndExpr *logicAndExpr = new ASTLogicAndExpr(leftExpr, node->location);
    
    for (size_t index = 2; index < node->count(); index += 2) {
        ASTExpr *target = (ASTExpr*)handleBitwiseAndExpr(node->childs[index]);
        logicAndExpr->appendElement(target);
    }
    return logicAndExpr;
}

/// handler  for bitwise or expression
AST* ASTBuilder::handleBitwiseOrExpr(Node *node) 
{
    AssertNode("bitwiseOrExpr");
    if (node->count() == 1)
        return handleBitwiseXorExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleBitwiseXorExpr(node->childs[0]);
    ASTBitwiseOrExpr *expr = new ASTBitwiseOrExpr(leftExpr, node->location);
    
    for (size_t index = 2; index < node->count(); index += 2) {
        ASTExpr *target = (ASTExpr*)handleBitwiseXorExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}


/// handler for bitwise xor expression
AST* ASTBuilder::handleBitwiseXorExpr(Node *node) 
{
    AssertNode("bitwiseXorExpr");

    if (node->count() == 1)
        return handleBitwiseAndExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleBitwiseAndExpr(node->childs[0]);
    ASTBitwiseAndExpr *expr = new ASTBitwiseAndExpr(leftExpr, node->location);
    
    for (size_t index = 2; index < node->count(); index += 2) {
        ASTExpr *target = (ASTExpr*)handleBitwiseAndExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// handler for bitwise and expression
AST* ASTBuilder::handleBitwiseAndExpr(Node *node) 
{
    AssertNode("bitwiseAndExpr");

    if (node->count() == 1)
        return handleEqualityExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleEqualityExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    ASTBitwiseAndExpr *expr = new ASTBitwiseAndExpr(leftExpr, node->location);
    
    for (size_t index = 2; index < node->count(); index += 2) {
        ASTExpr *target = (ASTExpr*)handleEqualityExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// handler for equality expression
AST* ASTBuilder::handleEqualityExpr(Node *node) 
{
    AssertNode("equalityExpr");

    if (node->count() == 1)
        return handleRelationalExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleRelationalExpr(node->childs[0]);
    ASTEqualityExpr *expr = new ASTEqualityExpr(leftExpr, node->location);
    
    for (size_t index = 1; index < node->count(); index++) {
        
        size_t op = -1;
        if (node->childs[index]->childs[0]->assic == "==")
            op = ASTEqualityExpr::OP_EQ;
        else if (node->childs[index]->childs[0]->assic == "!=")
            op = ASTEqualityExpr::OP_NEQ;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        ASTExpr *target = (ASTExpr*)handleRelationalExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for relational expression
AST* ASTBuilder::handleRelationalExpr(Node *node) 
{
    AssertNode("relationalExpr");

    if (node->count() == 1)
        return handleShiftExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleShiftExpr(node->childs[0]);
    ASTRelationalExpr *expr = new ASTRelationalExpr(leftExpr, node->location);
    
    for (size_t index = 1; index < node->count(); index++) {
        size_t op = -1;
        if (node->childs[index]->childs[0]->assic == ">")
            op = ASTRelationalExpr::OP_GT;
        else if (node->childs[index]->childs[0]->assic == "<")
            op = ASTRelationalExpr::OP_LT;
        else if (node->childs[index]->childs[0]->assic == ">=")
            op = ASTRelationalExpr::OP_GTEQ;
        else if (node->childs[index]->childs[0]->assic == "<=")
            op = ASTRelationalExpr::OP_LTEQ;
        else 
            throw Exception::InvalidStatement(node->assic);        
        index++; 
        ASTExpr *target = (ASTExpr*)handleShiftExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for shift expression
AST* ASTBuilder::handleShiftExpr(Node *node) 
{
    AssertNode("shiftExpr"); 
    
    if (node->count() == 1)
        return handleAdditiveExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleAdditiveExpr(node->childs[0]);
    ASTShiftExpr *expr = new ASTShiftExpr(leftExpr, node->location);
    
    for (size_t index = 1; index < node->count(); index++) {
        size_t op = -1;
        if (node->childs[index]->childs[0]->assic == ">>")
            op = ASTShiftExpr::OP_RSHIFT;
        else if (node->childs[index]->childs[0]->assic == "<<")
            op = ASTShiftExpr::OP_LSHIFT;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        ASTExpr *target = (ASTExpr*)handleAdditiveExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for additive expression
AST* ASTBuilder::handleAdditiveExpr(Node *node) 
{
    AssertNode("additiveExpr");

    if (node->count() == 1)
        return handleMultiplicativeExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleMultiplicativeExpr(node->childs[0]);
    ASTAdditiveExpr *expr = new ASTAdditiveExpr(leftExpr, node->location);
    
    for (size_t index = 1; index < node->count(); index++) {
        size_t op = -1;
        if (node->childs[index]->childs[0]->assic == "+")
            op = ASTAdditiveExpr::OP_ADD;
        else if (node->childs[index]->childs[0]->assic == "-")
            op = ASTAdditiveExpr::OP_SUB;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        ASTExpr *target = (ASTExpr*)handleMultiplicativeExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for multiplicative expression
AST* ASTBuilder::handleMultiplicativeExpr(Node *node) 
{
    AssertNode("multiplicativeExpr");

    if (node->count() == 1)
        return handleUnaryExpr(node->childs[0]);
    
    ASTExpr *leftExpr = (ASTExpr*)handleUnaryExpr(node->childs[0]);
    ASTAdditiveExpr *expr = new ASTAdditiveExpr(leftExpr, node->location);
    
    for (size_t index = 1; index < node->count(); index++) {
        size_t op = -1;
        if (node->childs[index]->childs[0]->assic == "*")
            op = ASTMultiplicativeExpr::OP_MUL;
        else if (node->childs[index]->childs[0]->assic == "/")
            op = ASTMultiplicativeExpr::OP_DIV;
        else if (node->childs[index]->childs[0]->assic == "%")
            op = ASTMultiplicativeExpr::OP_MODULO;    
        else 
            throw Exception::InvalidStatement(node->assic);
        index++;     
        ASTExpr *target = (ASTExpr*)handleUnaryExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// hanlder for unary expression
AST* ASTBuilder::handleUnaryExpr(Node *node) 
{
    AssertNode("unaryExpr");

    ASTPrimaryExpr *expr = (ASTPrimaryExpr*)handlePrimary(node->childs[0]);
    if (node->count() == 1)
        return expr;
    
    ASTUnaryExpr *unaryExpr = new ASTUnaryExpr(expr, node->location);  
    for (size_t index = 1; index < node->count(); index++) {
        ASTSelectorExpr *sel = 
            (ASTSelectorExpr*)handleSelector(node->childs[index]);
        unaryExpr->appendElement(sel);
    }
    
    return unaryExpr;
}

/// handler for primary expression
AST* ASTBuilder::handlePrimary(Node *node) 
{
    AssertNode("primaryExpr"); 

    string text = node->childs[0]->assic;
    ASTExpr *expr = NULL; 
    if (text == "self")
        return new ASTPrimaryExpr(ASTPrimaryExpr::T_SELF, text, node->location);
    
    if (text == "super")
        return new ASTPrimaryExpr(ASTPrimaryExpr::T_SUPER, text, node->location);
    
    if (text == "true")
        return new ASTPrimaryExpr(ASTPrimaryExpr::T_TRUE, text, node->location);
    
    if (text == "false")
        return new ASTPrimaryExpr(ASTPrimaryExpr::T_FALSE, text, node->location);
    
    if (text == "null")
        return new ASTPrimaryExpr(ASTPrimaryExpr::T_NULL, text, node->location);
    
    if (text == "mapLiteral")
        return handleMapInitializer(node->childs[0]);
    
    if (text == "setLiteral")
        return handleArrayInitializer(node->childs[0]);
    
    
    if (isdigit(text[0])) {
        if (!text.find_last_of(".")) 
            return new ASTPrimaryExpr(ASTPrimaryExpr::T_NUMBER, 
                            node->childs[0]->assic, node->location);
        else 
            return new ASTPrimaryExpr(ASTPrimaryExpr::T_NUMBER, 
                            node->childs[0]->assic, node->location);
    } 
    if (node->count() == 3) { // compound expression 
		expr = (ASTExpr*) handleExpr(node->childs[1]);
        return new ASTPrimaryExpr(ASTPrimaryExpr::T_COMPOUND, 
                        expr, node->location);
    }
    return new ASTPrimaryExpr(ASTPrimaryExpr::T_IDENTIFIER, 
                    node->childs[0]->assic, node->location);
}

/// handler for selector
AST* ASTBuilder::handleSelector(Node *node) 
{
    AssertNode("selector");

    ASTSelectorExpr *selExpr = NULL;
    if (node->childs[0]->assic == "assignableSelector") {
        Node *subNode = node->childs[0];
        if (subNode->count() == 2) {// .identifier
            selExpr = new ASTSelectorExpr(ASTSelectorExpr::DOT_SELECTOR, 
                            subNode->childs[1]->assic, node->location);
            return selExpr;
        }
        
        else if (subNode->count() == 3) { // [ expression ]
            selExpr =  new ASTSelectorExpr(node->location);
            selExpr->m_arrayExpr = (ASTExpr*)handleExpr(subNode->childs[1]);
            selExpr->m_type = ASTSelectorExpr::ARRAY_SELECTOR;
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    else if (node->childs[0]->assic == "arguments") {
        Node *subNode = node->childs[0];
   
        selExpr = new ASTSelectorExpr(node->location);
        selExpr->m_type = ASTSelectorExpr::METHOD_SELECTOR;
        selExpr->m_methodCallExpr = new ASTMethodCallExpr(node->location);
   
        if (subNode->count() == 2)  // no argument
            return selExpr;
        
        else if (subNode->count() == 3) {
            subNode = subNode->childs[1];
            for (size_t index = 0; index < subNode->count(); index += 2) {
                ASTExpr *expr = (ASTExpr*)handleExpr(subNode->childs[index]);
                selExpr->m_methodCallExpr->appendArgument(expr);
            }
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    return NULL;
    
}

/// handler for new expression
AST* ASTBuilder::handleArgumentList(Node *node)
{
    AssertNode("argumentList");
    ASTArgumentList *argumentList = new ASTArgumentList(node->location);
    for (size_t index = 1; index < node->count() - 1; index++) {
        ASTExpr *expr = (ASTExpr*)handleExpr(node->childs[index]);
        argumentList->appendArgument(expr);
    }
    return argumentList;
}

/// handler for new expression
AST* ASTBuilder::handleNewExpr(Node *node) 
{
    AssertNode("newExpr");
    string type = node->childs[1]->childs[0]->assic;
    ASTArgumentList *list = 
        (ASTArgumentList*)handleArgumentList(node->childs[2]); 
    ASTNewExpr *newExpr = new ASTNewExpr(type, list, node->location);
    return newExpr; 
}
