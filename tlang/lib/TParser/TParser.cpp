//
//  TParser.cpp
//  A toyable language compiler (like a simple c++)

#include "TParser.h"
#include <assert.h>

enum {
    TT_NAME,
    TT_STRING,
    TT_OP,
    TT_TOKEN,
    TT_ENDMARK,
    TT_NEWLINE
};

TParser::TParser() 
{    
}

TParser::~TParser() 
{
    // the DFAs should be release
    map<string, vector<DFA *> *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        vector<DFA *> *dfaset = ite->second;
        vector<DFA *>::iterator i = dfaset->begin();
        while (i != dfaset->end()) {
            DFA *dfa = *i;
            if (dfa) delete dfa;
            i++;
        }
        dfaset->clear();
        delete dfaset;
    }
}

bool TParser::parseGrammarFile(const string & file) 
{
    bool controlFlag = false;
    int line = 0;
    Token *token = NULL;
    std::string atom = "";
    
    ifstream ifs(file.c_str());
    
    while (!ifs.eof()) {
        
        char ch = ifs.get();
        token = NULL;
        
        switch (ch) {
            case '|':
            case '*':
            case '+':
            case ':':
            case ';':
            case '(':
            case ')':
            case '[':
            case ']':
            case '?':
            case '.':
                token = new Token();
                token->type = TT_OP;
                token->assic = ch;
                break;
                
            case '\'':
                // indicate a string
                atom = "'";
                while ((ch = ifs.get()) != EOF) {
                    if (ch == '\'') {
                        atom += ch;
                        break;
                    }
                    atom += ch;
                }
                token = new Token();
                token->assic = atom;
                token->type = TT_STRING;
                token->lineno = line;
                break;
                
            case '/':
                if (controlFlag == false) {
                    controlFlag = true;
                    continue;
                }
                else {
                    // consume all line
                    while ((ch = ifs.get()) != EOF) {
                        if (ch != '\r' || ch == '\n')
                            break;
                    }
                }
                break;
            case '\r':
            case '\n':
                line++;
                break;
                
            default:
                if (isalpha(ch)) {
                    atom = "";
                    atom += ch;
                    while ((ch = ifs.get()) != EOF) {
                        if (ch == ' ')
                            break;
                        else {
                            atom += ch;
                        }
                    }
                    // get a token
                    token = new Token();
                    if (isupper(atom.at(0)))
                        token->type = TT_TOKEN;
                    else
                        token->type = TT_NAME;
                    token->assic = atom;
                    token->lineno = line;
                    
                }
        }
        if (token != NULL)
            m_tokens.pushToken(token);
        
    }
    ifs.close();
    return true;
}

/// build the grammar file
void TParser::build(const string &file, TGrammar *grammar) 
{
    assert(grammar != NULL);
    m_grammar = grammar;
    
    string first;
    parseGrammarFile(file);
    
    while (true) {
        // get ahead token from token stream
        Token *token = m_tokens.getToken();
        if (!token)
            break;
        
        // parse a rule
        NFA *start = NULL;
        NFA*end = NULL;
        string name;
        parseRule(name, &start, &end);
        
        // create a dfa accroding to the rule
        vector<DFA *> *dfaset = convertNFAToDFA(start, end);
        // simplifyDFA(name, dfaset);
        m_dfas[name] = dfaset;
#if 0
        if (m_grammar->first.empty())
            m_grammar->first = name;
#endif 
	}
    
    // symbol and id mapping
    map<string, vector<DFA*>*>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<string, vector<DFA *> *> ip = *ite;
        string name = ip.first;
        int index = (int)m_grammar->symbolIDs.size() + 255;
        m_grammar->symbolIDs[name] = index;
        m_grammar->symbolNames[index] = name;
    }
    
    
    // create the labels
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<std::string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        vector<DFA*> *dfaset = ip.second;
        
        // holder for all arcs
        TStateEntry stateEntry;
        
        // for each DFA
        vector<DFA *>::iterator it;
        for (it = dfaset->begin(); it != dfaset->end(); it++ ) {
            DFA *dfa = *it;
            
            TState state;
            // get all arcs for the dfa
            map<string, DFA *>::iterator iac;
            for (iac = dfa->m_arcs.begin(); iac != dfa->m_arcs.end(); iac++) {
                pair<string, DFA *> ipc = *iac;
                string label = ipc.first;
                DFA *dfac = ipc.second;
                int labelIndex = makeLabel(label);
                int nextStateIndex = getStateIndex(dfaset, dfac);
                state.arcs.push_back(make_pair(labelIndex, nextStateIndex));
            }
            
            // place all state into one states
            state.isFinal = dfa->m_isFinal;
            stateEntry.states.push_back(state);
        }
        
        // place all DFAS into grammar's state table
        // stateEntry.first = makeFirstSet(name);
        m_grammar->states.push_back(stateEntry);
        m_grammar->start = m_grammar->symbolIDs[first];
    }   

}


