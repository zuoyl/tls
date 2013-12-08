//
//  ASTVistor.h
//  A toyable language compiler (ASTlike a simple c++)
//


#ifndef __TL_AST_VISTOR_H__
#define __TL_AST_VISTOR_H__

#include "tl-common.h"

namespace tlang { 
    /// declarations
    class ASTCompileUnit;
    class ASTDeclaration;
    class ASTPackageDecl;
    class ASTImportDecl;
    class ASTAnnotation;
    class ASTClassModifier;
    class ASTClass;
    class ASTVariable;
    class ASTTypeDecl;
    class ASTMethod;
    class ASTFormalParameter;
    class ASTFormalParameterList;
    class ASTMethodBlock;
    class ASTArgumentList;
    class ASTIterableObjectDecl;
    class ASTMapInitializer;
    class ASTMapPairItemInitializer;
    class ASTArrayInitializer;

    // Statement
    class ASTBlock;
    class ASTStatement;
    class ASTLocalVariableDeclarationStmt;
    class ASTBlockStmt;
    class ASTIfStmt;
    class ASTWhileStmt;
    class ASTDoStmt;
    class ASTForStmt;
    class ASTForeachStmt;
    class ASTSwitchStmt;
    class ASTContinueStmt;
    class ASTBreakStmt;
    class ASTReturnStmt;
    class ASTThrowStmt;
    class ASTTryStmt;
    class ASTCatchStmt;
    class ASTFinallyCatchStmt;
    class ASTAssertStmt;
    class ASTExprStmt;

    // Expr
    class ASTExpr;
    class ASTExprList;
    class ASTRelationalExpr;
    class ASTConditionalExpr;
    class ASTAssignmentExpr;
    class ASTLogicOrExpr;
    class ASTLogicAndExpr;
    class ASTBitwiseOrExpr;
    class ASTBitwiseAndExpr;
    class ASTBitwiseXorExpr;
    class ASTEqualityExpr;
    class ASTRelationExpr;
    class ASTShiftExpr;
    class ASTAdditiveExpr;
    class ASTMultiplicativeExpr;
    class ASTUnaryExpr;
    class ASTPrimaryExpr;
    class ASTSelectorExpr;
    class ASTNewExpr;
    class ASTMethodCallExpr;

    /// class' ASTVistor
    /// all subclass that will access the abstract syntax tree 
    /// will inherit the ASTVistor 
    class ASTVistor {
        public:
            virtual void accept(ASTCompileUnit &unit) = 0;
            virtual void accept(ASTDeclaration &decl) = 0;
            virtual void accept(ASTPackageDecl &decl) = 0;
            virtual void accept(ASTImportDecl &decl) = 0;
            virtual void accept(ASTAnnotation &annotation) = 0;
            virtual void accept(ASTClass &cls) = 0;
            virtual void accept(ASTVariable &variable) = 0;
            virtual void accept(ASTTypeDecl &type) = 0;
            virtual void accept(ASTMethod &method) = 0;
            virtual void accept(ASTFormalParameterList &list) = 0;
            virtual void accept(ASTFormalParameter &para) = 0;
            virtual void accept(ASTMethodBlock &block) = 0;
            virtual void accept(ASTArgumentList &arguments) = 0;
            virtual void accept(ASTIterableObjectDecl &object) = 0;
            virtual void accept(ASTMapInitializer &mapInitializer) = 0;
            virtual void accept(ASTMapPairItemInitializer &mapPairItemInitializer) = 0;
            virtual void accpet(ASTArrayInitializer &arrayInitializer) = 0;
            
            // statement
            virtual void accept(ASTBlock &block) = 0; 
            virtual void accept(ASTStatement &stmt) = 0;
            virtual void accept(ASTLocalVariableDeclarationStmt &stmt) = 0;
            virtual void accept(ASTBlockStmt &stmt) = 0;
            virtual void accept(ASTIfStmt &stmt) = 0;
            virtual void accept(ASTWhileStmt &stmt) = 0;
            virtual void accept(ASTDoStmt &stmt) = 0;
            virtual void accept(ASTForStmt &stmt) = 0;
            virtual void accept(ASTSwitchStmt &stmt) = 0;
            virtual void accept(ASTForeachStmt &stmt) = 0;
            virtual void accept(ASTContinueStmt &stmt) = 0;
            virtual void accept(ASTBreakStmt &stmt) = 0;
            virtual void accept(ASTReturnStmt &stmt) = 0;
            virtual void accept(ASTThrowStmt &stmt) = 0;
            virtual void accept(ASTAssertStmt &stmt) = 0;
            virtual void accept(ASTTryStmt &stmt) = 0;
            virtual void accept(ASTCatchStmt &stmt) = 0;
            virtual void accept(ASTFinallyCatchStmt &stmt) = 0;
            virtual void accept(ASTExprStmt &stmt) = 0; 
            
            // expression
            virtual void accept(ASTExpr &expr) = 0;
            virtual void accept(ASTExprList &list) = 0;
            virtual void accept(ASTAssignmentExpr &expr) = 0;
            virtual void accept(ASTConditionalExpr &expr) = 0;
            virtual void accept(ASTLogicOrExpr &expr) = 0;
            virtual void accept(ASTLogicAndExpr &expr) = 0;
            virtual void accept(ASTBitwiseOrExpr &expr) = 0;
            virtual void accept(ASTBitwiseXorExpr &expr) = 0;
            virtual void accept(ASTBitwiseAndExpr &expr) = 0;
            virtual void accept(ASTEqualityExpr &expr) = 0;
            virtual void accept(ASTRelationalExpr &expr) = 0;
            virtual void accept(ASTShiftExpr &expr) = 0;
            virtual void accept(ASTAdditiveExpr &expr) = 0;
            virtual void accept(ASTMultiplicativeExpr &expr) = 0;
            virtual void accept(ASTUnaryExpr &expr) = 0;
            virtual void accept(ASTPrimaryExpr &expr) = 0;
            virtual void accept(ASTMethodCallExpr &expr) = 0;
            virtual void accept(ASTSelectorExpr &expr) = 0;
            
            // new
            virtual void accept(ASTNewExpr &expr) = 0;
     };
} // namespace tlang 
#endif // __TL_AST_VISTOR_H__
