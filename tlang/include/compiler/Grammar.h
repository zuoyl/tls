//
//  Grammar.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_GRAMMAR_H
#define TCC_GRAMMAR_H

#include "compiler/Common.h"
#include "compiler/TokenStream.h"

using namespace std;

class NFA;
class DFA;


typedef std::vector<NFA *> NFASet;
typedef std::vector<DFA *> DFASet;

class NFA {
public:
    NFA();
    ~NFA();
    
    void arc(NFA *to, const string &label);
    void arc(NFA *to, const char *label = NULL);
    void findUnlabeldState(NFASet &nfaset);
    NFA& operator = (NFA &rhs);
    
public:
    vector<pair<string, NFA *> > m_arcs;
    
};

class DFA {
public:
    DFA(NFASet &nfaset, NFA *finalState);
    ~DFA();
    void arc(DFA *to, string &label);
    void arc(DFA *to, const char *label = NULL);
    bool operator == (DFA &rhs);
    void unifyState(DFA *newState, DFA *oldState);
public:
    bool m_isFinal;
    DFA *m_first;
    map<string, DFA * > m_arcs;
    NFASet m_nfas;
};



struct GrammarState {
    vector<pair<int, int> > arcs;
    bool isFinal;
};

struct GrammarStateEntry {
    vector<GrammarState> states;
    vector<int> *first;
};

class Grammar {
public:
    string grammFile;
    vector<GrammarStateEntry> states;   // all state entry
    int start;                          // start index 

    vector<int>      labels;            // all labels
    map<string, int> symbolIDs;         // symbol id for non-terminal
    map<int, string> symbolNames;       // symbol name for non-terminal
    map<string, int> symbolToLabel;     // symbol to label mapping
    map<string, int> keywordIDs;        // keyword ids
    map<string, int> operatormap;       // operator maps
    map<string, int> tokens;            // all terminal tokens, such as IDENTIFIER
    map<int, int>    tokenIDs;          // token ID and lable index mapping
    
public:
    Grammar(){}
    ~Grammar(){}
    void build(const char *file);
    
private:
    bool parseGrammarFile(const char *file);
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
    void getFirstSet(string &name, DFASet *dfa, vector<string> &newset);
    
private:
    TokenStream m_tokens;
    std::map<std::string, DFASet *> m_dfas;
    map<string, std::vector<string> > m_first; 
};


bool isSameNFASet(NFASet &nfas1, NFASet &nfas2);
DFASet* convertNFAToDFA(NFA *start, NFA *end);
void simplifyDFA(const string &name, DFA *state);


#endif // TCC_GRAMMAR_H
