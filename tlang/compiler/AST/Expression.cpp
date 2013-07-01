



#include "Common.h"
#include "Expression.h"


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
