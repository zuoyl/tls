//
//  ASTVistor.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_AST_VISITOR_H
#define TCC_AST_VISITOR_H


#include "compiler/Common.h"

/// basic compiler unit
class Struct;
class Variable;
class Function;
class FunctionParameter;
class FunctionParameterList;
class FunctionBlock;
class Class;
class ClassBlock;
class Interface;

// Statement
class Statement;
class ImportStatement;
class BlockStatement;
class VariableDeclStatement;
class IfStatement;
class WhileStatement;
class DoStatement;
class ForStatement;
class SwitchStatement;
class ContinueStatement;
class BreakStatement;
class ReturnStatement;
class ThrowStatement;
class TryStatement;
class CatchStatement;
class FinallyCatchStatement;
class AssertStatement;


// Expression
class Expression;
class ExpressionList;
class RelationalExpression;
class ConditionalExpression;
class BinaryOpExpression;
class LogicOrExpression;
class LogicAndExpression;
class BitwiseOrExpression;
class BitwiseAndExpression;
class BitwiseXorExpression;
class EqualityExpression;
class RelationExpression;
class ShiftExpression;
class AdditiveExpression;
class MultiplicativeExpression;
class UnaryExpression;
class PrimaryExpression;
class SelectorExpression;
class NewExpression;
class MapExpression;
class ListExpression;


class ASTVisitor {
public:
    // struct
    virtual void accept(Struct &type) = 0;
    
    // variable 
    virtual void accept(Variable &var) = 0;
    
    // function
    virtual void accept(Function &function) = 0;
    virtual void accept(FunctionParameterList &list) = 0;
    virtual void accept(FunctionParameter &para) = 0;
    virtual void accept(FunctionBlock &block) = 0;
    
    // class & interface
    virtual void accep(Class &cls) = 0;
    virtual void accept(ClassBlock &block) = 0;
    virtual void accept(Interface &interface) = 0;
    
    // statement
    virtual void accept(Statement &stmt) = 0;
    virtual void accept(ImportStatement &stmt) = 0;
    virtual void accept(BlockStatement &stmt) = 0;
    virtual void accept(VariableDeclStatement &stmt) = 0;
    virtual void accept(IfStatement &stmt) = 0;
    virtual void accept(WhileStatement &stmt) = 0;
    virtual void accept(DoStatement &stmt) = 0;
    virtual void accept(ForStatement &stmt) = 0;
    virtual void accept(SwitchStatement &stmt) = 0;
    virtual void accept(ContinueStatement &stmt) = 0;
    virtual void accept(BreakStatement &stmt) = 0;
    virtual void accept(ReturnStatement &stmt) = 0;
    virtual void accept(ThrowStatement &stmt) = 0;
    virtual void accept(AssertStatement &stmt) = 0;
    virtual void accept(TryStatement &stmt) = 0;
    virtual void accept(CatchStatement &stmt) = 0;
    virtual void accept(FinallyCatchStatement &stmt) = 0;
    
    
    // expression
    virtual void accept(Expression &expr) = 0;
    virtual void accept(ExpressionList &list) = 0;
    virtual void accept(BinaryOpExpression &expr) = 0;
    virtual void accept(ConditionalExpression &expr) = 0;
    virtual void accept(LogicOrExpression &expr) = 0;
    virtual void accept(LogicAndExpression &expr) = 0;
    virtual void accept(BitwiseOrExpression &expr) = 0;
    virtual void accept(BitwiseXorExpression &expr) = 0;
    virtual void accept(BitwiseAndExpression &expr) = 0;
    virtual void accept(EqualityExpression &expr) = 0;
    virtual void accept(RelationalExpression &expr) = 0;
    virtual void accept(ShiftExpression &expr) = 0;
    virtual void accept(AdditiveExpression &expr) = 0;
    virtual void accept(MultiplicativeExpression &expr) = 0;
    virtual void accept(UnaryExpression &expr) = 0;
    virtual void accept(PrimaryExpression &expr) = 0;
    virtual void accept(SelectorExpression &expr) = 0;
    
    // new
    virtual void accept(NewExpression &expr) = 0;
    
    // map & list
    virtual void accept(MapExpression &expr) = 0;
    vritual void accept(MapItemExpression &expr) = 0;
    virtual void accept(SetExpression &expr) = 0;  
 };



#endif // TCC_AST_VISITOR_H
