//
//  Grammar.h
//  A toyable language compiler (like a simple c++)

#ifndef TCC_GRAMMAR_H
#define TCC_GRAMMAR_H

#include "FA.h"
#include "Tokens.h"


class Grammar 
{
public:
    static const string TerminalIdentifier;
    static const string TerminalNumber;
    static const string TerminalString;
    static const string TerminalHexNumber;

public:
    struct State 
    {
        vector<pair<int, int> > arcs;
        bool isFinal;
    };

    struct StateEntry 
    {
        vector<State> states;
        vector<int> first;
    };

public:
    static Grammar& getInstance();
    bool build(const string &fullFileName);

    vector<StateEntry>& getStates();
    StateEntry* getNonterminalState(int id);
    bool isLabelInState(int label, StateEntry &stateEntry);
    int getStartStateIndex() { return m_start; }

    int getKeywordLabel(const string &w);
    int getTerminalLabel(const string &w);
    int getOperatorLabel(const string &w);
    bool isKeyword(const string &w);
    bool isNonterminal(int id);
    bool isTerminal(int id);
    const string& getTerminalName(int id);
    const string& getNonterminalName(int id);
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
    void dumpNFAs(const string &name, NFA *start, NFA *end);
    void dumpDFAs(const string &name, vector<DFA *> &dfas);
private:
    TokenStream m_tokens;
    map<string, vector<DFA *> *> m_dfas;
    map<string, vector<string> > m_first; 
    
    vector<StateEntry> m_states;    // all state entry
    int m_start;                            // start index 
   
    string m_firstNonterminal;               // first nontermiinal
    vector<int>      m_labels;              // all labels
    
    map<string, int> m_terminals;           // all terminals such as IDENTIFIER
    map<int, string> m_terminalName;        // terminal label index and name
    
    map<string, int> m_nonterminals;        // non-terminal name nad lable index map
    map<int, string> m_nonterminalName;     // non-terminal label  and name map
    map<string, int> m_nonterminalState;

    map<string, int> m_keywords;          // keyword ids
    map<string, int> m_operators;         // operator maps
    static bool m_isInitialized;
};

#endif // TCC_GRAMMAR_H
