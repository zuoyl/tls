//
//  Grammar.h
//  A toyable language compiler (like a simple c++)

#ifndef TCC_GRAMMAR_H
#define TCC_GRAMMAR_H

#include "FA.h"
#include "Tokens.h"

// GrammarState will be used by parser
struct GrammarState
{
    map<int, int> arcs;
    bool isFinal;
};

struct GrammarStates 
{
    vector<GrammarState> states;
    vector<int> firstset;
};


class Grammar 
{
public:
    enum { Terminal, Nonterminal };
public:
    // get instance for Grammar 
    static Grammar& getInstance();
   
    // parse the grammar file and construct the internal data 
    bool build(const string &fullFileName);
   
    // get states
    GrammarStates* getStates(int index);
   
    // the parser get first nonterminal index 
    int getStartStateIndex() { return m_start; }
    // symbols contains terminal and nonterminal 
    int getSymbolID(int lableIndex);
    // get label for the specified kind token name
    int getLabel(int kind, const string &name);
   
    // get the label name  
    void getLabelName(int label, string &name);
   
    // check wether the label is nonterminal 
    bool isNonterminal(int id);
    
    // check wether the label is terminal  
    bool isTerminal(int label);
   
    // check wether the specified token is keyword 
    bool isKeyword(const string &w);

    // check wether the specified token is operator 
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
    void makeStateTableForNonterminal(const string &name, vector<DFA *> &dfas);
private:
    // token holder for all tokens from grammar file 
    TokenStream m_tokens;
   
    // nonterminals name and dfa map 
    map<string, vector<DFA *> *> m_dfas;
    
    // nonterminals label and dfa map 
    map<int, vector<DFA *> *> m_ldfas; 
    map<string, vector<string> > m_first; 
    
    // first state 
    int m_start;  
    // all states, key is nonterminal id
    map<int, GrammarStates> m_states;
    
    // first nonterminal 
    string m_firstNonterminal;           
    // all labels place holder, nonterminal's label is biger than 256 
    vector<int>      m_labels;            
    
    // terminal label and name map 
    map<string, int> m_terminals; 
    map<int, string> m_terminalName;
   
    // nonterminal label and name map
    map<string, int> m_nonterminals;   
    map<int, string> m_nonterminalName; 
    map<string, int> m_nonterminalState;
    
    // keyword label and name map
    map<string, int> m_keywords; 
    map<int, string> m_keywordName; 
   
    // operator label and name map 
    map<string, int> m_operators;
    map<int, string> m_operatorName; 

    // flag to indicate wether the grammar is initialized
    static bool m_isInitialized;
};

#endif // TCC_GRAMMAR_H
