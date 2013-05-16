//
//  AST.h
//  A toyable language compiler (like a simple c++)
//  2013/2/15  
// @author jenson.zuo@gmail.com

#ifndef TCC_AST_H
#define TCC_AST_H

#include "Common.h"
#include "Location.h"
class ASTVisitor;

/// "class AST
/// Abstract Syntax Tree
class AST 
{
public:	
	/// AST constructor
	/// construct a AST node and the parent node is specified by parameter
	/// @param parent the parent of the node, by default, no parent
    AST(){}
    AST(AST *parent):m_parent(parent){}
    AST(const Location &location):m_location(location){}
    AST(AST *parent, const Location &location):m_location(location), m_parent(parent){}
	
	/// AST destructor
    virtual ~AST();
		
	/// Walk AST node and viste the node.
	/// @param visitor vistors like TypeBuilder and IR code generator
	/// @see TypeBuilder
	/// @see IRBuilder
    virtual void walk(ASTVisitor *visitor);
	
	/// Add a new node as the child node of current node
	/// @param node the new node
    void addChildNode(AST *node);
	
	/// Get child by specified index
	/// @param index the child's index number
	/// @ret the specified node
    AST* getChildNode(int index);
	
	/// Set the node's parent node
	/// @param parent the node's new parent
    void setParentNode(AST *parent);
	
	/// Get the node's parent
	/// @ret the node's parent
    AST* getParentNode();
	
	/// Get the child's count of the current node
	/// @ret the count of childs
    int  getChildsCount();
    
    const Location& getLocation()const { return m_location; } 

    void setLocation(Location &location) { m_location = location; }
	
protected:
	/// the parent node of the current node
    AST* m_parent;
	/// vectors to hold all childs of the current node
    vector<AST *> m_childs;
    /// location of the AST node
    Location m_location;
};

#endif // TCC_AST_H
