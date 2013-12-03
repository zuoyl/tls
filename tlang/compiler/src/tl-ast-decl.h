//
//  Declaration.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_AST_DECL_H__
#define __TL_AST_DECL_H__ 

#include "tl-common.h"
#include "tl-ast.h"
#include "tl-scope.h"
#include "tl-ast-vistor.h"

namespace tl { 
    class Location;
    class ASTAnnotation;
    class ASTExpr;
    class QualifiedName;

    /// 'class QualifiedName
    class QualifiedName {
        public:
            typedef  vector<string>::iterator iterator;
        public:
            QualifiedName(){}
            ~QualifiedName(){}
            
            void addElement(const string &name) { m_names.push_back(name);}
            iterator begin() { return m_names.begin();}
            iterator end() { return m_names.end();}
            bool empty() { return m_names.empty();}
            size_t size() { return m_names.size(); }
            const string& operator [] (int index) { return m_names[index];}
            void getWholeName(string &name) {
                vector<string>::iterator ite = m_names.begin();
                for (; ite != m_names.end(); ite++) 
                    name += *ite;
            }

            QualifiedName& operator = (QualifiedName &rhs) {
                vector<string>::iterator ite = rhs.m_names.begin();
                for (; ite != rhs.m_names.end(); ite++)
                    m_names.push_back(*ite);
                return *this; 
            }
            bool operator == (QualifiedName &rhs) {
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
    class ASTAnnotation : public AST {
        public:
            ASTAnnotation(const Location &location):AST(location){}
            ~ASTAnnotation(){
                // the element value should be deleted
                map<string, ElementValue*>::iterator ite = m_elementPairs.begin();
                for (; ite != m_elementPairs.end(); ite++) {
                    ElementValue *elementValue = ite->second;
                    delete elementValue;
                }
            }
            void walk(ASTVistor* vistor){ vistor->accept(*this); }
        public:
            union ElementValue {
                ASTExpr* expr;
                ASTAnnotation *annotation;
            };
            map<string, ElementValue*> m_elementPairs;
            ElementValue  m_elementValue;
            QualifiedName m_qualifiedName;
        };

        /// 'class Declaration
    class ASTDeclaration : public AST {
        public:
            /// 'enum DeclarationModidifer
            enum Attribute  {
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
            ASTDeclaration(const Location &location):AST(location){
                m_attribute = InvalidAttribute;
                m_annotation = NULL;
            }
            virtual ~ASTDeclaration(){}

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
            ASTAnnotation* getAnnotation() { return m_annotation; }
            void setAnnotation(ASTAnnotation* annotation) { m_annotation = annotation;} 
        protected:
            int m_attribute;
            ASTAnnotation  *m_annotation;
    };

    /// 'class TypeDecl 
    class ASTTypeDecl : public AST {
        public:
            enum {
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
            ASTTypeDecl(int type, const string &name, const Location &location)
                :AST(location), m_type(type),m_name(name),m_isQualified(false){}
            ASTTypeDecl(int type, const QualifiedName &name, const Location &location)
                :AST(location),m_type(type),m_isQualified(true), m_qualifiedName(name){} 
            ASTTypeDecl(const Location &location):AST(location), m_isQualified(false){}
            ASTTypeDecl(int type, const Location &location)
                :AST(location),m_type(type), m_isQualified(true){} 
            ~ASTTypeDecl(){}
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
            ASTTypeDecl *m_type1;
            ASTTypeDecl *m_type2;
    };


    /// 'class PackageDeclaration
    class ASTPackageDecl : public AST {
        public:
            ASTPackageDecl(const Location &location):AST(location){}
            ~ASTPackageDecl(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            void addAnnotation(ASTAnnotation *annotation) { 
                m_annotations.push_back(annotation);
            }
        public:
            QualifiedName m_qualifiedName;
            vector<ASTAnnotation*> m_annotations;
    };


    /// 'class ImportDecl
    class ASTImportDecl:public AST {
        public:
            ASTImportDecl(const Location &location)
                :AST(location),m_isImportAll(false){}
            ~ASTImportDecl(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
        public:
            QualifiedName m_qualifiedName;
            bool m_isImportAll;
    };


    class ASTClass : public ASTDeclaration, public Scope {
        public:
            ASTClass(const string &clsName, 
                QualifiedName &baseClsName, 
                vector<QualifiedName> &abstractClsName,
                const Location &location);
            ~ASTClass();
            void walk(ASTVistor *visitor);
            void setImported(bool w) { m_isImported = w; }
            bool isImported() { return m_isImported; }
            // for class variable
            void addVariable(ASTVariable *var);
            ASTVariable* getVariable(const string & name) const;
            /// for filed access
            int getVaraibleOffset(const string &name) const;
            
            // for class method
            void addMethod(ASTMethod *method);
            ASTMethod* getMethod(const string &name) const;
            
            bool isInheritClass() { return m_baseClsName.empty(); }
            bool isImplementedAbstractClass() { return m_abstractClsList.empty(); }
            bool hasDeclarations() { return m_declarations.empty(); } 
            void addDeclaration(ASTDeclaration* decl) { m_declarations.push_back(decl); }

        public:
            bool m_isImported; 
            string m_name;
            QualifiedName m_baseClsName;
            vector<QualifiedName> m_abstractClsList; 
            vector<ASTDeclaration*> m_declarations;
    };

    /// 'class Variable
    class ASTVariable : public ASTDeclaration {
        public:
            /// Constructor
            ASTVariable(ASTTypeDecl *typeDecl, const string &id, const Location &location);
            /// Destructor
            ~ASTVariable();
            /// walkhelper method
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            
        public:
            /// Wether the vraible is been initialized
            bool m_isInitialized;
            
            /// Wether the variable is member of class
            bool m_isOfClass;
            /// Wether the variable is class' static variable
            bool m_isGlobal;
            
            /// Initialization expression if the vaible is initialized
            ASTExpr *m_expr;
            /// TypeDecl of variable
            ASTTypeDecl *m_typeDecl;
            /// Type of the variable
            Type *m_type;
            /// Name of Variable
            string m_name;
            /// Class name of variable if the variable is member of class
            string m_class;
    };

    class ASTFormalParameter;
    class ASTFromalParameterList;
    class ASTMethodBlock;
    class ASTExpr;

    /// 'class Method
    /// Method class to manage all semantics of method, the Method are both AST node and Scope
    /// @see AST
    /// @see Scope
    class ASTMethod : public ASTDeclaration , public Scope {
        public:
            /// Constructor
            ASTMethod(const Location &location);
            
            /// Constructor
            ASTMethod(ASTTypeDecl *typeDecl, 
                   const string &methodName, 
                   const string &clsName,
                   ASTFormalParameterList *list,
                   const Location &location);
            
            /// Destructor
            ~ASTMethod();
            
            /// Walkhelper which access the method node
            void walk(ASTVistor* visitor)    { visitor->accept(*this); }

            void setWetherThrowException(bool flag, 
                    vector<QualifiedName> &qualifedNameList){}
            /// Check to see wether has parameter
            /// @ret true indicate that has paremeter else none
            bool hasParamter() { return ( m_paraList != NULL ); }
            
            /// Get parameter's count
            /// @ret the count of all parameters
            int  getParameterCount();
            
            /// Get specified parameter by index
            /// @param the parameter's index
            ASTFormalParameter* getParameter(int index);
            
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
            ASTTypeDecl *m_retTypeDecl;
            /// method's name
            string m_name;
            /// Parameter list
            ASTFormalParameterList *m_paraList;
            /// Method Block
            ASTMethodBlock *m_block;
    };


    class ASTFormalParameter : public ASTDeclaration {
        public:
            ASTFormalParameter(ASTTypeDecl *variableType, 
                    const string &variableName, 
                    const Location &location)
                    :ASTDeclaration(location),
                     m_type(variableType),
                     m_name(variableName){}
            ~ASTFormalParameter(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            void setScalars(int scalars) { m_scalars = scalars; }
        public:
            ASTTypeDecl *m_type;
            string m_name;
            int m_scalars; 
            int m_index;    // indicate the parameter's index
            ASTMethod *m_method;
    };

    class ASTFormalParameterList : public AST {
        public:
            ASTFormalParameterList(const Location &location):AST(location){
                m_method = NULL; 
            }

            ~ASTFormalParameterList(){}
            void addParameter(ASTFormalParameter *parameter) {
                    m_parameters.push_back(parameter);
            }
            
            int  getParameterCount()  { 
                return (int)m_parameters.size(); 
            }
            
            ASTFormalParameter* getParameter(int index) {
                if (index >= 0 && index < (int)m_parameters.size()) {
                    return m_parameters.at(index);
                }
                return NULL;
            }
            void walk(ASTVistor *visitor) { visitor->accept(*this);} 
        public:
            vector<ASTFormalParameter*> m_parameters;
            ASTMethod* m_method;
        };

    class ASTMethodBlock : public AST {
        public:
            ASTMethodBlock(ASTBlock *block, const Location &location)
                :AST(location), m_block(block){}
            ~ASTMethodBlock(){}
            void addStatement(ASTStatement *stmt){ 
                    m_stmts.push_back(stmt);
            }
            void addVariable(ASTVariable *var) {
                    m_vars.push_back(var);
            }
            
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
        public:
            ASTBlock *m_block;
            vector<ASTStatement*> m_stmts;
            vector<ASTVariable*> m_vars;
        };

    class ASTIterableObjectDecl : public AST {
        public:
            ASTIterableObjectDecl(const Location &location):AST(location){}
            virtual ~ASTIterableObjectDecl(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
        public:
            string m_identifier;  // the identifier itself is a iterable object
    };
    /// 'class MapItemPairInitializer
    class ASTMapPairItemInitializer : public ASTIterableObjectDecl {
        public:
            ASTMapPairItemInitializer(const Location &location):ASTIterableObjectDecl(location){}
            ~ASTMapPairItemInitializer(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
        public:
            AST *m_key;
            AST *m_val;
    };

    /// 'class MapInitializer
    class ASTMapInitializer: public ASTIterableObjectDecl {
        public:
            ASTMapInitializer(const Location &location):ASTIterableObjectDecl(location){}
            ~ASTMapInitializer(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            void addInitializer(ASTMapPairItemInitializer *initializer) {
                m_values.push_back(initializer);
            }
        public:
            string m_keyTypeName;
            string m_valTypeName;
            vector<ASTMapPairItemInitializer*> m_values;
    };
    /// 'class IterableArrayObject
    class ASTArrayInitializer : public ASTIterableObjectDecl {
        public:
            ASTArrayInitializer(const Location& location):ASTIterableObjectDecl(location){}
            ~ASTArrayInitializer(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            void addInitializer(AST *initializer) {
                m_values.push_back(initializer);
            }
        public:
            string m_type;
            vector<AST*> m_values;
    };

} // namespace tl 
#endif // T__TL_AST_DECL_H__
