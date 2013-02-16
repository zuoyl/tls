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
    typedef std::pair<string, string> Member;

public:
    Struct(const string &id, const vector<Member> members): AST(NULL), Scope(id, NULL) {}
    ~Struct(){}
    void walk(ASTVisitor *visitor) { visitor->accept(*this); }
public:
    bool m_isPublic;
    string m_name;
    std::vector<Member> m_members;
};

#endif // TCC_STRUCT_H
