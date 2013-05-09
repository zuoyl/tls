//
//  TGrammar.cpp
//  A toyable language compiler (like a simple c++)

#include "TGrammar.h"
#include "TParser.h"

bool Grammar::m_isInitialized = false;

Grammar::Grammar(){}
Grammar::~Grammar(){}
Grammar* Grammar::getInstance()
{
    static Grammar grammar;
    return &grammar;
}

bool Grammar::build(const string &file)
{
    try {
        TParser parser;
        parser.build(file, this);
        return true;
    }
    catch (NoMatchedTokenException &expt) {
        return false;
    }
}

vector<TStateEntry>& Grammar::getStates()
{
    return m_states;
}

bool Grammar::isKeyword(const string &w)
{
    if (m_keywordIDs.find(w) != m_keywordIDs.end())
        return true;
    else
        return false;
}

int Grammar::getKeywordLabelIndex(const string &w)
{
#if 0
    if (m_symbolToLabel.find(w) != m_symbolToLabel.end())
        return m_symbolToLabel[w];
    else
        return -1;
#endif
    return 0;
}

int Grammar::getTokenLabelIndex(const string &w)
{
    if (m_tokens.find(w) != m_tokens.end()) {
        // get token id
        int tokenID = m_tokens[w];
        // get lable index by token id
        if (m_tokenIDs.find(tokenID) != m_tokenIDs.end())
            return m_tokenIDs[tokenID];

    }
    return -1;
}

int Grammar::getSymbolID(int labelid)
{
    if (m_symbolNames.find(labelid) != m_symbolNames.end()) {
        string &name = m_symbolNames[labelid];
        if (m_symbolIDs.find(name) != m_symbolIDs.end())
            return m_symbolIDs[name];
    }
    return -1;
}

const string& Grammar::getSymbolNameByLabelIndex(int id)
{
    if (m_symbolNames.find(id) != m_symbolNames.end())
        return m_symbolNames[id];
    else
        throw "error"; // temp
}

