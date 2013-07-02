
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

/// unaryExpr 
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
Type* UnaryExpr::getType()
{
    Type* primaryType = NULL;
    Type* resultType = NULL;

    if (m_primary)
        primaryType = m_primary->getType();
    resultType = primaryType; 
    vector<SelectorExpr *>::iterator ite = m_selectors.begin();
    for (; ite != m_selectors.end(); ite++) {
        SelectorExpr* selector = *ite;
        switch (selector->m_type) {
            case DOT_SELECTOR:
                // if it's dot selector, 
                // the result type should be type of identifer
                break;
            case ARRAY_SELECTOR:
                // if it's array selector, 
                // the result type should be type of element of set
                break;
            case METHOD_SELECTOR:
                // the result type should be return type of method 
                break;
            
            default:
                break;
        }
    }
    return resultType;
}
/// selectorExpr
bool SelectorExpr::isConstant()
{
    return false;
}
Type* SelectorExpr::getType()
{
    switch (m_type) {
        case DOT_SELECTOR:
            // if it's dot selector, 
            // the result type should be type of identifer
            break;
        case ARRAY_SELECTOR:
            // if it's array selector, 
            // the result type should be type of element of set
            break;
        case METHOD_SELECTOR:
            // the result type should be return type of method 
            break;
        default:
            break;
    }
    return NULL;// temp, how to deal with it
}

/// primaryExpr
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
            result = false; 
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
    return true;
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




