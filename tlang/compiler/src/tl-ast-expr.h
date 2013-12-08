//
//  Expr.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_AST_EXPR_H__
#define __TL_AST_EXPR_H__ 

#include "tl-common.h"
#include "tl-ast.h"
#include "tl-ast-vistor.h"
#include "tl-value.h"
#include "tl-label.h"
#include "tl-type-builtin.h"

namespace tlang {
    class Value;
    class ASTVistor;

    /// 'class Expr
    ///  Basic class for all sub expression
    class ASTExpr : public AST {
        public:    
            /// constructor
            ASTExpr(const Location &location):AST(location){}
            
            /// destructor
            virtual ~ASTExpr(){}
            
            /// walker method
            virtual void walk(ASTVistor *visitor) { 
                visitor->accept(*this); 
            }
            
            /// get the type of expression
            virtual Type* getType() = 0;
            
            /// check wether the expression is const 
            virtual bool isConstant() = 0;

            /// checke to see wether the expression has valid result
            virtual bool hasValidValue() { return false; } 
            
                /// get the result for expression evaluaton
            virtual void getValue(Value &val) { val = m_value; }
        public:
            Value m_value; // temp
    };

    class ASTExprList : public AST {
        public:
            ASTExprList(const Location &location):AST(location){}
            ~ASTExprList(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return NULL; }
            bool isConstant(); 
            void appendExpr(ASTExpr *expr){}
        public:
            vector<ASTExpr*> m_exprs;
    };

    class ASTAssignmentExpr : public ASTExpr {
        public:
            enum {BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_LSHIFT, BOP_RSHIFT};
            
        public:
            ASTAssignmentExpr(const string &op, ASTExpr *left, ASTExpr *right, const Location &location)
                :ASTExpr(location), m_opname(op), m_left(left), m_right(right){}
            ~ASTAssignmentExpr(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return NULL; }
            bool isConstant() { return false; } 
        public:
            string m_opname; 
            int m_op;
            ASTExpr *m_left;
            ASTExpr *m_right;
    };