void TParser::stripLabel(string &label, const char *chars, string &newLabel) 
{
    if (chars) {
        newLabel = label.replace(label.begin(), label.end(), chars, "");
    }
}

void TParser::advanceToken(Token **token) 
{
	m_tokens.advanceToken(token);
}

void TParser::expectToken(int type, const char*name) 
{
    if (m_tokens.matchToken(type, name)) {
        throw NoMatchedTokenException(name);
    }
    
}
/// the next token must be matched with the specified token
void TParser::match(int type, const char *name) 
{
    if(!m_tokens.matchToken(type, name)) {
        throw NoMatchedTokenException(name);
    }
}

/// check wether the next token is matched with specified token
bool TParser::isMatch(int type, const char *name) 
{
    return m_tokens.matchToken(type, name);
}

/// parse a rule, such as production: alternative 
void TParser::parseRule(string &name, NFA **start, NFA **end) 
{ 
    Token *token = NULL;
    
    match(TT_NAME);
    advanceToken(&token);
    name = token->assic;
    match(TT_OP, ":");
    parseAlternatie(start, end);
    match(TT_OP, ";");
}

/// parse the alternative, such as alternative : items (| items)*
void TParser::parseAlternatie(NFA **start, NFA **end) 
{
	assert(start != NULL);
	assert(end != NULL);
    // setup new state
    *start = new NFA();
    *end = new NFA();
    
    // parse items
    NFA *itemStartState = NULL;
    NFA *itemEndState = NULL;
    parseItems(&itemStartState, &itemEndState);
    
    assert(itemStartState != NULL);
    assert(itemEndState != NULL);
    
    // connect the state
    (*start)->arc(itemStartState);
    itemEndState->arc(*end);

    while (isMatch(TT_OP, "|")) {
        advanceToken();
        itemStartState = NULL;
        itemEndState = NULL;
        parseItems(&itemStartState, &itemEndState);
        
        (*start)->arc(itemStartState);
        itemEndState->arc(*end);
    }
}


/// parse the items, such as items : item+
void TParser::parseItems(NFA **start, NFA **end) 
{
    // setup new state
    parseItem(start, end);
    assert(*start != NULL);
    assert(*end != NULL);
    
    while (isMatch(TT_NAME) || isMatch(TT_STRING) || isMatch(TT_OP, "(")) {
        // parse item
        NFA *itemStartState = NULL;
        NFA *itemEndState = NULL;
        parseItem(&itemStartState, &itemEndState);
        
        // connect the state
        (*end)->arc(itemStartState);
        **end = *itemEndState;
    }
}


// item: ATOM('+'|'*'|'?')
void TParser::parseItem(NFA **start, NFA **end) 
{
    parseAtom(start, end);
    // check to see wether repeator exist?
    if (isMatch(TT_OP, "+")) {
        (*end)->arc(*start);
        advanceToken();
    } 
    else if (isMatch(TT_OP, "*")) {
        (*end)->arc(*start);
        advanceToken();
        *end = *start;
    }
    else if (isMatch(TT_OP, "?")) {
        (*start)->arc(*end);
        advanceToken();
        
    }
    else {
        throw NoMatchedTokenException("");
    }
}

void TParser::parseAtom(NFA **start, NFA **end) 
{
    if (isMatch(TT_OP, "(")) {
        advanceToken();
        parseAtom(start, end);
        match(TT_OP, ")");
        return;
    }
    
    else if (isMatch(TT_NAME) || isMatch(TT_STRING)) {
        Token *token = NULL;
		advanceToken(&token);
        *start = new NFA();
        *end = new NFA();
        (*start)->arc(*end, token->assic);
        return;
    }
    
    else {
        throw NoMatchedTokenException("");
    }
}
    


void TParser::initializeBuiltinIds() 
{
    Token *token = m_tokens.getToken();
    
    // iterate all tokens and get keywords and operators
    while (token != NULL) {
        
        int labelIndex = (int)m_grammar->labels.size();

        // if the token type is TT_STRING, it must be keyword and operator
        string name;
        stripLabel(token->assic, "'", name);
      
        if (token->type == TT_STRING) {
            // keywords
            if (isalpha(name[0])) {
                if (m_grammar->keywordIDs.find(name) != m_grammar->keywordIDs.end()) {
                    m_grammar->keywordIDs[name] = labelIndex;
                    m_grammar->labels.push_back(labelIndex);
                }
            }
            // operator maps
            else {
                if (m_grammar->operatormap.find(name) != m_grammar->operatormap.end()) {
                    m_grammar->operatormap[name] = labelIndex;
                    m_grammar->labels.push_back(labelIndex);
                }
            }
        }
        else if (token->type == TT_TOKEN) {
            if (m_grammar->tokens.find(name) != m_grammar->tokens.end()) {
                int tokenIndex = (int)m_grammar->tokens.size();
                m_grammar->tokens[name] = tokenIndex;
                m_grammar->tokenIDs[tokenIndex] = labelIndex;
                m_grammar->labels.push_back(labelIndex);
            }
        }
        // get next token
        token = m_tokens.getToken();
    }
    
    // reset token stream index to
    m_tokens.reset();
}

