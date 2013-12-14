//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "tl-common.h"
#include "tl-compile.h"
#include "tl-compile-option.h"

using namespace tlang;

/// application entry
int main(int argc, const char *argv[])
{   
    // parse all compile options 
    // to debug easily, turn on the parse tree and ast xml output
    CompileOption &options = CompileOption::getInstance();
    if (!options.parse(argv, argc)) {
        std::cout << "illegal options" << std::endl;
        options.usage(cout);
        return 0;
    }
    options.setOutputParseTree(true);
    options.setOutputAST(true);
    options.setOutputAssembleFile(true); 

    // compile source files 
    vector<string> sourceFiles;
    options.getSourceFiles(sourceFiles);
    if (sourceFiles.empty()) {
        std::cout << "none files to compile" << std::endl;
        return 0;
    }

    Compiler &compiler = Compiler::getInstance();
    compiler.compile(sourceFiles);
    
    return 0;
}
