//
//  ASTBuilder.cpp
//  A toyable language compiler (like a simple c++)
//

#include <algorithm>
#include "Common.h"
#include "ASTBuilder.h"
#include "AST.h"
#include "Parser.h"
#include "Type.h"
#include "Declaration.h"
#include "Statement.h"
#include "Expression.h"
#include "Exception.h"
#include "ASTxml.h"
#include "Compile.h"


#define TTEXT(node) node->assic
#define TCHILD(node, n) node->childs[n]
#define TSIZE(node) node->count()
#define TLOCATION(node) node->location

// macro to check wether the node is specified name
// the macro is only used in ast handler method, so parameter node is default
#define AssertNode(name) ASSERT(TTEXT(node) == name) 

ASTBuilder::ASTBuilder(const string& path, const string& file) 
{
    m_path = path;
    m_file = file;
}

ASTBuilder::~ASTBuilder() 
{
    
}

// method to convert a parse tree into an AST tree
AST* ASTBuilder::build(Node* parseTree) 
{
    if (!parseTree) 
        return NULL;
    
    AST*  root = new AST();
    // the root node of pareTree must be compile unit
    vector<Node* >::iterator ite = parseTree->childs.begin();
    for (; ite != parseTree->childs.end(); ite++) {
        Node*  decl = *ite;
        AST* child = handleDeclarations(decl);
        if (child)
            root->addChildNode(child);
    }
    if (CompileOption::getInstance().isOutputAST()) {
        ASTXml xml(m_path, m_file);
        xml.build(root);
    }
    
    return root;
}
/// handle primitive type
AST* ASTBuilder::handlePrimitiveType(Node* node)
{
    AssertNode("primitiveType");

    // get sub type of the primitive type
    string typeName = node->childs[0]->assic;
    int type = TypeDecl::TInvalid; 
    if (typeName == "bool")
        type = TypeDecl::TBool;
    else if (typeName == "int")
        type = TypeDecl::TInt;
    else if (typeName == "char")
        type = TypeDecl::TChar; 
    else if (typeName == "byte")
        type = TypeDecl::TByte;
    else if (typeName == "short")
        type = TypeDecl::TShort;
    else if (typeName == "long")
        type = TypeDecl::TLong;
    else if (typeName == "float")
        type = TypeDecl::TFloat;
    else if (typeName == "double")
        type = TypeDecl::TDouble;
    else if (typeName == "string")
        type = TypeDecl::TString;
    else 
        Error::complain(node->location, "unknown type '%s'", typeName.c_str());
    
    return new TypeDecl(type, typeName, node->location); 
}

/// handle class type
AST* ASTBuilder::handleClassTypeName(Node* node)
{ 
    AssertNode("classType");
    QualifiedName name;
    handleQualifiedName(node->childs[0], name);
    return  new TypeDecl(TypeDecl::TClass, name, node->location);
}
/// handle map type
AST* ASTBuilder::handleMapType(Node* node)
{
    AssertNode("mapType");
    TypeDecl* decl = new TypeDecl(TypeDecl::TMap, node->location); 
    if (node->childs[2]->childs[0]->assic == "primitiveType")
        decl->m_type1 = (TypeDecl*)handlePrimitiveType(node->childs[2]->childs[0]);

    if (node->childs[4]->childs[0]->assic == "primitiveType")
        decl->m_type1 = (TypeDecl*)handlePrimitiveType(node->childs[4]->childs[0]);
    return decl; 
}

/// handle type declaration such as int,
AST* ASTBuilder::handleType(Node* node)
{
    AssertNode("type");

    int type = TypeDecl::TInvalid;
    int  scalars = 0;
    TypeDecl* decl = NULL; 
   
    // check wether the type is array 
    if (TSIZE(node) > 1) 
        scalars = (TSIZE(node) - 1) / 2;
        
    if (node->childs[0]->assic == "primitiveType") 
        decl = (TypeDecl*)handlePrimitiveType(node->childs[0]); 
    else if (node->childs[0]->assic == "classType") 
        decl = (TypeDecl*)handleClassTypeName(node->childs[0]); 
    else if (node->childs[0]->assic == "mapType") 
        decl = (TypeDecl*)handleMapType(node->childs[0]);
    else
        Error::complain(node->location, 
                "unknown type '%s", node->childs[0]->assic.c_str());
    if (decl)
        decl->setScalars(scalars);
    
    return decl; 
}

