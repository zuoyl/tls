//
//  Declaration.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_DECLARATION_H
#define TCC_DECLARATION_H

#include "Common.h"
#include "AST.h"
#include "Scope.h"
#include "ASTVistor.h"
class Location;
class Annotation;
class QualifiedName;
class Expr;

/// 'class QualifiedName
class QualifiedName
{
public:
    typedef  vector<string>::iterator iterator;
public:
    QualifiedName(){}
    ~QualifiedName(){}
    
    void addElement(const string& name) { m_names.push_back(name);}
    iterator begin() { return m_names.begin();}
    iterator end() { return m_names.end();}
    bool empty() { return m_names.empty();}
    size_t size() { return m_names.size(); }
    const string& operator [] (int index) { return m_names[index];}
    void getWholeName(string& name) {
        vector<string>::iterator ite = m_names.begin();
        for (; ite != m_names.end(); ite++) 
            name+= *ite;
    }
    QualifiedName& operator = (QualifiedName& rhs) {
        vector<string>::iterator ite = rhs.m_names.begin();
        for (; ite != rhs.m_names.end(); ite++)
            m_names.push_back(*ite);
        return *this; 
    }
    bool operator == (QualifiedName& rhs) {
       if (rhs.m_names.size() != m_names.size())
           return false;
       for (size_t index = 0; index < m_names.size(); index++)
           if (m_names[index] != rhs.m_names[index])
               return false;
       return true;
    }

private:
    vector<string> m_names;
};

/// 'class Annotation
class Annotation : public AST
{
public:
    Annotation(const Location& location):AST(location){}
    ~Annotation(){
        // the element value should be deleted
        map<string, ElementValue*>::iterator ite = m_elementPairs.begin();
        for (; ite != m_elementPairs.end(); ite++) {
            ElementValue* elementValue = ite->second;
            delete elementValue;
        }
    }
    void walk(ASTVistor* vistor){ vistor->accept(*this); }
public:
    union ElementValue 
    {
        Expr* expr;
        Annotation *annotation;
    };
    map<string, ElementValue *> m_elementPairs;
    ElementValue  m_elementValue;
    QualifiedName m_qualifiedName;
};

/// 'class Declaration
class Declaration : public AST
{
public:
    /// 'enum DeclarationModidifer
    enum Attribute  
    {
        InvalidAttribute = 0x0000, 
        PublicAttribute = 0x0001,
        PrivateAttribute = 0x0002, 
        ProtectedAttribute = 0x0004,
        StaticAttribute = 0x0008,
        AbstractAttribute = 0x0010,
        FinalAttribute = 0x0020,
        AnnotationAttribute = 0x0040,
        ConstAttribute = 0x0080,
        NativeAttribute = 0x1000,
        SychronizedAttribute = 0x2000,
    };
public:
    Declaration(const Location& location):AST(location){
        m_attribute = InvalidAttribute;
        m_annotation = NULL;
    }
    virtual ~Declaration(){}

    void setAttribute(int attribute) { m_attribute = attribute; }
    int  getAttribute() { return m_attribute; }
    bool isPublic() { 
        return (m_attribute & PublicAttribute) == PublicAttribute; 
    }
    bool isPrivate() { 
        return (m_attribute & PrivateAttribute) == PrivateAttribute; 
    }
    bool isProtected() { 
        return (m_attribute & ProtectedAttribute) == ProtectedAttribute;
    }
    bool isStatic() { 
        return (m_attribute & StaticAttribute) == StaticAttribute; 
    }
    bool isAbstract() { 
        return (m_attribute & AbstractAttribute) == AbstractAttribute; 
    }
    
    bool isConst() {
        return (m_attribute & ConstAttribute) == ConstAttribute;
    }
    bool isAnnotation() { 
        return (m_attribute & AnnotationAttribute ) == AnnotationAttribute;
    }
    bool isFinal() { return (m_attribute & FinalAttribute) == FinalAttribute; } 
     
    bool isNative() { 
        return (m_attribute & NativeAttribute) == NativeAttribute; 
    }
    bool isSychronzied() { 
        return (m_attribute & SychronizedAttribute) == SychronizedAttribute; 
    }
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
       TInvalid, 
       TId,
       TBool,
       TChar,
       TByte,
       TShort,
       TInt,
       TLong,
       TFloat,
       TDouble,
       TString,
       TArray,
       TClass,
       TMap,
       TVoid,
    };
public:
    TypeDecl(int type, const string& name, const Location& location)
        :AST(location), m_type(type),m_name(name),m_isQualified(false){}
    TypeDecl(int type, const QualifiedName& name, const Location& location)
        :AST(location),m_type(type),m_isQualified(true), m_qualifiedName(name){} 
    TypeDecl(const Location& location):AST(location), m_isQualified(false){}
    TypeDecl(int type, const Location& location)
        :AST(location),m_type(type), m_isQualified(true){} 
    ~TypeDecl(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    void setScalars(int scalars) {
        m_isArray = (scalars > 0);
        m_scalars = scalars;
    }
public:
    int m_type;
    bool m_isArray;
    bool m_isMap;
    int  m_scalars;
    string m_name;
    bool m_isQualified;
    QualifiedName m_qualifiedName;
    // if the type is map, the name of type1 and type2 
    string m_name1;
    string m_name2;
    TypeDecl* m_type1;
    TypeDecl* m_type2;
};


