
//
// Expression.cpp 
//  A toyable language compiler (like a simple c++)
//

#include "Common.h"
#include "Expression.h"



bool ExprList::isConstant()
{
    vector< Expr* >::iterator ite = m_exprs.begin();
    for (; ite != m_exprs.end(); ite++) {
        Expr* expr = *ite;
        if (expr && !expr->isConstant())
            return false;
    }
    return true;
}

bool ComparisonExpr::isConstant() 
{
    // check all elements wether it is constant 
    if (m_target && !m_target->isConstant())
        return false;
    vector<pair<string, Expr*> >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        pair<string, Expr *>& item = *ite;
        Expr* expr = item.second;
        if (expr && !expr->isConstant())
            return false;
    }
    return true; 
}

bool LogicOrExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}


bool LogicAndExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}
bool BitwiseOrExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}
bool BitwiseAndExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}
bool EqualityExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}
bool RelationalExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}
bool ShiftExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}
bool AdditiveExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool MultiplicativeExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    vector< Expr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        Expr* element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}


