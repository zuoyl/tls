//
//  tl-ast-xml.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_AST_XML_H__
#define __TL_AST_XML_H__

#include "tl-ast.h"
#include "tl-ast-vistor.h"
#include "tl-scope.h"
#include <string>


namespace tl {
    class ASTXml : public ASTVistor {
        public:
            explicit ASTXml(const string &path, const string &file);
            ~ASTXml();
            
            void build(AST *ast);
            // decarations
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
            ASTXml(); 
            void handleSelectorExpr(ASTPrimaryExpr &primExpr, vector<ASTSelectorExpr *> &elements);
            void walk(AST *node);
            void pushXmlNode(xmlNodePtr node);
            void popXmlNode();
            // for xml output
            xmlNodePtr m_rootXmlNode;
            xmlDocPtr m_xmlDoc;
            xmlNodePtr m_curXmlNode;
            std::stack<xmlNodePtr> m_xmlNodes; 
            std::string m_file;
            std::string m_path;
    };
} // namespace tl
#endif // __TL_AST_XML_H__
