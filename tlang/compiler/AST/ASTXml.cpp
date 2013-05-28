//
//  ASTXml.cpp
//  A toyable language compiler (like a simple c++)


#include "ASTXml.h"

ASTXml::ASTXml(const string &path, const string &file)
{}
ASTXml::~ASTXml()
{}
void build(AST* ast)
{} 
// type
void ASTXml::accept(TypeSpec &type)
{}
// struct
void ASTXml::accept(Struct &st)
{}
// 
// variable 
void ASTXml::accept(Variable &var)
{}

// method
void ASTXml::accept(Method &method)
{}
void ASTXml::accept(MethodParameterList &list)
{}
void ASTXml::accept(MethodParameter &para)
{}
void ASTXml::accept(MethodBlock &block)
{}

// class
void ASTXml::accep(Class &cls)
{}
void ASTXml::accept(ClassBlock &block)
{}

// protocol
void ASTXml::accept(Protocol &protocol)
{}

// statement
void ASTXml::accept(Statement &stmt)
{}
void ASTXml::accept(IncludeStatement &stmt)
{}
void ASTXml::accept(BlockStatement &stmt)
{}
void ASTXml::accept(VariableDeclStatement &stmt)
{}
void ASTXml::accept(IfStatement &stmt)
{}
void ASTXml::accept(WhileStatement &stmt)
{}
void ASTXml::accept(DoStatement &stmt)
{}
void ASTXml::accept(ForStatement &stmt)
{}
void ASTXml::accept(ForEachStatement &stmt)
{}
void ASTXml::accept(SwitchStatement &stmt)
{}
void ASTXml::accept(ContinueStatement &stmt)
{}
void ASTXml::accept(BreakStatement &stmt)
{}
void ASTXml::accept(ReturnStatement &stmt)
{}
void ASTXml::accept(ThrowStatement &stmt)
{}
void ASTXml::accept(AssertStatement &stmt)
{}
void ASTXml::accept(TryStatement &stmt)
{}
void ASTXml::accept(CatchStatement &stmt)
{}
void ASTXml::accept(FinallyCatchStatement &stmt)
{}


// expression
void ASTXml::accept(Expr &expr)
{}
void ASTXml::accept(ExprList &list)
{}
void ASTXml::accept(BinaryOpExpr &expr)
{}
void ASTXml::accept(ConditionalExpr &expr)
{}
void ASTXml::accept(LogicOrExpr &expr)
{}
void ASTXml::accept(LogicAndExpr &expr)
{}
void ASTXml::accept(BitwiseOrExpr &expr)
{}
void ASTXml::accept(BitwiseXorExpr &expr)
{}
void ASTXml::accept(BitwiseAndExpr &expr)
{}
void ASTXml::accept(EqualityExpr &expr)
{}
void ASTXml::accept(RelationalExpr &expr)
{}
void ASTXml::accept(ShiftExpr &expr)
{}
void ASTXml::accept(AdditiveExpr &expr)
{}
void ASTXml::accept(MultiplicativeExpr &expr)
{}
void ASTXml::accept(UnaryExpr &expr)
{}
void ASTXml::accept(PrimaryExpr &expr)
{}
void ASTXml::accept(SelectorExpr &expr)
{}
void ASTXml::accept(MethodCallExpr &expr)
{}

// new
void ASTXml::accept(NewExpr &expr)
{}

// map & list
void ASTXml::accept(MapExpr &expr)
{}
void ASTXml::accept(MapItemExpr &expr)
{}
void ASTXml::accept(SetExpr &expr)
{}
void ASTXml::handleSelectorExpr(PrimaryExpr &primExpr, vector<SelectorExpr *> &elements)
{}
