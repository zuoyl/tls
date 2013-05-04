//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "compiler/Common.h"
#include "compiler/TokenStream.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"
#include "compiler/Scope.h"
#include "compiler/ASTBuilder.h"
#include "compiler/TypeBuilder.h"
#include "compiler/IRBuilder.h"


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

