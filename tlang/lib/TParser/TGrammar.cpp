//
//  TGrammar.cpp
//  A toyable language compiler (like a simple c++)

#include "TGrammar.h"
#include "TParser.h"

int buildGrammar(const string &file, TGrammar *grammar) 
{
    try {
        TParser parser;
        parser.build(file, grammar);
        return 0;
    }
    catch (NoMatchedTokenException &expt) {
        return -1; // temp
    }
}

