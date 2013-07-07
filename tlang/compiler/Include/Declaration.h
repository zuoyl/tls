//
//  Declaration.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_DECLARATION_H
#define TCC_DECLARATION_H

#include "Common.h"
#include "AST.h"
#include "ASTVistor.h"
#include "Type.h"
#include "Location.h"
#include "Scope.h"

class Annotation;


/// 'class Declaration
class Declaration : public AST
{
public:
    /// 'enum DeclarationModidifer
    enum  
    {
        InvalidAttribute = 0x0000; 
        PublicAttribute = 0x0001,
        PrivateAttribute = 0x0002, 
        ProtectedAttribute = 0x0004,
        StaticAttribute = 0x0008,
        AbstractAttribute = 0x0010,
        FinalAttribute = 0x0020,
        AnnotationAttribute = 0x0040,

        NativeAttribute = 0x1000,
        SychonizedAttribute = 0x2000,
    };
public:
    Declaration(Location& location):AST(location){
        m_attribute = InvalidAttribute;
        m_annotation = NULL;
    }
    virtual ~Declaration(){}

    void setAttribute(int attribute) { m_attribute = Attribute; }
    int  getAttribute() { return m_attribute; }
    bool isPublic() { return (m_attribute & PublicAttribute); }
    bool isPrivate() { return (m_attribute & PrivateAttribute); }
    bool isProtected() { return (m_attribute & ProtectedAttribute;) }
    bool isStatic() { return (m_attribute & StaticAttribute); }
    bool isAbstract() { return (m_attribute & AbstractAttribute); }
    bool isAnnotation() { return (m_attribute & AnnotationAttribute ); }
    bool isNative() { return (m_modfier & NativeAttribute); }
    bool isSychonzied() { return (m_attribute & SychonizedAttribute); }
    Annotation* getAnnotation() { return m_annotation; }
    void setAnnotation(Annotation* annotation) { m_annotation = annotation;} 
protected:
    int m_attribute;
    Annotation* m_annotation;
};

/// 'class TypeDecl 
class TypeDecl : public AST
{
public:
    enum 
    {
       TInvalid; 
       TBool,
       TChar,
       TByte,
       TShort,
       TInt,
       TLong,
       TFloat,
       TDouble,
       TString,
       TClass,
       TMap,
    };
public:
    TypeDecl(int type, const string& name, const Location& location):
        m_type(type),m_name(name),AST(location){}
    TypeDecl(const QualifedName& name, const string& location):AST(location){} 
    TypeDecl(const Location& location):AST(location){}
    TypeDecl(int type, const Location& location):m_type(type),AST(location){} 
    ~TypeDecl(){}
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
    void setScalars(int scalars) {
        m_isArray = (scalras > 0);
        m_scalras = scalars;
    }
public:
    int m_type;
    bool m_isArray;
    bool m_isMap;
    int  m_scalars;
    string m_name;
    QualifedName m_qualifedName;
    // if the type is map, the name of type1 and type2 
    string m_name1;
    string m_name2;
    TypeDecl* m_type1;
    TypeDecl* m_type2;
};


/// 'class PackageDecl
class PackageDecl: public AST 
{
public:
    PackageDecl(const Location& location):AST(location){}
    ~PackageDecl(){}
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
    void addAnnotation(Annotation* annotation) { 
        m_annotations.push_back(annotation);
    }
public:
    vector<string> m_qualifiedName;
    vector<Annotation*> m_annotations;
};


/// 'class ImportDecl
class ImportDecl:public AST 
{
public:
    ImportDecl(const Location& location)
        :AST(location),m_isImportAll(false){}
    ~ImportDecl(){}
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
public:
    QualifiedName m_qualifiedName;
    bool m_isImportAll;
};

/// 'class Annotation
class Annotation : public AST
{
public:
    Annotation(const Locaiton& location):AST(location){}
    ~Annotation(){}
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
public:
    union ElementValue 
    {
        Expr* expr;
        Annotation *annotation;
    };
    map<string, ElementValue *> m_elementPairs;
    ElementValue  m_elmentValue;
    QualifiedName m_qualifiedName;
};

class Class : public Declaration, public Scope {
public:
    Class(const string& clsName, 
        QualifedName &baseClsName, 
        vector<QualifedName>& abstractClsName,
        ClassBlock* block,
        const Location& location);
    ~Class();
    void walk(ASTVisitor* visitor);
    void addDeclaration(Declaration* decl);
    // for class variable
    void addVariable(Variable* var);
    Variable* getVariable(const string&  name) const;
    /// for filed access
    int getVaraibleOffset(const string& name) const;
    
    // for class method
    void addMethod(Method* func);
    Method* getMethod(const string& name) const;
    
    bool isInheritClass() { return (m_base.size() > 0); }
    bool isImplementedAbstractClass() { return (m_abstractCls.size() > 0); }
    
public:
    string m_name;
    string m_baseClsName;
    vector<QualifedName> m_abstractClsList; 
    vector<Declaration*> m_declarations;
};

class ClassBlock : public AST {
public:
    ClassBlock(const Location& location);
    ~ClassBlock();
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
    void addDeclaration(Declaration *decl); 
    void addMethod(Method* method);
    void addVariable(Variable* var);
    Variable* getVariable(const string& name);
    Method* getMethod(const string& name);
public:
    vector<Variable* > m_vars;
    vector<Method* > m_methods;
};

