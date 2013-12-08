//
//  tl-ir-builder.h -walk ast to generate the IR instructions
//  A toyable language compiler (like a simple c++)
//

#ifndef __TL_IR_BUILDER_H__
#define __TL_IR_BUILDER_H__

#include "tl-common.h"
#include "tl-ast-vistor.h"
#include "tl-ir-nodeblock.h"
#include "tl-ir-emiter.h"


namespace tlang {
    class AST;
    class Label;
    class Value;
    class Frame;

    class IRBuilder : public ASTVistor {
        public:
            IRBuilder(const string &path, const string &file);
            virtual ~IRBuilder();
            void build(AST* ast, IRBlockList* blocks, TypeDomain* typeDomain);    
            // decarations
            void accept(ASTCompileUnit &unit);
            void accept(ASTDeclaration &decl);
            void accept(ASTPackageDecl &decl);
            void accept(ASTImportDecl &decl);
            void accept(ASTAnnotation &annotation);
            void accept(ASTClass &cls);
            void accept(ASTVariable &variable);
            void accept(ASTTypeDecl &type);
            void accept(ASTMethod &method);
            void accept(ASTFormalParameterList &list);
            void accept(ASTFormalParameter &para);
            void accept(ASTMethodBlock &block);
            void accept(ASTArgumentList &arguments);
            void accept(ASTIterableObjectDecl &object);
            void accept(ASTMapInitializer &mapInitializer);
            void accept(ASTMapPairItemInitializer &mapPairItemInitializer);
            void accpet(ASTArrayInitializer &arrayInitializer);
            
            // statement
            void accept(ASTBlock &block); 
            void accept(ASTStatement &stmt);
            void accept(ASTLocalVariableDeclarationStmt &stmt);
            void accept(ASTBlockStmt &stmt);
            void accept(ASTIfStmt &stmt);
            void accept(ASTWhileStmt &stmt);
            void accept(ASTDoStmt &stmt);
            void accept(ASTForStmt &stmt);
            void accept(ASTSwitchStmt &stmt);
            void accept(ASTForeachStmt &stmt);
            void accept(ASTContinueStmt &stmt);
            void accept(ASTBreakStmt &stmt);
            void accept(ASTReturnStmt &stmt);
            void accept(ASTThrowStmt &stmt);
            void accept(ASTAssertStmt &stmt);
            void accept(ASTTryStmt &stmt);
            void accept(ASTCatchStmt &stmt);
            void accept(ASTFinallyCatchStmt &stmt);
            void accept(ASTExprStmt &stmt); 
            
            // expression
            void accept(ASTExpr &expr);
            void accept(ASTExprList &list);
            void accept(ASTAssignmentExpr &expr);
            void accept(ASTConditionalExpr &expr);
            void accept(ASTLogicOrExpr &expr);
            void accept(ASTLogicAndExpr &expr);
            void accept(ASTBitwiseOrExpr &expr);
            void accept(ASTBitwiseXorExpr &expr);
            void accept(ASTBitwiseAndExpr &expr);
            void accept(ASTEqualityExpr &expr);
            void accept(ASTRelationalExpr &expr);
            void accept(ASTShiftExpr &expr);
            void accept(ASTAdditiveExpr &expr);
            void accept(ASTMultiplicativeExpr &expr);
            void accept(ASTUnaryExpr &expr);
            void accept(ASTPrimaryExpr &expr);
            void accept(ASTMethodCallExpr &expr);
            void accept(ASTSelectorExpr &expr);
            void accept(ASTNewExpr &expr);
            
        private:
            void build(AST *ast);
            void makeAllGlobalVariables();
            void mangleMethodName(ASTMethod &method, string &name);
            int  getLinkAddress(ASTMethod &method);
            Value* handleSelectorExpr(ASTPrimaryExpr &primExpr, vector<ASTSelectorExpr*> &selectors);    
            void callObjectMethod(const string &objectName, 
                        const string &method, 
                        vector<Value> &arguments, 
                        Value &result){}
            
            void constructObject(int storageType, int offset, Type *type){} 
            
            /// Enter a new scope
            /// @param name the scope's name
            /// @param scope the new scope
            void enterScope(const string& name, Scope* scope);
            /// Exit current scope
            void exitScope();
            
            Object* getObject(const string& name, bool nested = true);
            Type* getType(const string &name, bool nested = true);
            Type* getType(const string &clsName, const string &name); 
            /// helper method for iterable statement
            void pushIterablePoint(ASTStatement *stmt);
            void popIterablePoint();
            ASTStatement* getCurIterablePoint();
            void clearIterablePoint();
            ASTClass* getCurrentClass();
            void pushClass(ASTClass *cls);
            void popClass();
            // alloc object in heap
            int allocHeapObject(Object *object);
        private:
            Scope *m_rootScope;
            Scope *m_curScope;
            string m_curScopeName;
            IRBlockList *m_blocks;
            vector<ASTVariable* > m_globalVars;
            vector<ASTStatement* > m_iterablePoints;
            IREmiter m_ir;
            TypeDomain *m_typeDomain;
            stack<ASTClass*> m_classStack;
    };
} // namespace tlang 
#endif // __TL_IR_BUILDER_H__
