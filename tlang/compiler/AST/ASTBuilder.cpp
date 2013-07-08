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
#include "Variable.h"
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
        Node*  decls = *ite;
        AST* child = handleDecl(decls->childs[0]);
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
    if (typeName == "bool")
        type = TypeDecl::TBool;
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
    
    return new TypeDecl(type, typeName, node->location); 
}

/// handle class type
AST* ASTBuilder::handleClassTypeName(Node* node)
{ 
    AssertNode("classType");
    QualifiedName name;
    handleQualifiedName(node->childs[0], name);
    return  new TypeDecl(name, node->location);
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
        decl = handlePrimitiveType(node->childs[0]); 
    else if (node->childs[0]->assic == "classType") 
        decl = handleClassTypeName(node->childs[0]); 
    else if (node->childs[0]->assic = "mapType") 
        decl = handleMapTypeDecl(node->childs[0]);
    else
        Error::complain(node->location, 
                "unknown type '%s", node->childs[0]->assic.c_str());
    if (decl)
        decl->seScalars(scalars);
    
    return decl; 
}

/// handle the declaration
AST* ASTBuilder::handleDeclarations(Node* node)
{
    if (!node)
        return NULL;

    if (TTEXT(node) == "packageDeclaration")
        return handlePackageDecl(node);
    else if (TTEXT(node) == "importDeclaration")
        return  handleImportDecl(node);
    else if (TTEXT(node) == "typeDeclaration")
        return handleTypeDecl(node);
    else
        Error::complain(TLOCATION(node), 
                "unknown declaration '%s'\n", TEXT(node).c_str());
    return NULL; 
}
/// handle type declaration
AST* ASTBuilder::handleTypeDeclaration(Node* node)
{
    AssertNode("typeDeclaration");
    
    // process the class modifier at first
    Annotaton* annotation = NULL;
    int attribute = DeclarationAttribute::InvalidAttribute;
    size_t index;
    for (index = 0; index < TSIZE(node) - 1; index++) {
        if (TTEXT(TCHILD(node, index)) == "annotation") {
            annotation = (Annotation*)handleAnnotation(TCHILD(node, index));
            attribute &= Declaration:AnnotationAttribute;
        }
        else if (TTEXT(TCHILD(node, index)) == "public")
            attribute &= Declaration::PublicAttribute;
        else if (TTEXT(TCHILD(node, index)) == "private")
            attribute &= Declaration::PrivateAttribute;
        else if (TTEXT(TCHILD(node, index)) == "protected")
            attribute &= Declaration::ProtectedAttribute;
        else if (TTEXT(TCHILD(node, index)) == "static")
            attribute &= Declaration::StaticAttribute;
        else if (TTEXT(TCHILD(node, index)) == "abstract")
            attribute &= Declaration::AbstractAttribute;
        else if (TTEXT(TCHILD(node, index)) == "final")
            attribute &= Declaration::FinalAttribute; 
    }

    Declaration* decl = NULL;
    if (TTEXT(TCHILD(node, index)) == "classDeclaration")
        decl = (Declaration*)handleClassDecl(node);
    else if (TTEXT(TCHILD(node, index)) == "enumDeclaration")
        decl = (Declaration*)handleEnumDecl(node);
    else if (TTEXT(TCHILD(node, index)) == "constantDeclaration")
        decl = (Declaration*)handleConstantDecl(node);
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
void ASTBuilder::handleQualifiedNameList(Node* node, vector<QualifedName>& list)
{
    AssertNode("qualifedNameList");
    
    for (size_t index = 0; index < TSIZE(node), index++) {
        QualifedName name;
        handleQualifedName(TCHILD(node, index), name);
        list.push_back(name);
        index++; // skip ','
    }
}
/// handle annotation declaration
AST* ASTBuilder::handleAnnotation(Node* node)
{
    AssertNode("annotation");

    Annotation* annotation = new Annotation(node->location);
    handleQualifiedName(node->childs[1]->childs[0], annotaion->m_qualifiedName);
    if (TSIZE(node) == 4) {
        // there will be elmentPairs or  value
        if (TTEXT(TCHILD(node, 3)) == "elementValuePairs") 
           handleAnnotationElementValuePairs(TCHILD(node, 3), 
                   annotation->m_elmentPiars); 
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
        map<string, ElementValue*>& elementValuePairs)
{
    AssertNode("elementValuePairs");
    
    for (size_t index = 0; index < TSIZE(node); index++) {
        // elementValuePair is node->childs[index];
        Node* elementValuePairNode = TCHILDS(node, index);
        string identifier = elementValuePairNode->childs[0]->assic;
        Annotation::ElementValue* elementValue = new Annotation::ElementValue();
        handleAnnotationElementValue(elementValuePairNode->childs[2],
                elementValue);
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
            (Annotation*)handleAnnotationDecl(node->childs[3]->childs[0]);
    else
        elementValue.m_expr = 
            (Expr*)handleExpr(node->childs[3]->childs[0]);
}

/// handle package declaration
AST* ASTBuilder::handlePackageDeclaration(Node* node)
{
    AssertNode("packageDeclaration");
    
    PackageDecl* decl = new PackageDecl(node->location);
    size_t index = 0;
    for (; index < TSIZE(node) - 3; index++) {
        Annotation* annotation = (Annotation*)handleAnnotation(TCHILD(index));
        pkgDecl->addAnnotation(annotation); 
    }
    index++; // skip the 'package' keyword
    handleQualifiedName(TCHILD(node, index), decl->m_qualifiedName);
    return decl;
}

/// handle import declaration 
AST* ASTBuilder::handleImportDeclaration(Node* node) 
{
    AssertNode("importDeclaration");
    ImportDecl *decl = new ImportDecl(node->location);
    handleQualifiedName(TCHILD(node, 1), decl->m_qualifiedName); 
    if (TSIZE(node) == 5)
        decl->m_isImportAll = true;
    return decl;
}

/// handle class declaration 
AST* ASTBuilder::handleClassDeclaration(Node* node) 
{
    int index = 1; // skip the 'class' keyword
    string clsName = node->childs[index++]->assic;

    // base class name 
    QualifedName baseClsName;
    if (TTEXT(TCHILD(node, index)) == "extend") {
        index++;
        handleQualifedName(TCHILD(node, index), baseClsName);
    }
    // abstract class list 
    vector<QualifedName> qualifedNameList; 
    if (TTEXT(TCHILD(node, index)) == "implements") {
        index++;
        handleQualifiedNameList(node->childs[index], qualifiedNameList);
    }
    
    Class* cls = new Class(clsName, baseClsName, qualiefiedNameList);
   
    index++; // skip the '{' operator
    while (index < TSIZE(node) - 1) {
        Declaration* decl = 
            (Declaration*)handleClassBodyDeclaration(node->childs[index], clsName);
        cls->addDeclaration(decl); 
    }
    return cls; 
}
/// handle class modifier
void ASTBuilder::handleClassModifier(Node* node,
        int &attribute, 
        Annotation **annotation)
{
    Assert(annotation != NULL);
    AssertNode("classModifier");

    if (TTEXT(TCHILD(node, 0)) == "annotation") {
        *annotation = (Annotation*)handleAnnotation(TCHILD(node, 0));
        attribute &= Declaration:AnnotationAttribute;
    }
    else if (TTEXT(TCHILD(node, 0)) == "public")
        attribute &= Declaration::PublicAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "private")
        attribute &= Declaration::PrivateAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "protected")
        attribute &= Declaration::ProtectedAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "static")
        attribute &= Declaration::StaticAttribute;
    else if (TTEXT(TCHILD(oode, 0)) == "abstract")
        attribute &= Declaration::AbstractAttribute;
    else if (TTEXT(TCHILD(node, 0)) == "final")
        attribute &= Declaration::FinalAttribute; 

}
/// hanlde class body declaration 
AST* ASTBuilder::handleClassBodyDeclaration(Node* node, const string& cls) 
{
    AssertNode("classBodyDeclaration");

    if (TSIZE(node) == 1)
        return NULL;
    // process the class modifier at first
    Annotaton* annotation = NULL;
    int attribute = DeclarationAttribute::InvalidAttribute;
    for (size_t i = 0; i < TSIZE(node) - 1; i++) 
        handleClassModifier(TCHILD(node, i), attribute, &annotation);
    
        
    // handle the member declaration
    Declaration* decl = 
        (Declaration*)handleClassMemberDeclaration(TSIZE(node) - 1, cls);
    if (delc) {
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
   else if (TTEXT(TCHILD(node, 0)) == "filedDeclaration")
        return handleFiledDeclaration(TCHILD(node, 0), clsName);
   else if (TTEXT(TCHILD(node, 0)) == "consructorDeclaration")
       return handleConstructorDeclaration(TCHILD(node, 0), clsName);
   else
       Error::complain(node->location, "unkown member declaraion");

   return NULL;
}
    
/// handle class method
AST* ASTBuilder::handleMethodDeclaration(Node* node, const string& clsName) 
{
    int index = 0; 
    // return type and method name
    TypeDecl* retType = NULL;
    if (node->childs[index]->assic != "void")
        retType = (TypeDecl*)handleType(node->childs[index]);
    index++; 
    string methodName = node->childs[index++]->assic;
    // method parameter list
    MethodParameterList* methodParameterList = 
        (MethodParameterList*)handleMethodParameterList(node->childs[index]);    
    
    // check to see wethe the exception is thrown
    vector<QualifedName> qualifedNameList;
    if (node->childs[index]->assic == "throw") {
        // handle qualified name list
        index++; 
        handleQualifiedNameList(node->childs[index++]);
    }
    // check the method body 
    MethodBlock* methodBlock = NULL; 
    if (node->childs[index]->assic ==  "methodBody")
        methodBlock = (MethodBlock*)handleMethodBlock(node->childs[index]);

    // create method instance 
    Method* method = new Method(retType, 
                            clsName,
                            methodName,
                            methodParameterList, 
                            methodBlock,  
                            node->location);
    // tell method wether it will throw exception 
    method->setWetherThrowException(true, qualifedNameList); 
    return method;
}

/// handle variable declaration
AST* ASTBuilder::handleFiledDeclaration(Node* node, const string& clsName) 
{
    AssertNode("fildDeclaration");

    TypeDecl* type = (TypeDecl*)handleTypeDecl(node->childs[0]);
    return handleVairalbeDeclarators(node->childs[1], type); 
}
// handle variableDeclarators
AST* ASTBuilder::handleVariableDeclarators(Node* node, TypeDecl* type)
{
    AssertNode("variableDeclarators");
    
    string varName = node->childs[0]->assic;
    int scalars = (TCHILD(node->childs[0]) - 1)/2;
    if (TSIZE(node) > 2)
        AST* initializer = handleVariableInitializer(node->childs[2]);
    return new Variable(); // todo 
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
                "unknow initializer", node->childs[0]->assic.str());
}
/// handle array initializer
AST* ASTBuilder::handleArrayInitializer(Node* node)
{
    AssertNode("arrayInitializer");

    ArrayInitializer* arrayInitializer = new ArrayInitializer(node->location); 
    for (int index = 1; index < TSIZE(node) - 1; index++) {
        AST* initializer = handleVaraibleInitializer(node->childs[index]);
        arrayInitializer->addInitializer(initializer);
        if (node->childs[index] == ",")
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
        AST* initializer = handleMapPairItemInitializer(node->childs[index]);
        mapInitializer->addInitializer(initializer);
        if (node->childs[index] == ",")
            continue;
    }
    return mapInitializer; 
}
/// handle map item pair initializer
AST* ASTBuilder::handleMapItemPairInitializer(Node* node) 
{
    AssertNode("mapItemPairInitializer");
    
    MapPairItemInitializer* mapPairItemInitialzier = 
        new MapPairItemInitializer(node->location);
    mapPairItemInitializer->m_key =
        handleVariableInitializer(node->childs[0]);
    mapPairItemInitializer->m_val =
        handleVaraibleInitializer(node->childs[2]);
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
        Declaration::Attribute &attribute,
        Annotation** annotation)
{
    AssertNode("varaibleModifier");
    Assert(annotation != NULL);

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

    Declaration::Attribute attribute = Declaration::Invalid; 
    int index = 0;
    Annotation* annotation = NULL; 
    
    while (index < (TSIZE(node) - 2)){
        if (node->childs[index]->assic != "variableModifier")
            break;
        handleVariableModifier(node->childs[index], attribute, &annotation);
    }
    // get type name and id
    TypeDecl* variableType = 
        (TypeDecl*)handleTypeDecl(node->childs[index++]);
    string variableName;
    int scalars = 0;
    handleVariableDeclaratorId( node->childs[index], variableName, scalars);
    FormalParameter* formalParameter = 
        new  FormalParameter(variableType, variablename, node->location);
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
    return stmt;
}