/// handle the declaration
AST* ASTBuilder::handleDeclarations(Node* node)
{
    if (!node)
        return NULL;

    if (TTEXT(node) == "packageDeclaration")
        return handlePackageDeclaration(node);
    else if (TTEXT(node) == "importDeclaration")
        return  handleImportDeclaration(node);
    else if (TTEXT(node) == "typeDeclaration")
        return handleTypeDeclaration(node);
    else
        Error::complain(TLOCATION(node), 
                "unknown declaration '%s'", TTEXT(node).c_str());
    return NULL; 
}
/// handle type declaration
AST* ASTBuilder::handleTypeDeclaration(Node* node)
{
    AssertNode("typeDeclaration");
    
    // process the class modifier at first
    Annotation* annotation = NULL;
    int attribute = Declaration::InvalidAttribute;
    size_t index;
    for (index = 0; index < TSIZE(node) - 1; index++) 
        handleClassModifier(node->childs[index], attribute, &annotation); 

    Declaration* decl = NULL;
    if (TTEXT(TCHILD(node, index)) == "classDeclaration")
        decl = (Declaration*)handleClassDeclaration(node->childs[index]);
    else if (TTEXT(TCHILD(node, index)) == "enumDeclaration")
        decl = (Declaration*)handleEnumDeclaration(node->childs[index]);
    else if (TTEXT(TCHILD(node, index)) == "constantDeclaration")
        decl = (Declaration*)handleConstantDeclaration(node->childs[index]);
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
void ASTBuilder::handleQualifiedName(Node* node, QualifiedName& names)
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
void ASTBuilder::handleQualifiedNameList(Node* node, vector<QualifiedName>& list)
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
AST* ASTBuilder::handleAnnotation(Node* node)
{
    AssertNode("annotation");

    Annotation* annotation = new Annotation(node->location);
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
        Node* node,
        map<string, Annotation::ElementValue*>& elementValuePairs)
{
    AssertNode("elementValuePairs");
    
    for (size_t index = 0; index < TSIZE(node); index++) {
        // elementValuePair is node->childs[index];
        Node* elementValuePairNode = TCHILD(node, index);
        string identifier = elementValuePairNode->childs[0]->assic;
        Annotation::ElementValue* elementValue = new Annotation::ElementValue();
        handleAnnotationElementValue(elementValuePairNode->childs[2],
                *elementValue);
        elementValuePairs.insert(make_pair(identifier, elementValue)); 
        index++; //skip the ', operator
    }
}
/// handle element value
void ASTBuilder::handleAnnotationElementValue(
        Node* node,
        Annotation::ElementValue& elementValue)
{
    AssertNode("elementValue"); 
    
    if (TTEXT(TCHILD(node, 0))== "annotation")
        elementValue.annotation = 
            (Annotation*)handleAnnotation(node->childs[3]->childs[0]);
    else
        elementValue.expr = 
            (Expr*)handleExpr(node->childs[3]->childs[0]);
}

/// handle package declaration
AST* ASTBuilder::handlePackageDeclaration(Node* node)
{
    AssertNode("packageDeclaration");
    
    PackageDeclaration* decl = new PackageDeclaration(node->location);
    size_t index = 0;
    for (; index < TSIZE(node) - 3; index++) {
        Annotation* annotation = (Annotation*)handleAnnotation(TCHILD(node, index));
        decl->addAnnotation(annotation); 
    }
    index++; // skip the 'package' keyword
    handleQualifiedName(TCHILD(node, index), decl->m_qualifiedName);
    return decl;
}

/// handle import declaration 
AST* ASTBuilder::handleImportDeclaration(Node* node) 
{
    AssertNode("importDeclaration");
    ImportDeclaration *decl = new ImportDeclaration(node->location);
    handleQualifiedName(TCHILD(node, 1), decl->m_qualifiedName); 
    if (TSIZE(node) == 5)
        decl->m_isImportAll = true;
    return decl;
}

/// handle class declaration 
AST* ASTBuilder::handleClassDeclaration(Node* node) 
{
    
    AssertNode("classDeclaration");

    int index = 1; // skip the 'class' keyword
    string clsName = node->childs[index++]->assic;

    // base class name 
    QualifiedName baseClsName;
    if (TTEXT(TCHILD(node, index)) == "extend") {
        index++;
        handleQualifiedName(TCHILD(node, index), baseClsName);
        index++; 
    }
    // abstract class list 
    vector<QualifiedName> qualifiedNameList; 
    if (TTEXT(TCHILD(node, index)) == "implements") {
        index++;
        handleQualifiedNameList(node->childs[index], qualifiedNameList);
        index++; 
    }
    
    Class* cls = new Class(clsName, baseClsName, qualifiedNameList, node->location);
   
    index++; // skip the '{' operator
    while (index < TSIZE(node) - 1) {
        Declaration* decl = 
            (Declaration*)handleClassBodyDeclaration(node->childs[index], clsName);
        cls->addDeclaration(decl); 
        index++; 
    }
    return cls; 
}
/// handle class modifier
void ASTBuilder::handleClassModifier(Node* node,
        int& attribute, 
        Annotation** annotation)
{
    ASSERT(annotation != NULL);
    AssertNode("classModifier");

    if (TTEXT(TCHILD(node, 0)) == "annotation") {
        *annotation = (Annotation*)handleAnnotation(TCHILD(node, 0));
        attribute &= Declaration::AnnotationAttribute;
    }
    else if (TTEXT(TCHILD(node, 0)) == "public")
        attribute |= Declaration::PublicAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "private")
        attribute |= Declaration::PrivateAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "protected")
        attribute |= Declaration::ProtectedAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "static")
        attribute |= Declaration::StaticAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "abstract")
        attribute |= Declaration::AbstractAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "final")
        attribute |= Declaration::FinalAttribute; 
    else if (TTEXT(TCHILD(node,0)) == "const")
        attribute |= Declaration::ConstAttribute;
}

