//
//  Struct.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_STRUCT_H
#define TCC_STRUCT_H

#include "Common.h"
#include "Runtime/Type.h"
#include "AST/AST.h"
#include "AST/ASTVistor.h"

class Struct : public AST, public Scope {
public:
    Struct(const string &id);
    ~Struct(){}
    void walk(ASTVisitor *visitor) { visitor->accept(*this); }
    void pushMember(Expression *type, const string &id);
public:
    bool m_isPublic;
    string m_name;
    std::vector<Expression* type, const string> m_members;
};

#endif // TCC_STRUCT_H
