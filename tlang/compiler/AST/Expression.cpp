
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

bool UnaryExpr::isConstant()
{
    if (m_primary && !m_primary->isConstant())
        return false;
    vector< SelectorExpr * >::iterator ite = m_selectors.begin();
    for (; ite != m_selectors.end(); ite++) {
        Expr* expr = *ite;
        if (expr && !expr->isConstant())
            return false;
    }
    return true;
}

bool SelectorExpr::isConstant()
{
    return false;// temp
}

bool PrimaryExpr::isConstant()
{
    bool result = false;
    switch (m_type) {
        case T_NULL:
        case T_TRUE:
        case T_FALSE:
        case T_NUMBER:
        case T_HEX_NUMBER:
        case T_STRING:
            result = true;
            break;
        case T_COMPOUND:
            if (m_expr)
                result = m_expr->isConstant();
            break;
        case T_MAP:
        case T_LIST:
        case T_IDENTIIFER:
            // these type should be judged in detail 
            break;
        defaut:
            result = false;
            break;
    }
    return result;
}


bool MapExpr::isConstant()
{
    vector<MapItemExpr *>::iterator ite = m_items.begin();
    for (; ite != m_items.end(); ite++) {
        Expr* expr = *ite;
        if (expr && !expr->isConstant())
            return false;
    }
}

bool MapItemExpr::isConstant()
{
    if (m_key && !m_key->isConstant())
        return false;
    if (m_val && !m_val->isConstant())
        return false;
    return true;
}

bool SetExpr::isConstant()
{
    if (m_exprList && !m_exprList->isConstant())
        return false;
    return true;
}




