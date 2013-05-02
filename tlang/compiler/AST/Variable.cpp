//
//  Variable.cpp
//  A toyable language compiler (like a simple c++)

#include "compiler/Variable.h"

Variable::Variable(bool isStatic, bool isConst, const string &type, const string &id, Expr *expr)
    :m_isStatic(isStatic), m_isConst(isConst), m_type(type), m_expr(expr)
{
    if (!expr)
        m_isInitialized = false;
    m_isGlobal = false;
    m_isPublic = false;
    m_isOfClass = false;
}
Variable::~Variable()
{
    if (expr)
		delete expr;
}
