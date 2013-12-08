//
//  Stmt.h
//  A toyable language compiler (like a simple c++)
//


#ifndef __TL_AST_STMT_H__
#define __TL_AST_STMT_H__

#include "tl-ast.h"
#include "tl-ast-expr.h"
#include "tl-label.h"
#include "tl-type.h"
#include "tl-location.h"
#include "tl-scope.h"

namespace tlang {
    class ASTVistor;

    /// 'class Stmt
    /// Basic class for all Stmt
    class ASTStatement : public AST {
        public:
            /// Constructor
            ASTStatement(const Location &location):AST(location),m_isIterable(false){}
            /// Destructor
            virtual ~ASTStatement(){}
            /// Walker method
            virtual void walk(ASTVistor *vistor){ vistor->accept(*this);}
            /// Checker wether the Stmt is iterable Stmt
            virtual bool isIterable() { return false; }
            
            /// Get Iterable start point
            virtual Label& getIterableStartPoint() { 
                return m_iterableStartPoint; 
            }
            /// Get iterable end point
            virtual Label& getIterableEndPoint() { 
                return m_iterableEndPoint; 
            }
            /// Set Iteralbe start point
            virtual void setIterableStartPoint(Label &label) {
                m_iterableStartPoint = label;
            }
            /// Set Iterable end point
            virtual void setIterableEndPoint(Label &label) {
                m_iterableEndPoint = label;
            }
            
        protected:
            Label m_iterableStartPoint;
            Label m_iterableEndPoint;
            bool  m_isIterable;
    };

    /// 'class Block
    class ASTBlock : public AST {
        public:
            ASTBlock(const Location &location):AST(location){}
            ~ASTBlock() {
                vector<ASTStatement*>::iterator ite = m_stmts.begin();
                for (; ite != m_stmts.end(); ite++) 
                    delete *ite;
            }
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
            void addStatement(ASTStatement* stmt) { 
                m_stmts.push_back(stmt);
            }
            void addVariable(ASTVariable *variable) {
                m_variables.push_back(variable);
            }
        public:
            vector<ASTStatement*> m_stmts;
            vector<ASTVariable*> m_variables;
    };

    /// 'class BlockStmt
    class ASTBlockStmt : public ASTStatement, public Scope {
        public:
            ASTBlockStmt(const Location &location)
                :ASTStatement(location), Scope("block", NULL){}
            ~ASTBlockStmt(){}
            void addStmt(ASTStatement *stmt){}
            void walk(ASTVistor* vistor){ vistor->accept(*this);}
        public:
            vector<ASTVariable*> m_vars;
            vector<ASTStatement*> m_stmts;
    };

    /// 'class VariableDeclStmt
    class ASTLocalVariableDeclarationStmt : public ASTStatement {
        public:
            ASTLocalVariableDeclarationStmt(ASTVariable* var, ASTExpr *expr, const Location &location)
                :ASTStatement(location),m_var(var), m_expr(expr){}
            ~ASTLocalVariableDeclarationStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
        public:
            ASTVariable *m_var;
            ASTExpr *m_expr;
    };

    /// 'class ExprSttement
    class ASTExprStmt : public ASTStatement {
        public:
            ASTExprStmt(const Location &location):ASTStatement(location){}
            ~ASTExprStmt(){}
            void walk(ASTVistor* vistor) { vistor->accept(*this); }
            void addElement(const string& op, ASTExpr* expr) { 
                m_elements.push_back(make_pair(op, expr));
            }
        public:
            ASTExpr *m_target;
            vector<pair<string, ASTExpr* > > m_elements;
        };

    /// 'class IfStaement
    class ASTIfStmt : public ASTStatement, public Scope {
        public:
            ASTIfStmt(ASTExpr* condition, ASTStatement* stmt1, ASTStatement* stmt2, const Location &location)
                :ASTStatement(location), Scope("if Stmt", NULL),
                m_conditExpr(condition), m_ifBlockStmt(stmt1), m_elseBlockStmt(stmt2){}
            ~ASTIfStmt(){}
            void walk(ASTVistor* vistor){ vistor->accept(*this);}
        public:
            ASTExpr *m_conditExpr;
            ASTStatement *m_ifBlockStmt;
            ASTStatement *m_elseBlockStmt;
    };

    /// 'class ForStmt
    class ASTForStmt : public ASTStatement, public Scope {
        public:
            ASTForStmt(AST* initializer, ASTExpr *expr2, 
                       ASTExprList *exprList, ASTStatement *stmt, const Location &location)
                :ASTStatement(location), Scope("for Stmt", NULL),
                 m_initializer(initializer), 
                 m_conditExpr(expr2),
                 m_exprList(exprList), 
                 m_stmt(stmt){}
            ~ASTForStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
            bool isIterable() { return true; }
        public:
            AST *m_initializer;
            ASTExpr *m_conditExpr;
            ASTExprList *m_exprList;
            ASTStatement *m_stmt;
            Label m_loopLabel;
    };

    /// 'class ForEachStmt
    // 'foreach' '(' foreachVarItem (',' foreachVarItem)? 'in' (identifier|mapLiteral|setLitieral) ')' blockStmt
    class ASTForeachStmt : public ASTStatement, public Scope {
        public:
            enum { Object,  MapObject, SetObject};
        public:
            ASTForeachStmt(const Location &location)
                :ASTStatement(location), Scope("foreach Stmt", NULL),
                 m_variable1(NULL), m_variable2(NULL), m_stmt(NULL),
                 m_iterableObject(NULL){}
            ~ASTForeachStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
            bool isIterable() { return true; }
        public:
            ASTVariable *m_variable1;
            ASTVariable *m_variable2;
            ASTStatement* m_stmt;
            ASTIterableObjectDecl *m_iterableObject; 
            