    class ASTComparisonExpr : public ASTExpr {
        public:
            enum {BOP_ADD, BOP_SUB, BOP_MUL, BOP_DIV, BOP_LSHIFT, BOP_RSHIFT};
        public:
            ASTComparisonExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location), m_target(target){}
            ~ASTComparisonExpr(){}
            void appendElement(const string &op, ASTExpr *expr){ 
                require(!op.empty());
                require(expr != NULL);
                m_elements.push_back(make_pair(op, expr));
            }
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_boolType; }
            bool isConstant(); 
        public:
            ASTExpr *m_target;
            vector<pair<string, ASTExpr*> > m_elements;
        private:
            BoolType m_boolType;
    };

    class ASTLogicOrExpr : public ASTExpr {
        public:
            ASTLogicOrExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target) {}
            ~ASTLogicOrExpr(){}
            void appendElement(ASTExpr *expr) { 
                require(expr != NULL);
                m_elements.push_back(expr); 
            }
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_boolType; }
            bool isConstant(); 

        public:
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
        private:
            BoolType m_boolType;
    };

    class ASTLogicAndExpr : public ASTExpr {
    public:
        ASTLogicAndExpr(ASTExpr *target, const Location &location)
            :ASTExpr(location), m_target(target){}
        ~ASTLogicAndExpr(){}
        void appendElement(ASTExpr *expr){}
        void walk(ASTVistor *visitor){ visitor->accept(*this);}
        Type* getType() { return &m_boolType; }
        bool isConstant(); 
    public:
        ASTExpr *m_target;
        vector<ASTExpr*> m_elements;
    private:
        BoolType m_boolType;
    };


    class ASTBitwiseOrExpr : public ASTExpr {
        public:
            ASTBitwiseOrExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTBitwiseOrExpr(){}
            void appendElement(ASTExpr *expr){}
            void walk(ASTVistor*visitor){ visitor->accept(*this);}
            Type* getType() { return &m_intType; }
            bool isConstant(); 
        public:
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
        private:
            IntType m_intType;
    };

    class ASTBitwiseXorExpr : public ASTExpr {
        public:
            ASTBitwiseXorExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTBitwiseXorExpr(){}
            void appendElement(ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
        public:
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
            Type* getType() { return &m_intType; }
            bool isConstant(); 
        private:
            IntType m_intType;
    };

    class ASTBitwiseAndExpr : public ASTExpr {
        public:
            ASTBitwiseAndExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTBitwiseAndExpr(){}
            void appendElement(ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_intType; }
            bool isConstant(); 
        public:
            ASTExpr* m_target;
            vector<ASTExpr*> m_elements;
        private:
            IntType m_intType;
    };

    class ASTEqualityExpr : public ASTExpr {
        public:
            enum { OP_EQ, OP_NEQ };
        public:
            ASTEqualityExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTEqualityExpr(){}
            void appendElement(int op, ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_boolType; }
            bool isConstant(); 
        public:
            int m_op;
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
        private:
            BoolType m_boolType;
    };

    class ASTRelationalExpr : public ASTExpr {
        public:
            enum { OP_GT, OP_LT, OP_GTEQ, OP_LTEQ };
            
            ASTRelationalExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTRelationalExpr(){}
            void appendElement(int op, ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_boolType; }
            bool isConstant(); 
        public:
            int m_op;
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
        private:
            BoolType m_boolType;
    };

    class ASTShiftExpr : public ASTExpr {
        public:
            enum { OP_LSHIFT, OP_RSHIFT };
        public:
            ASTShiftExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTShiftExpr(){}
            void appendElement(int op, ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_intType; }
            bool isConstant(); 
        public:
            int m_op;
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
        private:
            IntType m_intType;
    };


    class ASTAdditiveExpr : public ASTExpr {
        public:
            enum { OP_ADD, OP_SUB };
        public:
            ASTAdditiveExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ~ASTAdditiveExpr(){}
            void appendElement(int op, ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_intType; }
            bool isConstant(); 
        public:
            int m_op;
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;
        private:
            IntType m_intType;
    };

    class ASTMultiplicativeExpr : public ASTExpr {
        public:
            enum { OP_MUL, OP_DIV, OP_MODULO };
        public:
            ASTMultiplicativeExpr(ASTExpr *target, const Location &location)
                :ASTExpr(location),m_target(target){}
            ASTMultiplicativeExpr(const Location &location)
                :ASTExpr(location){}
            void appendElement(int op, ASTExpr *expr){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return &m_intType; }
            bool isConstant(); 
        public:
            int m_op;
            ASTExpr *m_target;
            vector<ASTExpr*> m_elements;    
        private:
            IntType m_intType;
        };

    class ASTSelectorExpr;
    class ASTPrimaryExpr;
    class ASTUnaryExpr : public ASTExpr {
        public:
            ASTUnaryExpr(ASTPrimaryExpr *target, const Location &location)
                :ASTExpr(location), m_primary(target){}
            ~ASTUnaryExpr(){}
            void appendElement(ASTSelectorExpr *expr){ m_selectors.push_back(expr); }
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType(); 
            bool isConstant(); 
        public:
            ASTPrimaryExpr *m_primary;
            vector<ASTSelectorExpr*> m_selectors;
            
    };

    class ASTSelectorExpr : public ASTExpr {
        public:
            enum { DOT_SELECTOR, ARRAY_SELECTOR, METHOD_SELECTOR};
            
            ASTSelectorExpr(int type, const string &id, const Location &location)
                :ASTExpr(location),m_type(type), m_id(id){}
            ASTSelectorExpr(int type, ASTExpr *expr, const Location &location)
                :ASTExpr(location),m_type(type) {
                if (type == ARRAY_SELECTOR)
                    m_arrayExpr = expr;
                else
                    m_methodCallExpr = (ASTMethodCallExpr*)expr;
            }
            ASTSelectorExpr(const Location& location):ASTExpr(location){}
            ~ASTSelectorExpr(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType(); 
            bool isConstant(); 
        public:
            int m_type;
            string m_id; // for .identifier
            ASTExpr *m_arrayExpr;
            ASTMethodCallExpr *m_methodCallExpr;
    };

    class ASTPrimaryExpr : public ASTExpr {
        public:
            enum {
                T_SELF, 
                T_SUPER, 
                T_NULL, 
                T_TRUE,
                T_FALSE, 
                T_NUMBER, 
                T_HEX_NUMBER, 
                T_STRING,
                T_MAP, 
                T_LIST,
                T_IDENTIFIER,
                T_COMPOUND
            };
        public: 
            ASTPrimaryExpr(int type, const Location &location)
                :ASTExpr(location), m_type(type), m_expr(NULL), m_resultType(NULL){}
            ASTPrimaryExpr(int type, const string &text, const Location &location)
                :ASTExpr(location), m_type(type),m_text(text),m_expr(NULL), m_resultType(NULL){}
            ASTPrimaryExpr(int type, ASTExpr *expr, const Location &location)
                :ASTExpr(location),m_type(type),m_expr(expr),m_resultType(NULL){}
            ~ASTPrimaryExpr(){ 
                if (m_resultType)
                    delete m_resultType;
            }
            void appendSelector(ASTSelectorExpr *sel){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType(); 
            bool isConstant(); 
        public:
            int m_type;
            string m_text;
            ASTExpr *m_expr;
            Type *m_resultType;
    };

    class ASTArgumentList : public ASTExpr {
        public:
            ASTArgumentList(const Location &location):ASTExpr(location){}
            ~ASTArgumentList(){}
            void walk(ASTVistor *vistor) { vistor->accept(*this); }
            void appendArgument(ASTExpr *expr) { m_arguments.push_back(expr); }
            Type* getType() { return NULL; }
            bool isConstant() { return false; }
        public:
            vector<ASTExpr*> m_arguments;

    };

    class ASTMethodCallExpr : public ASTExpr {
        public:
            ASTMethodCallExpr(const Location &location):ASTExpr(location){}
            ASTMethodCallExpr(const string &methodName, const Location &location)
                :ASTExpr(location), m_methodName(methodName){}
            ~ASTMethodCallExpr() {
                vector<ASTExpr*>::iterator ite;
                for (ite = m_arguments.begin(); ite != m_arguments.end(); ite++)
                    delete *ite;
            }
            void walk(ASTVistor *visitor) { 
                visitor->accept(*this);
            }
            Type* getType() { return NULL; }
            bool isConstant() { return false; }
            void setMethodName(const string& name) {
                m_methodName = name;
            }
            string& getMethodName() { return m_methodName; }
            
            void appendArgument(ASTExpr *expr) {
                if (expr)
                    m_arguments.push_back(expr);
            }
            int getArgumentCount() {
                return (int)m_arguments.size();
            }
            ASTExpr* getArgumentExpr(int index) {
                if (index >= 0 && index < (int)m_arguments.size())
                    return m_arguments[index];
                else
                    return NULL;
            }
        public:
            string m_methodName;
            vector<ASTExpr*> m_arguments;    
    };

    /// 'class IterableObject 
    /// the iterable object can be iterated by foreach statement
    class ASTNewExpr : public ASTExpr {
        public:
            ASTNewExpr(const string &type, ASTArgumentList *list, const Location &location)
                :ASTExpr(location), m_type(type), m_arguments(list){}
            ~ASTNewExpr(){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return NULL; }
            bool isConstant() { return false; } 
        public:
            string m_type;
            ASTArgumentList *m_arguments; 
        };

    class ASTTypeExpr: public ASTExpr {
        public:
            enum { TE_SET, TE_MAP, TE_USER, TE_BUILTIN};
        public:
            ASTTypeExpr(int type, const string &name, const Location &location)
                :ASTExpr(location), m_name1(name){}
            ASTTypeExpr(int type, const string &name1, const string &name2, const Location &location)
                :ASTExpr(location), m_name1(name1),m_name2(name2){}
            ~ASTTypeExpr(){}
            Type* getType() { return NULL; }
            bool isConstant(){ return false; } 
        public:
            int m_type;
            string m_name1;
            string m_name2; 
            
    };

    class ASTMapItemExpr: public ASTExpr {
        public:
            ASTMapItemExpr(ASTExpr *key, ASTExpr *val, const Location &location)
                :ASTExpr(location), m_key(key),m_val(val){}
            ~ASTMapItemExpr(){}  
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return NULL; }
            bool isConstant(); 
        public:
            ASTExpr *m_key;
            ASTExpr *m_val;
    };

    class ASTMapExpr : public ASTExpr {
        public:
            ASTMapExpr(const Location &location):ASTExpr(location){}
            ASTMapExpr(int type, const Location &location):ASTExpr(location){}
            ~ASTMapExpr(){}
            void walk(ASTVistor *vistor){ vistor->accept(*this);}
            void appendItem(ASTMapItemExpr *item){ m_items.push_back(item);}
            Type* getType() { return NULL; }
            bool isConstant(); 

        public:
            vector<ASTMapItemExpr*> m_items;
    };


    class ASTSetExpr : public ASTExpr {
        public:
            ASTSetExpr(ASTExprList *exprList, const Location &location)
                :ASTExpr(location){}
            ASTSetExpr(const Location &location):ASTExpr(location){}
            void walk(ASTVistor *visitor){ visitor->accept(*this);}
            Type* getType() { return NULL; }
            bool isConstant(); 
        public:
            ASTExprList *m_exprList;
    };

} // namespace tlang 
#endif // __TL_AST_EXPR_H__
