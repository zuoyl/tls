//
//  TParser.h
//  A toyable language compiler (like a simple c++)

#ifndef TCC_TPARSER_H
#define TCC_TPARSER_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <ifstream>
#include "FA.h"
#include "Tokens.h"

struct State {
    vector<pair<int, int> > arcs;
    bool isFinal;
};

struct StateEntry {
    vector<State> states;
    vector<int> *first;
};

class TParser {
public:
    TParser(){}
    ~TParser(){}
    void build(const string &file);
    void output(const string &file)
    
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
    Token * advanceToken();
    
    int  makeLabel(string &label);
    int  makeFirst(string &label);
    
    int  getStateIndex(DFASet *dfas, DFA *dfa);
    void stripLabel(string &label, const char *chars, string &newLabel);
    
    void initializeBuiltinIds();
    void initializeFirstset();
    vector<string>* makeFirstSet(string &name);
    void getFirstSet(string &name, vector<DFA*> *dfa, vector<string> &newset);
    
private:
    TokenStream m_tokens;
    map<string, vector<DFA *> *> m_dfas;
    map<string, vector<string> > m_first; 
    
    vector<GrammarStateEntry> m_states;   // all state entry
    int m_start;                          // start index 

    vector<int>      m_labels;            // all labels
    map<string, int> m_symbolIDs;         // symbol id for non-terminal
    map<int, string> m_symbolNames;       // symbol name for non-terminal
    map<string, int> m_symbolToLabel;     // symbol to label mapping
    map<string, int> m_keywordIDs;        // keyword ids
    map<string, int> m_operatormap;       // operator maps
    map<string, int> m_ttokens;            // all terminal tokens, such as IDENTIFIER
    map<int, int>    m_tokenIDs;          // token ID and lable index mapping
};

#endif // TCC_TPARSER_H