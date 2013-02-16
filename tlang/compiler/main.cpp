//
//  main.cpp
//  A toyable language compiler (like a simple c++)



#include <Common.h>
#include <Parser/Grammar.h>
#include <Lexer/TokenStream.h>
#include <Lexer/Lexer.h>
#include <Parser/Parser.h>
#include <Runtime/Scope.h>
#include <AST/ASTBuilder.h>
#include <Runtime/TypeBuilder.h>
#include <IRBuidler/IRBuilder.h>


int main (int argc, const char * argv[])
{
    Grammar grammar;
    grammar.build("grammar.txt");
    
    Lexer lexer("sampleCode.txt", &grammar);
    TokenStream tokenStream;
    lexer.parse(&tokenStream);
    
    // create the parse tree
    Parser parser(&grammar);
    Node * parseTree = parser.parse(&tokenStream);
    
    // create the AST
    ASTBuilder astBuilder;
    AST *ast = astBuilder.build(parseTree);
    
    // build the type and scope
    TypeBuilder typeBuilder;
    typeBuilder.build(ast);
    
    // build the ir code
    IRBuilder irBuilder;
    IRBlockList blocks;
    irBuilder.build(ast, &blocks);

    
    return 0;
}