void ASTBuilder::handleModifier(Node* node,
        int& attribute,
        Annotation** annotation)
{
    AssertNode("modifier");
    
    if (node->childs[0]->assic == "classModifier")
        return handleClassModifier(node->childs[0], attribute, annotation);

    if (TTEXT(TCHILD(node, 0)) == "native")
        attribute |= Declaration::NativeAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "sychronized")
        attribute |= Declaration::SychronizedAttribute;

}


/// hanlde class body declaration 
AST* ASTBuilder::handleClassBodyDeclaration(Node* node, const string& cls) 
{
    AssertNode("classBodyDeclaration");

    if (TSIZE(node) == 1)
        return NULL;
    // process the class modifier at first
    Annotation* annotation = NULL;
    int attribute = Declaration::InvalidAttribute;
    size_t i = 0; 
    for (; i < TSIZE(node) - 1; i++) 
        handleModifier(TCHILD(node, i), attribute, &annotation);
    
        
    // handle the member declaration
    Declaration* decl = 
        (Declaration*)handleClassMemberDeclaration(TCHILD(node, i), cls);
    if (decl) {
        decl->setAttribute(attribute);
        decl->setAnnotation(annotation);
    }
    return decl;
}
/// handle class member declaration
AST* ASTBuilder::handleClassMemberDeclaration(Node* node, const string& clsName)
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
AST* ASTBuilder::handleEnumDeclaration(Node* node)
{
    return NULL;
}

/// handle constant declaration
AST* ASTBuilder::handleConstantDeclaration(Node* node)
{
    return NULL;
}

/// handle class method
AST* ASTBuilder::handleMethodDeclaration(Node* node, const string& clsName) 
{
    AssertNode("methodDeclaration"); 
    int index = 0; 
    // return type and method name
    TypeDecl* retType = NULL;
    if (node->childs[index]->assic != "void")
        retType = (TypeDecl*)handleType(node->childs[index]);
    else
        retType = new TypeDecl(TypeDecl::TVoid, 
                        node->childs[index]->assic,
                        node->location);
    
    index++; 
    string methodName = node->childs[index++]->assic;
    // method parameter list
    FormalParameterList* formalParameterList = 
        (FormalParameterList*)handleFormalParameters(node->childs[index]);    
    
    // check to see wethe the exception is thrown
    vector<QualifiedName> qualifiedNameList;
    if (node->childs[index]->assic == "throw") {
        // handle qualified name list
        index++; 
        handleQualifiedNameList(node->childs[index++], qualifiedNameList);
    }
    // check the method body 
    MethodBlock* methodBlock = NULL; 
    if (node->childs[index]->assic ==  "methodBody")
        methodBlock = (MethodBlock*)handleMethodBlock(node->childs[index]);

    // create method instance 
    Method* method = new Method(retType, 
                            methodName,
                            clsName,
                            formalParameterList, 
                            node->location);
    // tell method wether it will throw exception 
    method->setWetherThrowException(true, qualifiedNameList); 
    method->m_block = methodBlock; 
    return method;
}

/// handle variable declaration
AST* ASTBuilder::handleFieldDeclaration(Node* node, const string& clsName) 
{
    AssertNode("fieldDeclaration");

    TypeDecl* type = (TypeDecl*)handleType(node->childs[0]);
    return handleVariableDeclarators(node->childs[1], type); 
}
// handle variableDeclarators
AST* ASTBuilder::handleVariableDeclarators(Node* node, TypeDecl* type)
{
    AssertNode("variableDeclarators");
    
    string varName;
    int scalars = 0;
    handleVariableDeclaratorId(node->childs[0], varName, scalars); 
    if (TSIZE(node) > 2)
        AST* initializer = handleVariableInitializer(node->childs[2]);
    return new Variable(type, varName, node->location);  
}

