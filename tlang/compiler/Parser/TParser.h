//
//  TParser.h
//  A toyable language compiler (like a simple c++)

#ifndef TCC_TPARSER_H
#define TCC_TPARSER_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdarg>
#include "FA.h"
#include "Tokens.h"
#include "TGrammar.h"

class NoMatchedTokenException : public std::exception 
{
public:
    NoMatchedTokenException(int type){}
    NoMatchedTokenException(const char *token){}
    ~NoMatchedTokenException() throw(){}
};

class TParser 
{
public:
    TParser();
    ~TParser();
    //! parse the grammar file and generate the dfas;
    void build(const string &file, Grammar *grammar);
    // !output the dfas into a specified file.cpp which will be used by compiler
    void output(const string &file);
        
private:
    bool parseGrammarFile(const string &file);
    void parseRule(string &name, NFA **start, NFA **end);
    void parseAlternative(NFA **start, NFA**end);
    void parseItems(NFA **start, NFA **end);
    void parseItem(NFA **start, NFA **end);
    void parseAtom(NFA **start, NFA **end);
    
    void match(int type,Token **token = NULL);
    bool isMatch(int type, const char *name = NULL);
    void match(int type, const char *name = NULL);
    void advanceToken(Token **token = NULL);
    
    int  makeLabel(string &label);
    int  getStateIndex(vector<DFA*> *dfas, DFA *dfa);
    void stripLabel(string &label);
    
    void initializeBuiltinIds();
    void initializeFirstset();
    void getFirstSet(string &name, vector<DFA*> *dfa, vector<string> &newset);
    void makeFirst(vector<DFA*> *dfas, string &label, vector<int> *firstset);
    void dumpAllBuiltinIds();    
private:
    TokenStream m_tokens;
    map<string, vector<DFA *> *> m_dfas;
    map<string, vector<string> > m_first; 
    Grammar *m_grammar;
};

#endif // TCC_TPARSER_H