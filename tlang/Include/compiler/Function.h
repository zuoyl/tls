//
//  Function.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_FUNCTION_H
#define TCC_FUNCTION_H

#include "compiler/Common.h"
#include "compiler/AST.h"
#include "compiler/ASTVistor.h"
#include "compiler/Scope.h"

class FunctionParameter;
class FunctionParameterList;
class FunctionBlock;


/// 'class Function
/// Function class to manage all semantics of function, the Function are both AST node and Scope
/// @see AST
/// @see Scope
class Function : public AST , public Scope {
public:
	/// Constructor
    Function();
	
	/// Constructor
    Function(const string &signature, 
             const string &retType, 
             const string &id, 
             FunctionParameterList *list,
             FunctionBlock *block);
	
	/// Destructor
    ~Function();
	
	/// Walkhelper which access the function node
    void walk(ASTVisitor *visitor)    { visitor->accept(*this); }
	
	/// Check to see wether has parameter
	/// @ret true indicate that has paremeter else none
    bool hasParamter() { return ( m_paraList != NULL ); }
	
	/// Get parameter's count
	/// @ret the count of all parameters
    int  getParameterCount();
	
	/// Get specified parameter by index
	/// @param the parameter's index
    FunctionParameter* getParameter(int index);
    
	/// Get locals's count for the function
	/// @ret the locals' count
    int  getLocalsCount();
	
	/// Get locals' total size for the function
	/// @ret the local's total size
	int  getLocalsSize();
    
public:
	/// Wether the function is a virtual function
    bool   m_isVirtual;
	/// Wether the function is static function
    bool   m_isStatic;
	///  Wether the function is public function
    bool   m_isPublic;
	/// Wethre the function has constant attribute
    bool   m_isConst;
	/// Wether the function is member function of class
    bool   m_isOfClass;
	/// Wether the function is memeber function of interface
    bool   m_isOfProtocol;
	
	/// If the funciton is member of class, the class name
    string m_class;
	/// The interface name if the function is a member of interface
    string m_protocol;
	/// Return type's name
    string m_returnType;
	/// Function's name
    string m_name;
	/// Signature 
    string   m_signature;
	/// Parameter list
    FunctionParameterList *m_paraList;
	/// Function Block
    FunctionBlock *m_block;
};


class FunctionParameter : public AST {
public:
    FunctionParameter(bool isConst, const string &type, const string &id, bool hasDefault, Expression *deft){}
    ~FunctionParameter(){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    bool m_isConst;
    bool m_hasDefault;
    int m_index;    // indicate the parameter's index
    string m_type;
    string m_name;
    Expression *m_default;
	Function *m_function;
};

class FunctionParameterList : public AST {
public:
    FunctionParameterList(){}
    ~FunctionParameterList(){}
    void addParameter(FunctionParameter *para){
        if (para)
            m_parameters.push_back(para);
    }
    
    int  getParameterCount() {
        return (int)m_parameters.size();
    }
    FunctionParameter* getParameter(int index) {
        if (index >= 0 && index < m_parameters.size()) {
            return m_parameters.at(index);
        }
        return NULL;
    }
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    std::vector<FunctionParameter *> m_parameters;
};

class FunctionBlock : public AST {
public:
    FunctionBlock(){}
    ~FunctionBlock(){}
    void addStatement(Statement *stmt){}
    void walk(ASTVisitor *visitor){ visitor->accept(*this);}
public:
    vector<Statement *> m_stmts;
};



#endif // TCC_FUNCTION_H
