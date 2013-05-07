//
//  Struct.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_STRUCT_H
#define TCC_STRUCT_H

#include "Common.h"
#include "AST.h"
#include "ASTVistor.h"
#include "Type.h"
#include "Scope.h"

class Struct : public AST, public Scope {
public:
    bool m_isPublic;
    string m_name;
    map<string, TypeSpec *> m_members;
public:
    Struct(const string &id) { m_name = id; }
    ~Struct(){}
    void walk(ASTVisitor *visitor) { visitor->accept(*this); }
    void pushMember(TypeSpec *typeSpec, const string &id) 
    { 
        if (m_members.find(id) != m_members.end())
            m_members[id] = typeSpec;
    }
};

#endif // TCC_STRUCT_H
