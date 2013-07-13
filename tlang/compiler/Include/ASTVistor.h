//
//  ASTVistor.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_AST_VISTOR_H
#define TCC_AST_VISTOR_H

#include "Common.h"

/// declarations
class Declaration;
class PackageDeclaration;
class ImportDeclaration;
class Annotation;
class ClassModifier;
class Class;
class ClassBlock;
class Variable;
class TypeDecl;
class Method;
class FormalParameter;
class FormalParameterList;
class MethodBlock;
class ArgumentList;
class IterableObjectDecl;
class MapInitializer;
class MapPairItemInitializer;
class ArrayInitializer;

// Statement
class Block;
class LocalVariableDeclarationStatement;
class Statement;
class BlockStatement;
class IfStatement;
class WhileStatement;
class DoStatement;
class ForStatement;
class ForeachStatement;
class SwitchStatement;
class ContinueStatement;
class BreakStatement;
class ReturnStatement;
class ThrowStatement;
class TryStatement;
class CatchStatement;
class FinallyCatchStatement;
class AssertStatement;
class ExprStatement;

// Expr
class Expr;
class ExprList;
class RelationalExpr;
class ConditionalExpr;
class AssignmentExpr;
class LogicOrExpr;
class LogicAndExpr;
class BitwiseOrExpr;
class BitwiseAndExpr;
class BitwiseXorExpr;
class EqualityExpr;
class RelationExpr;
class ShiftExpr;
class AdditiveExpr;
class MultiplicativeExpr;
class UnaryExpr;
class PrimaryExpr;
class SelectorExpr;
class NewExpr;
class MethodCallExpr;

/// class' ASTVistor
/// all subclass that will access the abstract syntax tree 
/// will inherit the ASTVistor 
class ASTVistor 
{
public:
    virtual void accept(Declaration& decl) = 0;
    virtual void accept(PackageDeclaration& decl) = 0;
    virtual void accept(ImportDeclaration& decl) = 0;
    virtual void accept(Annotation& annotation) = 0;
    virtual void accept(Class& cls) = 0;
    virtual void accept(ClassBlock& block) = 0;
    virtual void accept(Variable& variable) = 0;
    virtual void accept(TypeDecl& type) = 0;
    virtual void accept(Method& method) = 0;
    virtual void accept(FormalParameterList& list) = 0;
    virtual void accept(FormalParameter& para) = 0;
    virtual void accept(MethodBlock& block) = 0;
    virtual void accept(ArgumentList& arguments) = 0;
    virtual void accept(IterableObjectDecl& object) = 0;
    virtual void accept(MapInitializer& mapInitializer) = 0;
    virtual void accept(MapPairItemInitializer& mapPairItemInitializer) = 0;
    virtual void accpet(ArrayInitializer& arrayInitializer) = 0;
    
    // statement
    virtual void accept(Block& block) = 0; 
    virtual void accept(Statement& stmt) = 0;
    virtual void accept(LocalVariableDeclarationStatement& stmt) = 0;
    virtual void accept(BlockStatement& stmt) = 0;
    virtual void accept(IfStatement& stmt) = 0;
    virtual void accept(WhileStatement& stmt) = 0;
    virtual void accept(DoStatement& stmt) = 0;
    virtual void accept(ForStatement& stmt) = 0;
    virtual void accept(SwitchStatement& stmt) = 0;
    virtual void accept(ForeachStatement& stmt) = 0;
    virtual void accept(ContinueStatement& stmt) = 0;
    virtual void accept(BreakStatement& stmt) = 0;
    virtual void accept(ReturnStatement& stmt) = 0;
    virtual void accept(ThrowStatement& stmt) = 0;
    virtual void accept(AssertStatement& stmt) = 0;
    virtual void accept(TryStatement& stmt) = 0;
    virtual void accept(CatchStatement& stmt) = 0;
    virtual void accept(FinallyCatchStatement& stmt) = 0;
    virtual void accept(ExprStatement& stmt) = 0; 
    
    // expression
    virtual void accept(Expr& expr) = 0;
    virtual void accept(ExprList& list) = 0;
    virtual void accept(AssignmentExpr& expr) = 0;
    virtual void accept(ConditionalExpr& expr) = 0;
    virtual void accept(LogicOrExpr& expr) = 0;
    virtual void accept(LogicAndExpr& expr) = 0;
    virtual void accept(BitwiseOrExpr& expr) = 0;
    virtual void accept(BitwiseXorExpr& expr) = 0;
    virtual void accept(BitwiseAndExpr& expr) = 0;
    virtual void accept(EqualityExpr& expr) = 0;
    virtual void accept(RelationalExpr& expr) = 0;
    virtual void accept(ShiftExpr& expr) = 0;
    virtual void accept(AdditiveExpr& expr) = 0;
    virtual void accept(MultiplicativeExpr& expr) = 0;
    virtual void accept(UnaryExpr& expr) = 0;
    virtual void accept(PrimaryExpr& expr) = 0;
    virtual void accept(MethodCallExpr& expr) = 0;
    virtual void accept(SelectorExpr& expr) = 0;
    
    // new
    virtual void accept(NewExpr& expr) = 0;
 };
#endif // TCC_AST_VISTOR_H
