//
//  TGrammar.cpp
//  A toyable language compiler (like a simple c++)

#include "Grammar.h"
#include "GrammarParser.h"

bool Grammar::m_isInitialized = false;
const string Grammar::TerminalIdentifier = "IDENTIFIER";
const string Grammar::TerminalNumber =   "NUMBER";
const string Grammar::TerminalString  = "STRING";
const string Grammar::TerminalHexNumber = "HEX_NUMBER";

Grammar::Grammar(){}
Grammar::~Grammar(){}
Grammar& Grammar::getInstance()
{
    static Grammar grammar;
    return grammar;
}

bool Grammar::build(const string &file)
{
    try {
        GrammarParser parser;
        parser.build(file, this);
        return true;
    }
    catch (NoMatchedTokenException &expt) {
        return false;
    }
}

vector<Grammar::StateEntry>& Grammar::getStates()
{
    return m_states;
}

Grammar::StateEntry* Grammar::getNonterminalState(int id)
{
    if (m_nonterminalName.find(id) != m_nonterminalName.end()) {
        string name = m_nonterminalName[id];
        if (m_nonterminalState.find(name) != m_nonterminalState.end()) {
            int index = m_nonterminalState[name];
            if (index < (int)m_states.size())
                return &m_states[index];
        }
    }
    return NULL;
}

bool Grammar::isKeyword(const string &w)
{
    if (m_keywords.find(w) != m_keywords.end())
        return true;
    else
        return false;
}

int Grammar::getKeywordLabel(const string &w)
{
    if (m_keywords.find(w) != m_keywords.end())
        return m_keywords[w];
    else
        return -1;
}

bool Grammar::isNonterminal(int id)
{
    if (m_nonterminalName.find(id) != m_nonterminalName.end())
        return true;
    else
        return false;

}

bool Grammar::isTerminal(int id)
{
    if (m_terminalName.find(id) != m_nonterminalName.end())
        return true;
    else
        return false;
}



int Grammar::getTerminalLabel(const string &w)
{
    if (m_terminals.find(w) != m_terminals.end())
        return m_terminals[w];
    return -1;
}

int Grammar::getOperatorLabel(const string &w)
{
    if (m_operators.find(w) != m_operators.end())
        return m_operators[w];
    return -1;
}


const string& Grammar::getTerminalName(int id)
{
    if (m_terminalName.find(id) != m_terminalName.end())
        return m_terminalName[id];
    else
        throw "error"; // temp
}


const string& Grammar::getNonterminalName(int id)
{
    if (m_nonterminalName.find(id) != m_terminalName.end())
        return m_nonterminalName[id];
    else
        throw "error"; // temp
}

/// check wether the label is in the specified state
bool Grammar::isLabelInState(int label, Grammar::StateEntry &stateEntry) 
{
    vector<State> &states = stateEntry.states;
    vector<State>::iterator ite;
    
    for (ite = states.begin(); ite < states.end(); ite++) {
        State state = *ite;
        
        vector<pair<int, int> > &arcs = state.arcs;
        for (int i = 0; i < arcs.size(); i++) {
            if (label == arcs[i].first)
                return true;
        }
    }
    return false;
}