/// handle to generate an initializer
AST* ASTBuilder::handleVariableInitializer(Node* node)
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
}
/// handle array initializer
AST* ASTBuilder::handleArrayInitializer(Node* node)
{
    AssertNode("arrayInitializer");

    ArrayInitializer* arrayInitializer = new ArrayInitializer(node->location); 
    for (int index = 1; index < TSIZE(node) - 1; index++) {
        AST* initializer = handleVariableInitializer(node->childs[index]);
        arrayInitializer->addInitializer(initializer);
        if (node->childs[index]->assic == ",")
            continue;
    }
    return arrayInitializer; 
}
/// handle map initializer
AST* ASTBuilder::handleMapInitializer(Node* node)
{
    AssertNode("mapInitializer");
    
    MapInitializer* mapInitializer = new MapInitializer(node->location);
    for (int index = 1; index < TSIZE(node) - 1; index++) {
        MapPairItemInitializer* initializer = (MapPairItemInitializer*)
                         handleMapPairItemInitializer(node->childs[index]);
        mapInitializer->addInitializer(initializer);
        if (node->childs[index]->assic == ",")
            continue;
    }
    return mapInitializer; 
}
/// handle map item pair initializer
AST* ASTBuilder::handleMapPairItemInitializer(Node* node) 
{
    AssertNode("mapItemPairInitializer");
    
    MapPairItemInitializer* mapPairItemInitializer = 
        new MapPairItemInitializer(node->location);
    mapPairItemInitializer->m_key =
        handleVariableInitializer(node->childs[0]);
    mapPairItemInitializer->m_val =
        handleVariableInitializer(node->childs[2]);
    return mapPairItemInitializer;
}
/// handle formal parameters
AST* ASTBuilder::handleFormalParameters(Node* node)
{
    AssertNode("formalParameters");
    
    if (TSIZE(node) == 3)
        return handleFormalParameterList(node->childs[1]);
    return NULL;
}

/// handle formal parameter list
AST* ASTBuilder::handleFormalParameterList(Node* node) 
{
    AssertNode("formalParameterList");

    FormalParameterList* formalParameterList =
        new FormalParameterList(node->location);
    
    // handle normal parameter
    for (int index = 0; index < TSIZE(node); index++) {
        FormalParameter* parameter = 
            (FormalParameter*) handleFormalParameter(node->childs[index]);
        formalParameterList->addParameter(parameter);
        index++; // skop the token ','
    }
        
    return formalParameterList;
}
/// handle variable modifier
void ASTBuilder::handleVariableModifier(
        Node* node,
        int& attribute,
        Annotation** annotation)
{
    AssertNode("varaibleModifier");
    ASSERT(annotation != NULL);

    if (node->childs[0]->assic == "final")
        attribute &= Declaration::FinalAttribute;
    else if (node->childs[0]->assic == "const")
        attribute &= Declaration::ConstAttribute;
    else if (node->childs[0]->assic == "annotation")
        *annotation = (Annotation*)handleAnnotation(node->childs[0]);
    else
        Error::complain(node->location,
                "unknown variable attribute '%s'",
                node->childs[0]->assic.c_str());
}

/// handle variable declarator id
void ASTBuilder::handleVariableDeclaratorId(
        Node* node,
        string& variableName,
        int& scalars)
{
    AssertNode("variableDeclaratorId");
    variableName = node->childs[0]->assic;
    if (TSIZE(node) > 1)
        scalars = (TSIZE(node) - 1) / 2;
}
/// handle formal parameter
AST* ASTBuilder::handleFormalParameter(Node* node) 
{
    AssertNode("formalParameter");

    int attribute = Declaration::InvalidAttribute; 
    int index = 0;
    Annotation* annotation = NULL; 
    
    while (index < (TSIZE(node) - 2)){
        if (node->childs[index]->assic != "variableModifier")
            break;
        handleVariableModifier(node->childs[index], attribute, &annotation);
    }
    // get type name and id
    TypeDecl* variableType = 
        (TypeDecl*)handleType(node->childs[index++]);
    string variableName;
    int scalars = 0;
    handleVariableDeclaratorId( node->childs[index], variableName, scalars);
    FormalParameter* formalParameter = 
        new  FormalParameter(variableType, variableName, node->location);
    formalParameter->setAttribute(attribute);
    formalParameter->setScalars(scalars);
    return formalParameter;
}

/// handle method block
AST* ASTBuilder::handleMethodBlock(Node* node) 
{
    AssertNode("methodBody");
    
    Block* block = (Block*)handleBlock(node->childs[0]);
    MethodBlock* methodBlock = new MethodBlock(block, node->location);
    return methodBlock;
}
//
// Statements
//

/// handle block
AST* ASTBuilder::handleBlock(Node* node)
{
    AssertNode("block");
    
    Block* block = new Block(node->location);
    for (size_t index = 1; index < TSIZE(node) - 1; index++) {
        Statement *stmt = (Statement*)handleStatement(node->childs[index]);
        block->addStatement(stmt);
    }
    return block;
}

/// handle block statement
AST* ASTBuilder::handleBlockStatement(Node* node)
{
    AssertNode("blockStatement");
   
    if (node->childs[0]->assic == "localVariableDeclarationStatement")
        return handleLocalVariableDeclarationStatement(node->childs[0]);
    else if (node->childs[0]->assic == "statement")
        return handleStatement(node->childs[0]);
  //  else if (node->childs[0]->assic == "typeDeclaration")
  //      return handleTypeDeclaration(node->childs[0]);
    else
        Error::complain(node->location,
                "unknow statement '%s'",
                node->childs[0]->assic.c_str());
    return NULL;
}

