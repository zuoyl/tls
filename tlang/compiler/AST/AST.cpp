//
//  AST.cpp
//  A toyable language compiler (like a simple c++)
//

#include "AST.h"
#include "ASTVistor.h"


/// @brief AST destructor
AST::~AST() 
{
#if 0
    vector<AST* >::iterator ite = m_childs.begin();
	while (ite != m_childs.end()) {
		if (*ite)
			delete *ite;
	}
	m_childs.clear();
#endif
}
/// @brief walker method for all node
void AST::walk(ASTVistor* vistor) 
{
    vector<AST* >::iterator ite = m_childs.begin();
    for (; ite != m_childs.end(); ite++) {
        AST* node = *ite;
        node->walk(vistor);
    }
}
/// @brief Add a child AST node
void AST::addChildNode(AST* node) 
{
    if (node)
        m_childs.push_back(node);    
}

/// @brief Get a child AST node by index
AST* AST::getChildNode(int index)
{
    if (index >= 0 && index < (int)m_childs.size())
        return  m_childs[index];
    else
        return NULL;
}

/// @brief Set parent AST node
void AST::setParentNode(AST* parent) 
{
    m_parent = parent;
}

/// @brief Get parent AST node
AST* AST::getParentNode()
{
    return m_parent;
}

/// @brief Get child's count 
int  AST::getChildsCount() 
{
    return (int)m_childs.size(); 
}
