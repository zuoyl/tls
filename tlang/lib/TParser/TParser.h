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
#include "FA.h"
#include "Tokens.h"
#include "TGrammar.h"

class NoMatchedTokenException : public std::exception 
{
public:
    NoMatchedTokenException(const char *token){}
    ~NoMatchedTokenException() throw(){}
};

class TParser 
{
public:
    TParser();
    ~TParser();
    //! parse the grammar file and generate the dfas;
    void build(const string &file, TGrammar *grammar);
    // !output the dfas into a specified file.cpp which will be used by compiler
    void output(const string &file);
    
private:
    bool parseGrammarFile(const string &file);
    void parseRule(string &name, NFA **start, NFA **end);
    void parseAlternatie(NFA **start, NFA**end);
    void parseItems(NFA **start, NFA **end);
    void parseItem(NFA **start, NFA **end);
    void parseAtom(NFA **start, NFA **end);
    
    void match(int type, const char *name = NULL);
    bool isMatch(int type, const char *name = NULL);
    void expectToken(int type, const char *name = NULL);
    void advanceToken(Token **token = NULL);
    
    int  makeLabel(string &label);
    int  makeFirst(string &label);
    
    int  getStateIndex(vector<DFA*> *dfas, DFA *dfa);
    void stripLabel(string &label, const char *chars, string &newLabel);
    
    void initializeBuiltinIds();
    void initializeFirstset();
    vector<string>* makeFirstSet(string &name);
    void getFirstSet(string &name, vector<DFA*> *dfa, vector<string> &newset);
    
private:
    TokenStream m_tokens;
    map<string, vector<DFA *> *> m_dfas;
    map<string, vector<string> > m_first; 
    TGrammar *m_grammar;
};

#endif // TCC_TPARSER_H
