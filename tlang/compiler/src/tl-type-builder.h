//
//  tl-type-builder.h
//  A toyable language compiler (ASTlike a simple c++)
//  @author:jenson.zuo@gmail.com
//


#ifndef __TL_TYPE_BUILDER_H__
#define __TL_TYPE_BUILDER_H__

#include "tl-ast.h"
#include "tl-ast-vistor.h"
#include "tl-scope.h"
#include "tl-type.h"
#include "tl-type-builtin.h"

namespace tlang {
    class TypeBuilder : public ASTVistor {
        public:
            TypeBuilder(const string &path, const string &file);
            virtual ~TypeBuilder();
            
            bool isBuildComplete();
            void build(AST* ast, TypeDomain* typeDomain);
            
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
            void enterScope(Scope *scope);
            // get root scope
            Scope* getScope();
            void exitScope();
            
            bool hasObject(const string &name, bool nested = true);
            Object* getObject(const string &name, bool nested = true);
            void defineObject(Object *object);
            
            void defineType(Type *type);
            void defineType(const string &domain, Type *type);
            Type* getType(const string &name, bool nested = true);
            Type* getType(ASTTypeDecl *spec, bool nested = true);    
            void walk(AST *node);
            

            // the following methods are for break/continue/return statement* /
            void pushMethod(ASTMethod *method);
            void popMethod();
            ASTMethod* getCurrentMethod();

            void pushIterableStatement(ASTStatement *stmt);
            void popIterableStatement();
            ASTStatement* getCurrentIterableStatement();
            
            void pushBreakableStatement(ASTStatement *stmt);
            void popBreakableStatement();
            ASTStatement* getCurrentBreakableStatement();
            
            void pushClass(ASTClass *cls);
            void popClass();
            ASTClass* getCurrentClass();
            
            void handleSelectorExpr(ASTPrimaryExpr &primExpr, vector<ASTSelectorExpr*> &elements);
        private:
            string m_file;
            string m_path;
            string m_fullName;
            Scope *m_rootScope;
            Scope *m_curScope;
            stack<ASTMethod*> m_methods;
            stack<ASTStatement*> m_iterableStmts;
            stack<ASTStatement*> m_breakableStmts;
            stack<ASTClass*> m_classes;
            TypeDomain *m_typeDomain;
            // for static class data member and method
            vector<ASTVariable*> m_globalVars;
        };

} // namespace tlang 
#endif // __TL_TYPE_BUILDER_H__