            ASTTypeDecl *m_typeDecl[2];
            string m_id[2];
            int m_varNumbers;
            
            int m_objectSetType;
            string m_objectSetName;
            
            ASTExpr *m_expr;
            ASTTypeExpr* m_objectTypeExpr;
            
            Label m_loopLabel;
        };


    /// 'class WhileStmt
    class ASTWhileStmt : public ASTStatement, public Scope {
        public:
            ASTWhileStmt(ASTExpr *condit, ASTStatement *stmt, const Location &location)
                :ASTStatement(location), Scope("while Stmt", NULL),
                 m_conditExpr(condit), m_stmt(stmt){}
            ~ASTWhileStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
            bool isIterable() { return true; }
        public:
            Label m_loopLabel;
            ASTExpr *m_conditExpr;
            ASTStatement* m_stmt;
    };

    /// 'class DoStmt
    class ASTDoStmt : public ASTStatement {
        public:
            ASTDoStmt(ASTExpr *expr, ASTStatement *stmt, const Location &location)
                :ASTStatement(location), m_conditExpr(expr),m_stmt(stmt){}
            ~ASTDoStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
            bool isIterable() { return true; }
        public:
            Label m_loopLabel;
            ASTExpr *m_conditExpr;
            ASTStatement *m_stmt;    
    };

    /// 'class ReturnStmt
    class ASTReturnStmt : public ASTStatement {
        public:
            ASTReturnStmt(ASTExpr* expr, const Location& location)
                :ASTStatement(location),m_resultExpr(NULL){}
            ~ASTReturnStmt(){}
            void walk(ASTVistor* vistor){ vistor->accept(*this);}
        public:
            ASTExpr *m_resultExpr;
        };

    /// 'class AssertStmt
    class ASTAssertStmt : public ASTStatement {
        public:
            ASTAssertStmt(ASTExpr *expr, const Location &location)
                :ASTStatement(location),m_resultExpr(NULL){}
            ~ASTAssertStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
        public:
            ASTExpr *m_resultExpr;
    };

    /// 'class ContinueStmt
    class ASTContinueStmt : public ASTStatement {
        public:
            ASTContinueStmt(const Location &location)
                :ASTStatement(location){}
            ~ASTContinueStmt(){}
            void walk(ASTVistor  *vistor){ vistor->accept(*this);}
    };

    /// 'class SwitchStmt
    class ASTSwitchStmt : public ASTStatement {
        public:
            ASTSwitchStmt(ASTExpr *expr, const Location &location)
                :ASTStatement(location),m_conditExpr(NULL){}
        ~ASTSwitchStmt(){}
        void walk(ASTVistor *vistor){ vistor->accept(*this);}
        void addCaseStatement(vector<ASTExpr*> *exprList, ASTStatement *stmt){}
        void addDefaultStatement(ASTStatement *stmt){}
    public:
        vector<pair<vector<ASTExpr*>, ASTStatement* > > m_cases;
        ASTStatement *m_defaultStmt;
        ASTExpr *m_conditExpr;  
    };

    /// 'class BreakStmt
    class ASTBreakStmt : public ASTStatement {
        public:
            ASTBreakStmt(const Location &location)
                :ASTStatement(location){}
            ~ASTBreakStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
    };

    /// 'class ThrowStmt
    class ASTThrowStmt : public ASTStatement {
        public:
            ASTThrowStmt(ASTExpr*expr, const Location &location)
                :ASTStatement(location),m_resultExpr(NULL){}
            ~ASTThrowStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
        public:
            ASTExpr *m_resultExpr;
    };

    /// 'class CatchStmt
    class ASTCatchStmt : public ASTStatement {
        public:
            ASTCatchStmt(const string &type, const string &id, 
                ASTBlockStmt *stmt, const Location &location)
                :ASTStatement(location),m_block(NULL){}
            ~ASTCatchStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
        public:
            string m_type;
            string m_id;
            ASTBlockStmt *m_block;
    };

    /// 'class FinallyCatchStmt
    class ASTFinallyCatchStmt : public ASTStatement {
        public:
            ASTFinallyCatchStmt(ASTBlockStmt *stmt, const Location &location)
                :ASTStatement(location),m_block(NULL){}
            ~ASTFinallyCatchStmt(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
        public:
            ASTBlockStmt *m_block;
            
    };

    /// 'class TryStmt
    class ASTTryStmt : public ASTStatement {
        public:
            ASTTryStmt(ASTBlockStmt *block, const Location &location)
                :ASTStatement(location),m_blockStmt(NULL),m_finallyStmt(NULL){}
            ~ASTTryStmt(){}
            void addCatchPart(ASTCatchStmt *stmt){}
            void setFinallyCatchPart(ASTFinallyCatchStmt *finallyStmt){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
        public:
            ASTBlockStmt *m_blockStmt;
            vector<ASTCatchStmt*> m_catchStmts;
            ASTFinallyCatchStmt *m_finallyStmt;
    };

} // namespace tlang 
#endif // __TL_AST_STMT_H__ 
