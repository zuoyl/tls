//
//  Grammar.h
//  A toyable language compiler (like a simple c++)

#ifndef TCC_GRAMMAR_H
#define TCC_GRAMMAR_H

#include "FA.h"
#include "Tokens.h"


struct GrammarState 
{
    vector<pair<int, int> > arcs;
    bool isFinal;
};

struct GrammarStateEntry 
{
    vector<GrammarState> states;
    vector<int> first;
};

struct GrammarStates
{
    int terminalsCount;
    int nonterminalsCount;
    struct State {
        int state;
        bool isFinal;
    } *states;
};

class Grammar 
{
public:
    enum { Terminal, Nonterminal };
public:

public:
    static Grammar& getInstance();
    bool build(const string &fullFileName);
    GrammarStates& getGrammarStates() { return m_gramStates; }
    // the below two method will be deleted in future
    vector<GrammarStateEntry>& getStates();
    GrammarStateEntry* getNonterminalState(int id);
    
    bool isLabelInState(int label, GrammarStateEntry &stateEntry);
    int getStartStateIndex() { return m_start; }
    int getLabel(int kind, const string &name);
    void getLabelName(int label, string &name);
    
    bool isNonterminal(int id);
    bool isTerminal(int id);
    bool isKeyword(const string &w);
    bool isOperator(const string &w);
private:
    Grammar();
    ~Grammar();

    bool parseGrammarFile(const string &file);
    void parseRule(string &ruleName, NFA **start, NFA **end);
    void parseAlternative(const string &ruleName, NFA **start, NFA**end);
    void parseItems(const string &ruleName, NFA **start, NFA **end);
    void parseItem(const string &ruleName, NFA **start, NFA **end);
    void parseAtom(const string &ruleName, NFA **start, NFA **end);
    
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
    bool isKeyword(int id); 
    void dumpNFAs(const string &name, NFA *start, NFA *end);
    void dumpDFAs(const string &name, vector<DFA *> &dfas);
private:
    TokenStream m_tokens;
    map<string, vector<DFA *> *> m_dfas;
    map<string, vector<string> > m_first; 
    
    vector<GrammarStateEntry> m_states;    // all state entry
    int m_start;                            // start index 
   
    string m_firstNonterminal;               // first nontermiinal
    vector<int>      m_labels;              // all labels
    
    map<string, int> m_terminals;           // all terminals such as IDENTIFIER
    map<int, string> m_terminalName;        // terminal label index and name
    
    map<string, int> m_nonterminals;        // non-terminal name nad lable index map
    map<int, string> m_nonterminalName;     // non-terminal label  and name map
    map<string, int> m_nonterminalState;

    map<string, int> m_keywords;          // keyword ids
    map<int, string> m_keywordName; 
    map<string, int> m_operators;         // operator maps
    map<int, string> m_operatorName; 

    GrammarStates m_gramStates;
    static bool m_isInitialized;
};

#endif // TCC_GRAMMAR_H
