//
//  tl-ast.h
//  A toyable language compiler (like a simple c++)
//

#ifndef __TL_AST_H__
#define __TL_AST_H__


#include "tl-common.h"
#include "tl-location.h"
#include <string>
#include <vector>

namespace tlang {
    class ASTVistor;

    /// "class AST
    /// Abstract Syntax Tree
    class AST {
        public:	
            /// AST constructor
            /// construct a AST node and the parent node is specified by parameter
            /// @param parent the parent of the node, by default, no parent
            AST(){}
            AST(AST *parent):m_parent(parent){}
            AST(const Location &location):m_location(location){}
            AST(AST *parent, const Location &location):
                m_parent(parent),m_location(location){}
            
            /// AST destructor
            virtual ~AST(){}
                
            /// Walk AST node and viste the node.
            /// @param visitor vistors like TypeBuilder and IR code generator
            /// @see TypeBuilder
            /// @see IRBuilder
            virtual void walk(ASTVistor *visitor) {}
            
            
            /// Set the node's parent node
            /// @param parent the node's new parent
            void setParentNode(AST *parent) { m_parent = parent; }
            
            /// Get the node's parent
            /// @ret the node's parent
            AST* getParentNode() { return m_parent; }
            
            
            const Location& getLocation()const { return m_location; } 

            void setLocation(Location &location) { m_location = location; }
            
        protected:
            /// the parent node of the current node
            AST* m_parent;
            /// location of the AST node
            Location m_location;
    };
} // namespace tlang
#endif // __TL_AST_H__
