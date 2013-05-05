//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "TokenStream.h"
#include "Lexer.h"
#include "Parser.h"
#include "Scope.h"
#include "ASTBuilder.h"
#include "TypeBuilder.h"
#include "IRBuilder.h"


int main (int argc, const char * argv[]) {
    
    TGrammar grammar;
    buildGrammar("grammar.txt", &grammar);

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
    if (typeBuilder.isBuildComplete()) {
        // build the ir code
        IRBuilder irBuilder;
        IRBlockList blocks;
        irBuilder.build(ast, &blocks);
    }

    
    return 0;
}

