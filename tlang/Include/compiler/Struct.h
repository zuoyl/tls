//
//  Struct.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_STRUCT_H
#define TCC_STRUCT_H

#include "compiler/Common.h"
#include "compiler/AST.h"
#include "compiler/ASTVistor.h"
#include "compiler/Type.h"

class Struct : public AST, public Scope {
public:
    typedef pair<TypeSpec*, const string> Member;
public:
    Struct(const string &id);
    ~Struct(){}
    void walk(ASTVisitor *visitor) { visitor->accept(*this); }
    void pushMember(TypeSpec *typeSpec, const string &id);
public:
    bool m_isPublic;
    string m_name;
    vector<Member> m_members;
};

#endif // TCC_STRUCT_H