/// handle statement
AST* ASTBuilder::handleStatement(Node* node) 
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
AST* ASTBuilder::handleLocalVariableDeclarationStatement(Node* node) 
{
    AssertNode("localVariableDeclarationStatement");

    return handleLocalVariableDeclaration(node->childs[0]);
}
/// handle local variable declaration
AST* ASTBuilder::handleLocalVariableDeclaration(Node* node)
{
    AssertNode("localVariableDeclaration");

    int attribute = Declaration::InvalidAttribute; 
    int index = 0;
    Annotation* annotation = NULL; 
    
    while (index < (TSIZE(node) - 2)){
        if (node->childs[index]->assic != "variableModifier")
            break;
        handleVariableModifier(node->childs[index], attribute, &annotation);
    }
    TypeDecl* variableType = 
        (TypeDecl*)handleType(node->childs[index++]);
    return handleVariableDeclarators(node->childs[index], variableType);
}
/// handle if statement
AST* ASTBuilder::handleIfStatement(Node* node) 
{
    AssertNode("ifStatement");

    Expr* conditExpr = (Expr*)handleCompareExpr(node->childs[2]);
    Statement* stmt1 = NULL;
    Statement* stmt2 = NULL;
    // if statement 
    stmt1 = (Statement*)handleStatement(node->childs[4]);
    // else part 
    if (node->count() == 7) 
        stmt2 = (Statement*)handleStatement(node->childs[6]);
    return new IfStatement(conditExpr, stmt1, stmt2, node->location);
}
/// handle for initializer
AST* ASTBuilder::handleForInitializer(Node* node)
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
AST* ASTBuilder::handleForStatement(Node* node) 
{
    AssertNode("forStatement");

    int index = 2; // skip the 'for' and '(' keyword
    AST* initializer = NULL;
    Expr* loopCondition = NULL;
    ExprList* exprList = NULL;
    
    if (node->childs[index]->assic == "forInitializer")
        initializer = handleForInitializer(node->childs[index++]);
    index++;

    if (node->childs[index]->assic == "expression")
        loopCondition  = (Expr*)handleExpr(node->childs[index++]);
    index++;

    if (node->childs[index]->assic == "expressionList")
        exprList = (ExprList*)handleExprList(node->childs[index++]);
    index++;

    Statement* stmt = NULL;
    stmt = (Statement*)handleStatement(node->childs[index]);
    return new ForStatement(initializer, 
                    loopCondition, exprList, stmt, node->location);
}

/// handle foreach statement
AST* ASTBuilder::handleForeachStatement(Node* node) 
{
    AssertNode("foreachStatement"); 
    
    
    ForeachStatement* foreachStmt = new ForeachStatement(node->location); 

    // check the foreachVarItem
    foreachStmt->m_variable1 = (Variable*)handleForeachVariable(node->childs[2]);
    int index = 3;

    if (node->childs[index]->assic == ",") {
        foreachStmt->m_variable2 = 
            (Variable*)handleForeachVariable(node->childs[index]); 
        index++;
    }
    index++; // skip the 'in' keyword
    IterableObjectDecl* decl = 
        (IterableObjectDecl*)handleIterableObject(node->childs[index]); 
    foreachStmt->m_stmt = (Statement*)handleStatement(node->childs[index]);  
    return foreachStmt;
}
/// handle variable declaration in foreach statement
AST* ASTBuilder::handleForeachVariable(Node* node)
{
    AssertNode("foreachVariable");
    // the foreach variable may have type 
    if (TSIZE(node) == 2) {
        // the foreach variable have type declaration
        TypeDecl* variableType = (TypeDecl*)handleType(node->childs[0]);
        string variableName = node->childs[1]->assic;
        return new Variable(variableType, variableName, node->location);
    }
    else {
        string variableName = node->childs[0]->assic;
        return new Variable(NULL, variableName, node->location);
    }
}