/// 'class PackageDeclaration
class PackageDeclaration: public AST 
{
public:
    PackageDeclaration(const Location& location):AST(location){}
    ~PackageDeclaration(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    void addAnnotation(Annotation* annotation) { 
        m_annotations.push_back(annotation);
    }
public:
    QualifiedName m_qualifiedName;
    vector<Annotation*> m_annotations;
};


/// 'class ImportDecl
class ImportDeclaration:public AST 
{
public:
    ImportDeclaration(const Location& location)
        :AST(location),m_isImportAll(false){}
    ~ImportDeclaration(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
public:
    QualifiedName m_qualifiedName;
    bool m_isImportAll;
};


class Class : public Declaration, public Scope {
public:
    Class(const string& clsName, 
        QualifiedName& baseClsName, 
        vector<QualifiedName>& abstractClsName,
        const Location& location);
    ~Class();
    void walk(ASTVistor* visitor);
    void setImported(bool w) { m_isImported = w; }
    bool isImported() { return m_isImported; }
    // for class variable
    void addVariable(Variable* var);
    Variable* getVariable(const string&  name) const;
    /// for filed access
    int getVaraibleOffset(const string& name) const;
    
    // for class method
    void addMethod(Method* func);
    Method* getMethod(const string& name) const;
    
    bool isInheritClass() { return m_baseClsName.empty(); }
    bool isImplementedAbstractClass() { return m_abstractClsList.empty(); }
    bool hasDeclarations() { return m_declarations.empty(); } 
    void addDeclaration(Declaration* decl) { m_declarations.push_back(decl); }

public:
    bool m_isImported; 
    string m_name;
    QualifiedName m_baseClsName;
    vector<QualifiedName> m_abstractClsList; 
    vector<Declaration*> m_declarations;
};

/// 'class Variable
class Variable : public Declaration 
{
public:
	/// Constructor
    Variable(TypeDecl* typeDecl, const string& id, const Location& location);
	/// Destructor
    ~Variable();
	/// walkhelper method
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    
public:
	/// Wether the vraible is been initialized
    bool m_isInitialized;
	
    /// Wether the variable is member of class
    bool m_isOfClass;
	/// Wether the variable is class' static variable
    bool m_isGlobal;
    
	/// Initialization expression if the vaible is initialized
    Expr* m_expr;
	/// TypeDecl of variable
    TypeDecl*  m_typeDecl;
    /// Type of the variable
    Type* m_type;
    /// Name of Variable
    string m_name;
	/// Class name of variable if the variable is member of class
    string m_class;
};

class FormalParameter;
class FromalParameterList;
class MethodBlock;
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
    Method(TypeDecl* typeDecl, 
           const string& methodName, 
           const string& clsName,
           FormalParameterList* list,
           const Location& location);
	
	/// Destructor
    ~Method();
	
	/// Walkhelper which access the method node
    void walk(ASTVistor* visitor)    { visitor->accept(*this); }

    void setWetherThrowException(bool flag, 
            vector<QualifiedName>& qualifedNameList){}
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
    // class name 
    string m_class;
    // return type of method 
    TypeDecl* m_retTypeDecl;
	/// method's name
    string m_name;
	/// Parameter list
    FormalParameterList* m_paraList;
	/// Method Block
    MethodBlock* m_block;
};


class FormalParameter : public Declaration 
{
public:
    FormalParameter(TypeDecl* variableType, 
            const string& variableName, 
            const Location& location)
            :Declaration(location),
             m_type(variableType),
            m_name(variableName){}
    ~FormalParameter(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
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
    FormalParameterList(const Location& location):AST(location){
        m_method = NULL; 
    }

    ~FormalParameterList(){}
    void addParameter(FormalParameter* parameter) {
            m_parameters.push_back(parameter);
    }
    
    int  getParameterCount()  { 
        return (int)m_parameters.size(); 
    }
    
    FormalParameter* getParameter(int index) {
        if (index >= 0 && index < (int)m_parameters.size()) {
            return m_parameters.at(index);
        }
        return NULL;
    }
    void walk(ASTVistor* visitor) { visitor->accept(*this);} 
public:
    vector<FormalParameter*> m_parameters;
    Method* m_method;
};

class MethodBlock : public AST 
{
public:
    MethodBlock(Block* block, const Location& location)
        :AST(location), m_block(block){}
    ~MethodBlock(){}
    void addStatement(Statement* stmt){ 
        if (stmt)
            m_stmts.push_back(stmt);
    }
    void addVariable(Variable* var) {
        if (var)
            m_vars.push_back(var);
    }
    
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
public:
    Block* m_block;
    vector<Statement* > m_stmts;
    vector<Variable* > m_vars;
};

class IterableObjectDecl : public AST
{
public:
    IterableObjectDecl(const Location& location):AST(location){}
    virtual ~IterableObjectDecl(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
public:
    string m_identifier;  // the identifier itself is a iterable object
};
/// 'class MapItemPairInitializer
class MapPairItemInitializer : public IterableObjectDecl
{
public:
    MapPairItemInitializer(const Location& location):IterableObjectDecl(location){}
    ~MapPairItemInitializer(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
public:
    AST* m_key;
    AST* m_val;
};
/// 'class MapInitializer
class MapInitializer: public IterableObjectDecl
{
public:
    MapInitializer(const Location& location):IterableObjectDecl(location){}
    ~MapInitializer(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    void addInitializer(MapPairItemInitializer* initializer) {
        m_values.push_back(initializer);
    }
public:
    string m_keyTypeName;
    string m_valTypeName;
    vector<MapPairItemInitializer*> m_values;
};
/// 'class IterableArrayObject
class ArrayInitializer : public IterableObjectDecl
{
public:
    ArrayInitializer(const Location& location):IterableObjectDecl(location){}
    ~ArrayInitializer(){}
    void walk(ASTVistor* visitor){ visitor->accept(*this);}
    void addInitializer(AST* initializer) {
        m_values.push_back(initializer);
    }
public:
    string m_type;
    vector<AST*> m_values;
};


#endif // TCC_DECLARATION_H