int TParser::makeLabel(string &label) 
{
    int labelIndex = (int)m_grammar->labels.size();
    
    // at first, check to see wether the label is a symbol or a token
    if (isalpha(label[0])) {
        // if it's a symbol or a token
        // if the label is in symbol, just return it's index
        map<string, int>::iterator ite = m_grammar->symbolIDs.find(label);
        if (ite != m_grammar->symbolIDs.end()) {
            map<string, int>::iterator it = m_grammar->symbolToLabel.find(label);
            if (it != m_grammar->symbolToLabel.end()) {
                return m_grammar->symbolToLabel[label];
            }
            else {
                m_grammar->labels.push_back(m_grammar->symbolIDs[label]);
                m_grammar->symbolToLabel[label] = labelIndex;
                return labelIndex;
            }
        }
        
        // if the label is token
        else if (isupper(label[0])) {
            int tokenIndex = m_grammar->tokens[label];
            map<int, int>::iterator ite = m_grammar->tokenIDs.find(tokenIndex);
            if (ite != m_grammar->tokenIDs.end()) {
                return m_grammar->tokenIDs[tokenIndex];
            }
            else {
                m_grammar->labels.push_back(tokenIndex);
                m_grammar->tokenIDs[tokenIndex] = labelIndex;
                return labelIndex;
            }
        }
        
        else {
            // exception
        }
    }
    
    // check to see wether the label is key word or operator
    string value = "";
    stripLabel(label, "'", value);
    if (isalpha(value.at(0))) { // keyword
        map<string, int>::iterator ite = m_grammar->keywordIDs.find(value);
        if (ite != m_grammar->keywordIDs.end()) {
            return m_grammar->keywordIDs[value];
        }
        else {
            m_grammar->labels.push_back(labelIndex);
            m_grammar->keywordIDs[value] = labelIndex;
            return labelIndex;
        }
    }
    else { // operator
        map<string, int>::iterator ite = m_grammar->operatormap.find(value);
        if (ite == m_grammar->operatormap.end()) {
            // exception
        }
        int tokenIndex = m_grammar->operatormap[value];
        map<int, int>::iterator it = m_grammar->tokenIDs.find(tokenIndex);
        if (it != m_grammar->tokenIDs.end())
            return m_grammar->tokenIDs[tokenIndex];
        else {
            m_grammar->labels.push_back(tokenIndex);
            m_grammar->tokenIDs[tokenIndex] = labelIndex;
            return labelIndex;
        }
    }
    // exception
    return -1;
}

void TParser::initializeFirstset() 
{   
    map<string, vector<DFA*> *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        if (m_grammar->first.find(name) != m_grammar->first.end()) {
          //  getFirstSet(name, ip.second);
        }
    }
   
}

vector<string>* TParser::makeFirstSet(string &name) 
{
    return NULL;
}

int  TParser::getStateIndex(vector<DFA*> *dfas, DFA *dfa)
{
    return 0;
}


void TParser::getFirstSet(string &name, vector<DFA*> *dfas, vector<string> &newset) {
    vector<string> allLabels;
    map<string, vector<string> > overlaps;
    DFA *dfa = dfas->at(0);
    
    map<string, DFA *>::iterator ite;
    for (ite = dfa->m_arcs.begin(); ite != dfa->m_arcs.end(); ite++) {
        pair<string, DFA *> ip = *ite;
        string label = ip.first;
        
        // check to see wether the label
        if (m_dfas.find(label) != m_dfas.end()) {
            vector<string> *newLabels = NULL;
            
            if (m_first.find(label) != m_first.end()) {
                newLabels = &m_grammar->first[label];
                if (newLabels->empty()) {
                    // exception, recursion
                }
            }
            else {
     //           newLabels = getFirstSet(label, m_dfas[label]);
            }
            
            // add all new labels into all labes
            for(vector<string>::iterator it = newLabels->begin();
                it != newLabels->end();
                it++) {
                allLabels.push_back(*it);
            }
            // overlap
            overlaps[label] = *newLabels;
            delete newLabels;
            newLabels = NULL;
        }
        else {
            allLabels.push_back(label);
            vector<string> newset;
            newset.push_back(label);
            overlaps[label] = newset;
        }
    }
    
    // inverse check
    map<string, string> inverse;
    map<string, vector<string> >::iterator it;
    
    for (it = overlaps.begin(); it != overlaps.end(); it++) {
        pair<string, vector<string> > ip = *it;
        string label = ip.first;
        vector<string> subfirst = ip.second;
        
        for (vector<string>::iterator ii = subfirst.begin(); ii < subfirst.end(); ii++) {
            string sublabel = *ii;
            if (inverse.find(sublabel) != inverse.end()) {
                // throw exception
            }
            inverse[sublabel] = label;
        }
    }
    
    // 
    m_grammar->first[name] = allLabels;
        
}