/// handle block statement
AST* ASTBuilder::handleBlockStatement(Node* node)
{
    AssertNode("blockStatement");
   
    if (node->childs[0]->assic == "localVariableDeclarationStatement")
        return handleLocalVariableDeclarationStatement(node->childs[0]);
    else if (node->childs[0]->assic == "statement")
        return handleStatement(node->childs[0]);
    else if (node->childs[0]->assic == "typeDeclaration")
        return handleTypeDeclaration(node->childs[0]);
    else
        Error::complain(node->location,
                "unknow statement '%s'"
                node->cilds[0]->assic.c_str());
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
        return handleForEachStatement(node->childs[0]);
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

    Declaration::Attribute attribute = Declaration::Invalid; 
    int index = 0;
    Annotation* annotation = NULL; 
    
    while (index < (TSIZE(node) - 2)){
        if (node->childs[index]->assic != "variableModifier")
            break;
        handleVariableModifier(node->childs[index], attribute, &annotation);
    }
    TypeDecl* variableType = 
        (TypeDecl*)handleTypeDecl(node->childs[index++]);
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
        return handleLocalVaraibleDeclaration(node->childs[0]);
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
    
    Variable* variable1 = NULL;
    Variable* variable2 = NULL;

    // check the foreachVarItem
    variable1 = (Variable*)handleForeachVariable(node->childs[2]);
    int index = 3;

    if (node->childs[index]->assic == ",") {
        variable2 = (Variable*)handleForeachVariable(node->childs[index]); 
        index++;
    }
    index++; // skip the 'in' keyword
    IterableObjectDecl* decl = 
        (IterableObjectDecl*)handleIterableObject(node->childs[index]); 
    Statement* stmt = (Statement*)handleStatement(node->childs[index]);  
    return new ForeachStatement(variable1, 
                variable2, decl, stmt, node->location);;
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
        return new Varaible(NULL, variableName, node->location);
    }
}

