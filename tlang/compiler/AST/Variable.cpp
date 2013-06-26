//
//  Variable.cpp
//  A toyable language compiler (like a simple c++)

#include "Variable.h"

Variable::Variable(bool isStatic, bool isConst, TypeSpec *type, const string &id, Expr *expr, const Location &location)
    :AST(location), 
    m_isStatic(isStatic), 
    m_isConst(isConst), 
    m_typeSpec(type),
    m_name(id),
    m_expr(expr)
{
    if (!expr)
        m_isInitialized = false;
    m_isGlobal = false;
    m_isPublic = false;
    m_isOfClass = false;
}
Variable::~Variable()
{
    if (m_expr)
        delete m_expr;
    if (m_typeSpec)
        delete m_typeSpec;
}
