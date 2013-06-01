//
//  TParser.h
//  A toyable language compiler (like a simple c++)
//  The file should be included by tcc parser

#ifndef TCC_TGRAMMAR_H
#define TCC_TGRAMMAR_H

#include <string>
#include <vector>
#include <map>

using namespace std;

struct TState 
{
    vector<pair<int, int> > arcs;
    bool isFinal;
};

struct TStateEntry 
{
    vector<struct TState> states;
    vector<int> first;
};

class Grammar 
{
public:
    static const string TerminalIdentifier;
    static const string TerminalNumber;
    static const string TerminalString;
    static const string TerminalHexNumber;

public:
    static Grammar& getInstance();
    bool build(const string &file);

    vector<struct TStateEntry>& getStates();
    TStateEntry* getNonterminalState(int id);
    bool isLabelInState(int label, TStateEntry &stateEntry);
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

private:
    vector<TStateEntry> m_states;    // all state entry
    int m_start;                            // start index 
   
    string m_firstNoTerminal;               // first nontermiinal
    vector<int>      m_labels;              // all labels
    
    map<string, int> m_terminals;           // all terminals such as IDENTIFIER
    map<int, string> m_terminalName;        // terminal label index and name
    
    map<string, int> m_nonterminals;        // non-terminal name nad lable index map
    map<int, string> m_nonterminalName;     // non-terminal label  and name map
    map<string, int> m_nonterminalState;

    map<string, int> m_keywords;          // keyword ids
    map<string, int> m_operators;         // operator maps
    static bool m_isInitialized;

friend class TParser;
};

#endif // TCC_TGRAMMAR_HTCC_TGRAMMAR_H
