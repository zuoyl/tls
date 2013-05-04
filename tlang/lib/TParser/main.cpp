//
//  main.cpp
//  A toyable language compiler (like a simple c++)

#include "TParser.h"
#include "FA.h"
#include "TokenStream.h"
#inclue <iostream>

using namespace std;

void usage() {
    cout << "TParserGenerator inputFile outFile" << std::endl;
    cout << "inputFile: the grammar file" << std::endl;
    cout << "outFile: the parser file generated by TParserGenerator" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        return 0;
    }
    const string grammarFile = argv[0];
    
    TParser tparser;
    TGrammar tgrammar;
    cout << "building parser using %s..." << grammarFile.c_str() << std::endl;
    tparser.build(grammarFile, &tgrammar);    
    return 0;
}
