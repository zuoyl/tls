//
//  ASTVistor.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_AST_VISITOR_H
#define TCC_AST_VISITOR_H


#include "Common.h"

/// basic compiler unit
class TypeSpec;
class Variable;
class Method;
class MethodParameter;
class MethodParameterList;
class MethodBlock;
class Class;
class ClassBlock;

// Statement
class Statement;
class IncludeStatement;
class BlockStatement;
class VariableDeclStatement;
class IfStatement;
class WhileStatement;
class DoStatement;
class ForStatement;
class ForEachStatement;
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
class BinaryOpExpr;
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
class MapExpr;
class MapItemExpr;
class SetExpr;
class MethodCallExpr;


class ASTVisitor {
public:
    
    // type spec
    virtual void accept(TypeSpec& type) = 0;
    
    // variable 
    virtual void accept(Variable& var) = 0;
    
    // method
    virtual void accept(Method& method) = 0;
    virtual void accept(MethodParameterList& list) = 0;
    virtual void accept(MethodParameter& para) = 0;
    virtual void accept(MethodBlock& block) = 0;
    
    // class
    virtual void accep(Class& cls) = 0;
    virtual void accept(ClassBlock& block) = 0;
    
    // statement
    virtual void accept(Statement& stmt) = 0;
    virtual void accept(IncludeStatement& stmt) = 0;
    virtual void accept(BlockStatement& stmt) = 0;
    virtual void accept(VariableDeclStatement& stmt) = 0;
    virtual void accept(IfStatement& stmt) = 0;
    virtual void accept(WhileStatement& stmt) = 0;
    virtual void accept(DoStatement& stmt) = 0;
    virtual void accept(ForStatement& stmt) = 0;
    virtual void accept(SwitchStatement& stmt) = 0;
    virtual void accept(ForEachStatement& stmt) = 0;
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
    virtual void accept(BinaryOpExpr& expr) = 0;
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
    
    // map&  list
    virtual void accept(MapExpr& expr) = 0;
    virtual void accept(MapItemExpr& expr) = 0;
    virtual void accept(SetExpr& expr) = 0;  
 };



#endif // TCC_AST_VISITOR_H