/// handle iteralbe object declaration in foreach statement
AST* ASTBuidler::handleIterableObject(Node* node)
{
    AssertNode("iterableObject");

    IterableObject* iterableObject = NULL;

    if (node->childs[0]->assic == "IDENTIFIER") {
        iterableObject =  new IterableObject(node->location);
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
    for (size_t index = 1; index < node->count() - 1; index++) {
        string op = node->childs[index++]->childs[0]->assic;
        Expr* expr = (Expr*)handleExpr(node->childs[index]);
        stmt->addElement(op, expr);
    }
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
    if (node->childs[0]->assic == "logicalOrExpr")
        return handleLogicOrExpr(node->childs[0]);
    else if (node->childs[0]->assic == "newExpr")
        return handleNewExpr(node->childs[0]);
    else
        return NULL;


#if 0
    if (node->count() == 1)
        return handleConditionalExpr(node);
    
    Expr* leftExpr = (Expr*)handleAssignableExpr(node->childs[0]);
    string op = node->childs[1]->childs[0]->assic;
    Expr* rightExpr = (Expr*)handleExpr(node->childs[2]);
    return new BinaryOpExpr(op, leftExpr, rightExpr, node->location);
#endif 
}

/// handler for assignalbe expression
AST* ASTBuilder::handleAssignableExpr(Node* node) 
{
    return NULL;
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
        return handleMapExpr(node->childs[0]);
    
    if (text == "setLiteral")
        return handleSetExpr(node->childs[0]);
    
    
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
