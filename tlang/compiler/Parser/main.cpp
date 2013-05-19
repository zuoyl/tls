//
//  main.cpp
//  A toyable language compiler (like a simple c++)

#include <iostream>
#include <TParser/TGrammar.h>

using namespace std;

void usage() 
{
    cout << "tparser grammarfile" << std::endl;
    cout << "grammarfile: the grammar file" << std::endl;
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        usage();
        return 0;
    }
    const string grammarFile = argv[1];
    
    Grammar *grammar = Grammar::getInstance();
    cout << "building parser using %s..." << grammarFile.c_str() << std::endl;
    grammar->build(grammarFile);
    return 0;
}
