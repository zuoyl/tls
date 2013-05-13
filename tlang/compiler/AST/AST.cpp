//
//  AST.cpp
//  A toyable language compiler (like a simple c++)
//

#include "AST.h"
#include "ASTVistor.h"

/// @biref AST constructor
AST::AST(AST *parent)
{
	m_parent = parent;
}

/// @brief AST destructor
AST::~AST() 
{
	vector<AST *>::iterator ite = m_childs.begin();
	while (ite != m_childs.end()) {
		if (*ite)
			delete *ite;
	}
	m_childs.clear();
}

/// @brief walker method for all node
void AST::walk(ASTVisitor *visitor) 
{
    // do nothing
}

/// @brief Add a child AST node
void AST::addChildNode(AST *node) 
{
    m_childs.push_back(node);    
}

/// @brief Get a child AST node by index
AST* AST::getChildNode(int index)
{
    AST *child = NULL;    
    if (index >= 0 && index < m_childs.size())
        child = m_childs.at(index);
    
    return child;
}

/// @brief Set parent AST node
void AST::setParentNode(AST *parent) 
{
    m_parent = parent;
}

/// @brief Get parent AST node
AST* AST::getParentNode()
{
    return m_parent;
}

/// @brief Get child's count 
int  AST::getChildsCount() {
    return (int)m_childs.size(); 
}
