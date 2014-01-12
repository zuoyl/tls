
//
// Expression.cpp 
//  A toyable language compiler (like a simple c++)
//

#include "tl-common.h"
#include "tl-ast-expr.h"

using namespace tlang;

bool ASTExprList::isConstant()
{
    std::vector<ASTExpr*>::iterator ite = m_exprs.begin();
    for (; ite != m_exprs.end(); ite++) {
        ASTExpr *expr = *ite;
        if (expr && !expr->isConstant())
            return false;
    }
    return true;
}

bool ASTComparisonExpr::isConstant() 
{
    // check all elements wether it is constant 
    if (m_target && !m_target->isConstant())
        return false;
    std::vector<std::pair<std::string, ASTExpr*> >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        pair<std::string, ASTExpr *>& item = *ite;
        ASTExpr *expr = item.second;
        if (expr && !expr->isConstant())
            return false;
    }
    return true; 
}

bool ASTLogicOrExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector<ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}


bool ASTLogicAndExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector<ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTBitwiseOrExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector< ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTBitwiseAndExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector< ASTExpr* >::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTEqualityExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector< ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTRelationalExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector< ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTShiftExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector<ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTAdditiveExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector<ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

bool ASTMultiplicativeExpr::isConstant()
{
    if (m_target && !m_target->isConstant())
        return false;
    std::vector<ASTExpr*>::iterator ite = m_elements.begin();
    for (; ite != m_elements.end(); ite++) {
        ASTExpr *element = *ite;
        if (element && !element->isConstant())
            return false;
    }
    return true;
}

/// unaryExpr 
bool ASTUnaryExpr::isConstant()
{
    if (m_primary && !m_primary->isConstant())
        return false;
    std::vector<ASTSelectorExpr*>::iterator ite = m_selectors.begin();
    for (; ite != m_selectors.end(); ite++) {
        ASTExpr *expr = *ite;
        if (expr && !expr->isConstant())
            return false;
    }
    return true;
}

Type* ASTUnaryExpr::getType()
{
    Type *primaryType = NULL;
    Type *resultType = NULL;

    if (m_primary)
        primaryType = m_primary->getType();

    resultType = primaryType; 
    std::vector<ASTSelectorExpr *>::iterator ite = m_selectors.begin();
    for (; ite != m_selectors.end(); ite++) {
        ASTSelectorExpr *selector = *ite;
        switch (selector->m_type) {
            case ASTSelectorExpr::DOT_SELECTOR:
                // if it's dot selector, 
                // the result type should be type of identifer
                break;
            case ASTSelectorExpr::ARRAY_SELECTOR:
                // if it's array selector, 
                // the result type should be type of element of set
                break;
            case ASTSelectorExpr::METHOD_SELECTOR:
                // the result type should be return type of method 
                break;
            
            default:
                break;
        }
    }
    return resultType;
}

/// selectorExpr
bool ASTSelectorExpr::isConstant()
{
    return false;
}

Type* ASTSelectorExpr::getType()
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
bool ASTPrimaryExpr::isConstant()
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
        case T_IDENTIFIER:
            // these type should be judged in detail 
            result = false; 
            break;
        default:
            result = false;
            break;
    }
    return result;
}

Type* ASTPrimaryExpr::getType()
{
    if (m_resultType)
        return m_resultType;
    switch (m_type) {
        case T_NULL:
            m_resultType = new IntType(); // temp
            break;
        case T_TRUE:
        case T_FALSE:
            m_resultType = new BoolType();
            break;
        case T_NUMBER:
        case T_HEX_NUMBER:
            m_resultType = new IntType();
            break;
        case T_STRING:
            m_resultType = new StringType(); 
            break;
        case T_COMPOUND:
            if (m_expr)
                return m_expr->getType();
            break;
        case T_MAP:
            m_resultType = new MapType();
            break;
        case T_LIST:
            m_resultType = new SetType();
            break;
        case T_IDENTIFIER:
            m_resultType = new ClassType(); 
            break;
        default: 
            m_resultType = NULL;
            break;
    }
    return m_resultType;
}

bool ASTMapExpr::isConstant()
{
    std::vector<ASTMapItemExpr *>::iterator ite = m_items.begin();
    for (; ite != m_items.end(); ite++) {
        ASTExpr *expr = *ite;
        if (expr && !expr->isConstant())
            return false;
    }
    return true;
}

bool ASTMapItemExpr::isConstant()
{
    if (m_key && !m_key->isConstant())
        return false;
    if (m_val && !m_val->isConstant())
        return false;
    return true;
}

bool ASTSetExpr::isConstant()
{
    if (m_exprList && !m_exprList->isConstant())
        return false;
    return true;
}
