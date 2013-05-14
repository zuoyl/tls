//
//  main.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Compile.h"

int main (int argc, const char * argv[])
{    

    const char *options = "-W 1";
    Compiler &compiler = Compiler::getInstance();
    compiler.parseOptions(options);
    
    const char *files[2]= { "sampleCode.txt", " sampleCode2.txt" };
    vector<string> sourceFiles;
    sourceFiles[0] = files[0];
    sourceFiles[1] = files[1];
    compiler.compile(sourceFiles);
    
    return 0;
}