/// handle iteralbe object declaration in foreach statement
AST* ASTBuilder::handleIterableObject(Node* node)
{
    AssertNode("iterableObject");

    IterableObjectDecl* iterableObject = NULL;

    if (node->childs[0]->assic == "IDENTIFIER") {
        iterableObject =  new IterableObjectDecl(node->location);
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
AST* ASTBuilder::handleWhileStatement(Node* node) 
{
    Expr* conditExpr = (Expr*)handleCompareExpr(node->childs[2]);
    Statement* stmt = (Statement*)handleStatement(node->childs[4]);
    return new WhileStatement(conditExpr, stmt, node->location);
}

/// handle do while statement
AST* ASTBuilder::handleDoStatement(Node* node) 
{
    Expr* conditExpr = (Expr*)handleCompareExpr(node->childs[2]);
    Statement* stmt = (Statement*)handleStatement(node->childs[4]);
    return new DoStatement(conditExpr, stmt, node->location);
}

/// handler for switch statement
AST* ASTBuilder::handleSwitchStatement(Node* node) 
{
    Expr* resultExpr = (Expr*)handleExpr(node->childs[2]);
    SwitchStatement* switchStmt  = new SwitchStatement(resultExpr, node->location);
    
    for (int index = 5; index < node->count(); index++) {
        if (node->childs[index]->assic == "switchCase") {
            Node* subnode = node->childs[index];
            vector<Expr* >* exprList = new vector<Expr* >();
            for (int subIndex = 0; subIndex < subnode->count() - 1; subIndex += 3) {
                Expr* caseExpr = 
                (Expr*)handleExpr(subnode->childs[subIndex + 1]);
                exprList->push_back(caseExpr);
            }
            Statement* stmt = 
            (Statement*)handleSwitchStatement(subnode->childs[subnode->count()-1]);
            switchStmt->addCaseStatement(exprList, stmt);
            
        }
        else if (node->childs[index]->assic == "defaultCase") {
            Node* subnode = node->childs[index];
            Statement* stmt = (Statement*)handleSwitchStatement(subnode->childs[2]);
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
AST* ASTBuilder::handleReturnStatement(Node* node) 
{
    Expr* expr = NULL;
    if (node->count() == 3)
        expr = (Expr*)handleExpr(node->childs[1]);
    return new ReturnStatement(expr, node->location);
}
/// hander for assert statement
AST* ASTBuilder::handleAssertStatement(Node* node) 
{
    Expr*  expr = (Expr*)handleExpr(node->childs[1]);
    return new AssertStatement(expr, node->location);
}

/// hander for break statement
AST* ASTBuilder::handleBreakStatement(Node* node) 
{
    return new BreakStatement(node->location);
}

/// hander for continue statement
AST* ASTBuilder::handleContinueStatement(Node* node) 
{
    return new ContinueStatement(node->location);
}
/// hander for throw statement
AST* ASTBuilder::handleThrowStatement(Node* node) 
{
    Expr* expr = NULL;
    if (node->count() == 3)
        expr = (Expr*)handleExpr(node->childs[1]);
    return new ThrowStatement(expr, node->location);
}

/// hander for try statement
AST* ASTBuilder::handleTryStatement(Node* node) 
{
    BlockStatement* blockStmt = 
        (BlockStatement*)handleBlockStatement(node->childs[1]);
    TryStatement* tryStmt = new TryStatement(blockStmt, node->location);
    
    for (int index = 2; index < node->count(); index ++) {
        if (node->childs[index]->assic == "catchPart") {
            CatchStatement* catchStmt = 
                        (CatchStatement*)handleCatchStatement(node->childs[index]);
            tryStmt->addCatchPart(catchStmt);
        }
        else if (node->childs[index]->assic == "finallyPart") {
            FinallyCatchStatement* finallyStmt = (FinallyCatchStatement*)
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
AST* ASTBuilder::handleCatchStatement(Node* node) 
{
    string type = node->childs[1]->childs[0]->assic;
    string id = node->childs[2]->childs[0]->assic;
    BlockStatement* blockStmt = 
                (BlockStatement*)handleBlockStatement(node->childs[5]);
    
    return new CatchStatement(type, id, blockStmt, node->location);
}

/// hander for finally catch statement
AST* ASTBuilder::handleFinallyCatchStatement(Node* node) 
{
    BlockStatement* blockStmt = 
        (BlockStatement*)handleBlockStatement(node->childs[1]);
    return new FinallyCatchStatement(blockStmt, node->location);
}

/// handler for expression statement
AST* ASTBuilder::handleExprStatement(Node* node) 
{
    ExprStatement* stmt = new ExprStatement(node->location);
    stmt->m_target = (Expr*)handleExpr(node->childs[0]);
    return stmt;
}

/// hander for funcation statement
AST* ASTBuilder::handleMethodCallStatement(Node* node) 
{
    return NULL;
}

/// handler for expression list
AST* ASTBuilder::handleExprList(Node* node) 
{
    ExprList* exprList = new ExprList(node->location);
    Expr* expr = NULL;
    
    for (int index = 0; index < node->count(); index++) {
        expr = (Expr*)handleExpr(node->childs[index]);
        exprList->appendExpr(expr);
        index++;
    }
    return exprList;
}

/// handler for expression

AST* ASTBuilder::handleCompareExpr(Node* node)
{
    if (node->count() == 1)
        return handleExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleExpr(node->childs[0]);
    ComparisonExpr* comparExpr = new ComparisonExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index ++ ) {
        string op = node->childs[index++]->assic; 
        Expr* target = (Expr*)handleExpr(node->childs[index]);
        comparExpr->appendElement(op, target);
    }
    return comparExpr;
}

AST* ASTBuilder::handleExpr(Node* node) 
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
AST* ASTBuilder::handleAssignmentExpr(Node* node) 
{
    if (node->childs[0]->assic == "logicalExpr")
        return handleAssignmentExpr(node->childs[0]);
    
    if (node->count() == 1)
        return handleConditionalExpr(node);
    
    Expr* leftExpr = (Expr*)handleUnaryExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    Expr* rightExpr = (Expr*)handleAssignmentExpr(node->childs[2]);
    return new AssignmentExpr(op, leftExpr, rightExpr, node->location);
}

/// handler for conditional expression
AST* ASTBuilder::handleConditionalExpr(Node* node) 
{
    return handleLogicOrExpr(node);
}

/// handler for logic or expression
AST* ASTBuilder::handleLogicOrExpr(Node* node) 
{
    if (node->count() == 1)
        return handleLogicAndExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleLogicAndExpr(node->childs[0]);
    LogicOrExpr* logicOrExpre = new LogicOrExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleLogicAndExpr(node->childs[index]);
        logicOrExpre->appendElement(target);
    }
    return logicOrExpre;
}

/// handler for logic and expression
AST* ASTBuilder::handleLogicAndExpr(Node* node) 
{
    if (node->count() == 1)
        return handleBitwiseOrExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleBitwiseOrExpr(node->childs[0]);
    LogicAndExpr* logicAndExpr = new LogicAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleBitwiseAndExpr(node->childs[index]);
        logicAndExpr->appendElement(target);
    }
    return logicAndExpr;
}

/// handler  for bitwise or expression
AST* ASTBuilder::handleBitwiseOrExpr(Node* node) 
{
    if (node->count() == 1)
        return handleBitwiseXorExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleBitwiseXorExpr(node->childs[0]);
    BitwiseOrExpr* expr = new BitwiseOrExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleBitwiseXorExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}


/// handler for bitwise xor expression
AST* ASTBuilder::handleBitwiseXorExpr(Node* node) 
{
    if (node->count() == 1)
        return handleBitwiseAndExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleBitwiseAndExpr(node->childs[0]);
    BitwiseAndExpr* expr = new BitwiseAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleBitwiseAndExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// handler for bitwise and expression
AST* ASTBuilder::handleBitwiseAndExpr(Node* node) 
{
    if (node->count() == 1)
        return handleEqualityExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleEqualityExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    BitwiseAndExpr* expr = new BitwiseAndExpr(leftExpr, node->location);
    
    for (int index = 2; index < node->count(); index += 2) {
        Expr* target = (Expr*)handleEqualityExpr(node->childs[index]);
        expr->appendElement(target);
    }
    return expr;
}

/// handler for equality expression
AST* ASTBuilder::handleEqualityExpr(Node* node) 
{
    if (node->count() == 1)
        return handleRelationalExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleRelationalExpr(node->childs[0]);
    EqualityExpr* expr = new EqualityExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        int op = -1;
        if (node->childs[index]->childs[0]->assic == "==")
            op = EqualityExpr::OP_EQ;
        else if (node->childs[index]->childs[0]->assic == "!=")
            op = EqualityExpr::OP_NEQ;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        Expr* target = (Expr*)handleRelationalExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for relational expression
AST* ASTBuilder::handleRelationalExpr(Node* node) 
{
    if (node->count() == 1)
        return handleShiftExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleShiftExpr(node->childs[0]);
    RelationalExpr* expr = new RelationalExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        
        
        int op = -1;
        if (node->childs[index]->childs[0]->assic == ">")
            op = RelationalExpr::OP_GT;
        else if (node->childs[index]->childs[0]->assic == "<")
            op = RelationalExpr::OP_LT;
        else if (node->childs[index]->childs[0]->assic == ">=")
            op = RelationalExpr::OP_GTEQ;
        else if (node->childs[index]->childs[0]->assic == "<=")
            op = RelationalExpr::OP_LTEQ;
        else 
            throw Exception::InvalidStatement(node->assic);        
        index++; 
        Expr* target = (Expr*)handleShiftExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for shift expression
AST* ASTBuilder::handleShiftExpr(Node* node) 
{
    if (node->count() == 1)
        return handleAdditiveExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleAdditiveExpr(node->childs[0]);
    ShiftExpr* expr = new ShiftExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        int op = -1;
        if (node->childs[index]->childs[0]->assic == ">>")
            op = ShiftExpr::OP_RSHIFT;
        else if (node->childs[index]->childs[0]->assic == "<<")
            op = ShiftExpr::OP_LSHIFT;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        Expr* target = (Expr*)handleAdditiveExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for additive expression
AST* ASTBuilder::handleAdditiveExpr(Node* node) 
{
    if (node->count() == 1)
        return handleMultiplicativeExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleMultiplicativeExpr(node->childs[0]);
    AdditiveExpr* expr = new AdditiveExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        int op = -1;
        if (node->childs[index]->childs[0]->assic == "+")
            op = AdditiveExpr::OP_PLUS;
        else if (node->childs[index]->childs[0]->assic == "-")
            op = AdditiveExpr::OP_SUB;
        else 
            throw Exception::InvalidStatement(node->assic);
        index++; 
        Expr* target = (Expr*)handleMultiplicativeExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// handler for multiplicative expression
AST* ASTBuilder::handleMultiplicativeExpr(Node* node) 
{
    if (node->count() == 1)
        return handleUnaryExpr(node->childs[0]);
    
    Expr* leftExpr = (Expr*)handleUnaryExpr(node->childs[0]);
    AdditiveExpr* expr = new AdditiveExpr(leftExpr, node->location);
    
    for (int index = 1; index < node->count(); index++) {
        int op = -1;
        if (node->childs[index]->childs[0]->assic == "*")
            op = MultiplicativeExpr::OP_MUL;
        else if (node->childs[index]->childs[0]->assic == "/")
            op = MultiplicativeExpr::OP_DIV;
        else if (node->childs[index]->childs[0]->assic == "%")
            op = MultiplicativeExpr::OP_MODULO;    
        else 
            throw Exception::InvalidStatement(node->assic);
        index++;     
        Expr* target = (Expr*)handleUnaryExpr(node->childs[index]);
        expr->appendElement(op, target);
    }
    return expr;
}

/// hanlder for unary expression
AST* ASTBuilder::handleUnaryExpr(Node* node) 
{
    PrimaryExpr* expr = (PrimaryExpr*)handlePrimary(node->childs[0]);
    if (node->count() == 1)
        return expr;
    
    UnaryExpr* unaryExpr = new UnaryExpr(expr, node->location);  
    for (int index = 1; index < node->count(); index++) {
        SelectorExpr* sel = (SelectorExpr*)handleSelector(node->childs[index]);
        unaryExpr->appendElement(sel);
    }
    
    return unaryExpr;
}

/// handler for primary expression
AST* ASTBuilder::handlePrimary(Node* node) 
{
    string text = node->childs[0]->assic;
    Expr* expr = NULL; 
    if (text == "self")
        return new PrimaryExpr(PrimaryExpr::T_SELF, text, node->location);
    
    if (text == "super")
        return new PrimaryExpr(PrimaryExpr::T_SUPER, text, node->location);
    
    if (text == "true")
        return new PrimaryExpr(PrimaryExpr::T_TRUE, text, node->location);
    
    if (text == "false")
        return new PrimaryExpr(PrimaryExpr::T_FALSE, text, node->location);
    
    if (text == "null")
        return new PrimaryExpr(PrimaryExpr::T_NULL, text, node->location);
    
    if (text == "mapLiteral")
        return handleMapInitializer(node->childs[0]);
    
    if (text == "setLiteral")
        return handleArrayInitializer(node->childs[0]);
    
    
    if (isdigit(text[0])) {
        if (!text.find_last_of(".")) 
            return new PrimaryExpr(PrimaryExpr::T_NUMBER, 
                            node->childs[0]->assic, node->location);
        else 
            return new PrimaryExpr(PrimaryExpr::T_NUMBER, 
                            node->childs[0]->assic, node->location);
    } 
    if (node->count() == 3) { // compound expression 
		expr = (Expr*) handleExpr(node->childs[1]);
        return new PrimaryExpr(PrimaryExpr::T_COMPOUND, 
                        expr, node->location);
    }
    return new PrimaryExpr(PrimaryExpr::T_IDENTIFIER, 
                    node->childs[0]->assic, node->location);
}

/// handler for selector
AST* ASTBuilder::handleSelector(Node* node) 
{
    SelectorExpr* selExpr = NULL;
    
    if (node->childs[0]->assic == "assignableSelector") {
        Node* subNode = node->childs[0];
        if (subNode->count() == 2) {// .identifier
            selExpr = new SelectorExpr(SelectorExpr::DOT_SELECTOR, 
                            subNode->childs[1]->assic, node->location);
            return selExpr;
        }
        
        else if (subNode->count() == 3) { // [ expression ]
            selExpr =  new SelectorExpr(node->location);
            selExpr->m_arrayExpr = (Expr*)handleExpr(subNode->childs[1]);
            selExpr->m_type = SelectorExpr::ARRAY_SELECTOR;
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    else if (node->childs[0]->assic == "arguments") {
        Node* subNode = node->childs[0];
   
        selExpr = new SelectorExpr(node->location);
        selExpr->m_type = SelectorExpr::METHOD_SELECTOR;
        selExpr->m_methodCallExpr = new MethodCallExpr(node->location);
   
        if (subNode->count() == 2)  // no argument
            return selExpr;
        
        else if (subNode->count() == 3) {
            subNode = subNode->childs[1];
            for (int index = 0; index < subNode->count(); index += 2) {
                Expr* expr = (Expr*)handleExpr(subNode->childs[index]);
                selExpr->m_methodCallExpr->appendArgument(expr);
            }
            return selExpr;
        }
        throw Exception::InvalidExpr(node->childs[0]->assic);
    }
    
    return NULL;
    
}

/// handler for new expression
AST* ASTBuilder::handleArgumentList(Node* node)
{
    ArgumentList* argumentList = new ArgumentList(node->location);
    for (int index = 1; index < node->count() - 1; index++) {
        Expr* expr = (Expr*)handleExpr(node->childs[index]);
        argumentList->appendArgument(expr);
    }
    return argumentList;
}
AST* ASTBuilder::handleNewExpr(Node* node) 
{
    string type = node->childs[1]->childs[0]->assic;
    ArgumentList* list = 
        (ArgumentList*)handleArgumentList(node->childs[2]); 
    NewExpr* newExpr = new NewExpr(type, list, node->location);
    return newExpr; 
}
