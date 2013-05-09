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
    vector<int> first; // should be check
};

class Grammar 
{
public:
    static Grammar* getInstance();
    bool build(const string &file);
    vector<struct TStateEntry> &getStates();
    int getStartStateIndex() { return m_start; }
    bool isKeyword(const string &w);
    int getKeywordLabelIndex(const string &w);
    int getTokenLabelIndex(const string &w);
    int getSymbolID(int labelid);
    const string& getSymbolNameByLabelIndex(int id);
private:
    Grammar();
    ~Grammar();
private:
    map<string, vector<string> > m_first; 
    vector<struct TStateEntry> m_states;   // all state entry
    int m_start;                          // start index 
    vector<int>      m_labels;            // all labels
    map<string, int> m_symbolIDs;         // symbol id for non-terminal
    map<int, string> m_symbolNames;       // symbol name for non-terminal
    map<string, int> m_symbolToLabel;     // symbol to label mapping
    map<string, int> m_keywordIDs;        // keyword ids
    map<string, int> m_operatormap;       // operator maps
    map<string, int> m_tokens;            // all terminal tokens, such as IDENTIFIER
    map<int, int>    m_tokenIDs;          // token ID and lable index mapping
    static bool m_isInitialized;

friend class TParser;
};

#endif // TCC_TGRAMMAR_HTCC_TGRAMMAR_H