class FormalParameter;
class FromalParameterList;
class MethodBlock;
class TypeDecl;
class Expr;

/// 'class Method
/// Method class to manage all semantics of method, the Method are both AST node and Scope
/// @see AST
/// @see Scope
class Method : public Declaration , public Scope 
{
public:
	/// Constructor
    Method(const Location& location);
	
	/// Constructor
    Method(TypeDecl* TypeDecl, 
           const string& clsName,
           const string& id, 
           FormalParameterList* list,
           const Location& location);
	
	/// Destructor
    ~Method();
	
	/// Walkhelper which access the method node
    void walk(ASTVisitor* visitor)    { visitor->accept(*this); }

    void setWetherThrowException(bool flag, vector<QualifedName>& qualifedNameList){}
	/// Check to see wether has parameter
	/// @ret true indicate that has paremeter else none
    bool hasParamter() { return ( m_paraList != NULL ); }
	
	/// Get parameter's count
	/// @ret the count of all parameters
    int  getParameterCount();
	
	/// Get specified parameter by index
	/// @param the parameter's index
    FormalParameter* getParameter(int index);
    
	/// Get locals's count for the method
	/// @ret the locals' count
    int  getLocalsCount();
	
	/// Get locals' total size for the method
	/// @ret the local's total size
	int  getLocalsSize();
    
public:
	/// Wether the method is a virtual method
    bool   m_isVirtual;
	/// Wether the method is static method
    bool   m_isStatic;
	///  Wether the method is public method
    bool   m_isPublic;
	/// Wethre the method has constant attribute
    bool   m_isConst;
	/// Wether the method is member method of class
    bool   m_isOfClass;
	/// Wether the method is memeber method of interface
    bool   m_isOfProtocol;
    /// flag to indicate the method ast is declaration or implementation
    bool  m_isDeclaration;
	/// If the funciton is member of class, the class name
    string m_class;
	/// The interface name if the method is a member of interface
    string m_protocol;
	/// Return type's name
    TypeDecl* m_retTypeDecl;
	/// Method's name
    string m_name;
	/// Signature 
    string   m_signature;
	/// Parameter list
    FormalParameterList* m_paraList;
	/// Method Block
    MethodBlock* m_block;
};


class FormalParameter : public Declaration, public AST 
{
public:
    FormalParameter(TypeDecl* variableType, 
            const string& name, 
            const Location& location)
            :AST(location),Declaration(location),
             m_type(variableType),
            m_name(variableName){}
    ~FormalParameter(){}
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
    void setScalars(int scalars) { m_scalars = scalars; }
public:
    TypeDecl* m_type;
    string m_name;
    int m_scalars; 
    int m_index;    // indicate the parameter's index
	Method* m_method;
};

class FormalParameterList : public AST 
{
public:
    FormalParameterList(const Location& location):AST(location){}
    ~FormalParameterList(){}
    void addParameter(FormalParameter* parameter) {
            m_parameters.push_back(para);
    }
    
    int  getParameterCount()  { 
        return (int)m_parameters.size(); 
    }
    
    FormalParameter* getParameter(int index) {
        if (index >= 0 && index < m_parameters.size()) {
            return m_parameters.at(index);
        }
        return NULL;
    }
    void walk(ASTVisitor* visitor) { visitor->accept(*this);} 
public:
    vector<FormalParameter*> m_parameters;
    Method* m_method;
};

class MethodBlock : public AST 
{
public:
    MethodBlock(const Location& location):AST(location){}
    ~MethodBlock(){}
    void addStatement(Statement* stmt){ 
        if (stmt)
            m_stmts.push_back(stmt);
    }
    void addVariable(Variable* var) {
        if (var)
            m_vars.push_back(var);
    }
    
    void walk(ASTVisitor* visitor){ visitor->accept(*this);}
public:
    vector<Statement* > m_stmts;
    vector<Variable* > m_vars;
};

class ArgumentList : public Expr 
{
public:
    ArgumentList(const Location& location):Expr(location){}
    ~ArgumentList(){}
    void walk(ASTVisitor* visitor) { visitor->accept(*this); }
    void appendArgument(Expr* expr) { m_arguments.push_back(expr); }
    Type* getType() { return NULL; }
    bool isConstant() { return false; }
public:
    vector<Expr* > m_arguments;

};

class MethodCallExpr : public Expr 
{
public:
	MethodCallExpr(const Location& location):Expr(location){}
    MethodCallExpr(const string& methodName, const Location& location)
        :Expr(location), m_methodName(methodName){}
    ~MethodCallExpr() {
        vector<Expr* >::iterator ite;
        for (ite = m_arguments.begin(); ite != m_arguments.end(); ite++)
            delete* ite;
    }
    void walk(ASTVisitor* visitor) { 
        visitor->accept(*this);
    }
    Type* getType() { return NULL; }
    bool isConstant() { return false; }
    void setMethodName(const string& name) {
        m_methodName = name;
    }
    string& getMethodName() { return m_methodName; }
    
    void appendArgument(Expr* expr) {
        if (expr)
            m_arguments.push_back(expr);
    }
    int getArgumentCount() {
        return (int)m_arguments.size();
    }
    Expr* getArgumentExpr(int index) {
        if (index >= 0 && index < (int)m_arguments.size())
            return m_arguments[index];
        else
            return NULL;
    }
public:
    string m_methodName;
    vector<Expr* > m_arguments;    
};
#endif // TCC_DECLARATION_H
