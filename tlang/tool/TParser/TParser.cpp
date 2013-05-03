//
//  TParser.cpp
//  A toyable language compiler (like a simple c++)

#inlcude "TParser.h"

enum {
    TT_NAME,
    TT_STRING,
    TT_OP,
    TT_TOKEN,
    TT_ENDMARK,
    TT_NEWLINE
};


bool TParser::parseFile(const string & file) {
    bool controlFlag = false;
    int line = 0;
    Token *token = NULL;
    std::string atom = "";
    
    ifstream ifs(file);
    
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
void TParser::build(const char *file) {
    NFA *start = NULL;
    NFA *end = NULL;
    std::string name = "";
    std::string first = "";
    
    parseGrammarFile(file);
    
    while (true) {
        // get ahead token from token stream
        Token *token = m_tokens.getToken();
        if (!token)
            break;
        
        // parse a rule
        name = "";
        start = end = NULL;
        parseRule(name, &start, &end);
        
        // create a dfa accroding to the rule
        vector<DFA *> *dfaset = convertNFAToDFA(start, end);
        simplifyDFA(name, dfa);
        m_dfas[name] = dfaset;
        
        if (m_first.empty())
            m_first = name;
    }
    
    // symbol and id mapping
    std::map<std::string, DFASet *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        std::pair<std::string, vector<DFA *> *> ip = *ite;
        name = ip.first;
        int index = (int)this->symbolIDs.size() + 255;
        m_symbolIDs[name] = index;
        m_symbolNames[index] = name;
    }
    
    
    // create the labels
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        std::pair<std::string, DFASet *> ip = *ite;
        name = ip.first;
        vector<DFA*> *dfaset = ip.second;
        
        // holder for all arcs
        StateEntry stateEntry;
        
        // for each DFA
        vector<DFA *>::iterator it;
        for (it = dfaset->begin(); it != dfaset->end(); it++ ) {
            DFA *dfa = *it;
            
            State state;
            // get all arcs for the dfa
            map<string, DFA *>::iterator iac;
            for (iac = dfa->m_arcs.begin(); iac != dfa->m_arcs.end(); iac++) {
                pair<std::string, DFA *> ipc = *iac;
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
        m_states.push_back(stateEntry);
        m_start = m_symbolIDs[first];
    }   

}


void TParser::stripLabel(string &label, const char *chars, string &newLabel) {
    if (chars) {
        newLabel = label.replace(label.begin(), label.end(), chars, "");
    }
}

Token* TParser::advanceToken() {
    return m_tokens.getToken();
}

int TParser::getStateIndex(DFASet *dfas, DFA *dfa) {
    return 0; 
}

void TParser::expectToken(int type, const char*name) {
    if (m_tokens.matchToken(type, name)) {
        throw Exception::NoMatchedToken(name);
    }
    
}
/// the next token must be matched with the specified token
bool TParser::match(int type, const char *name) {
    if(!m_tokens.matchToken(type, name) {
        throw NoMatchTokenException(name);
    }
}

/// check wether the next token is matched with specified token
bool TParser::isMatch(int type, const char *name) {
    return m_tokens.matchToken(type, name);
}

/// parse a rule, such as production: alternative 
void TParser::parseRule(string &name, NFA **start, NFA **end) { 
    Token *token = NULL;
    
    match(TT_NAME);
    advanceToken(&token);
    name = token->assic;
    match(TT_OP, ":");
    parseAlternatie(start, end);
    match(TT_OP, ";");
}

/// parse the alternative, such as alternative : items (| items)*
void TParser::parseAlternatie(NFA **start, NFA **end) {
    // setup new state
    *start = new NFA();
    *end = new NFA();
    
    // parse items
    NFA *itemStartState = NULL;
    NFA *itemEndState = NULL;
    parseItems(&itemStartState, &itemEndState);
    
    assert(itemStartState != NULL);
    assert(itemEndState != NULL)
    
    // connect the state
    (*start)->arc(itemStartState);
    itemEndState->arc(*end);

    while (match(TT_OP, "|")) {
        advanceToken();
        itemStartState = NULL;
        itemEndState = NULL;
        parseItems(&itemStartState, &itemEndState);
        
        (*start)->arc(subStartState);
        subEndState->arc(*end);
    }
}


/// parse the items, such as items : item+
void TParser::parseItems(NFA **start, NFA **end) {
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
void TParser::parseItem(NFA **start, NFA **end) {
    parseAtom(start, end);
    // check to see wether repeator exist?
    if (match(TT_OP, "+")) {
        (*end)->arc(*start);
        advanceToken();
    } 
    else if (match(TT_OP, "*")) {
        (*end)->arc(*start);
        advanceToken();
        *end = *start;
    }
    else if (match(TT_OP, "?")) {
        (*start)->arc(*end);
        advanceToken();
        
    }
    else {
        throw NoMatchedTokenException();
    }
}

void TParser::parseAtom(NFA **start, NFA **end) {
    if (isMatchToken(TT_OP, "(")) {
        advanceToken();
        parseAtom(start, end);
        matchToken(TT_OP, ")");
        return;
    }
    
    else if (match(TT_NAME) || match(TT_STRING)) {
        Token *token = advanceToken();
        *start = new NFA();
        *end = new NFA();
        (*start)->arc(*end, token->assic);
        return;
    }
    
    else {
        throw NoMatchedTokenException();
    }
}
    


void TParser::initializeBuiltinIds() {
    Token *token = m_tokens.getToken();
    
    // iterate all tokens and get keywords and operators
    while (token != NULL) {
        
        int labelIndex = (int)this->labels.size();

        // if the token type is TT_STRING, it must be keyword and operator
        string name;
        stripLabel(token->assic, "'", name);
      
        if (token->type == TT_STRING) {
            // keywords
            if (isalpha(name[0])) {
                if (keywordIDs.find(name) != keywordIDs.end()) {
                    keywordIDs[name] = labelIndex;
                    labels.push_back(labelIndex);
                }
            }
            // operator maps
            else {
                if (operatormap.find(name) != operatormap.end()) {
                    operatormap[name] = labelIndex;
                    labels.push_back(labelIndex);
                }
            }
        }
        else if (token->type == TT_TOKEN) {
            if (tokens.find(name) != tokens.end()) {
                int tokenIndex = (int)tokens.size();
                tokens[name] = tokenIndex;
                tokenIDs[tokenIndex] = labelIndex;
                labels.push_back(labelIndex);
            }
        }
        // get next token
        token = m_tokens.getToken();
    }
    
    // reset token stream index to
    m_tokens.reset();
}

int TParser::makeLabel(string &label) {
    int labelIndex = (int)this->labels.size();
    
    // at first, check to see wether the label is a symbol or a token
    if (isalpha(label[0])) {
        // if it's a symbol or a token
        // if the label is in symbol, just return it's index
        std::map<string, int>::iterator ite = this->symbolIDs.find(label);
        if (ite != this->symbolIDs.end()) {
            std::map<string, int>::iterator it = this->symbolToLabel.find(label);
            if (it != this->symbolToLabel.end()) {
                return this->symbolToLabel[label];
            }
            else {
                this->labels.push_back(this->symbolIDs[label]);
                this->symbolToLabel[label] = labelIndex;
                return labelIndex;
            }
        }
        
        // if the label is token
        else if (isupper(label[0])) {
            int tokenIndex = this->tokens[label];
            std::map<int, int>::iterator ite = this->tokenIDs.find(tokenIndex);
            if (ite != this->tokenIDs.end()) {
                return this->tokenIDs[tokenIndex];
            }
            else {
                this->labels.push_back(tokenIndex);
                this->tokenIDs[tokenIndex] = labelIndex;
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
        map<string, int>::iterator ite = this->keywordIDs.find(value);
        if (ite != this->keywordIDs.end()) {
            return this->keywordIDs[value];
        }
        else {
            this->labels.push_back(labelIndex);
            this->keywordIDs[value] = labelIndex;
            return labelIndex;
        }
    }
    else { // operator
        map<string, int>::iterator ite = this->operatormap.find(value);
        if (ite == this->operatormap.end()) {
            // exception
        }
        int tokenIndex = this->operatormap[value];
        map<int, int>::iterator it = this->tokenIDs.find(tokenIndex);
        if (it != this->tokenIDs.end())
            return this->tokenIDs[tokenIndex];
        else {
            this->labels.push_back(tokenIndex);
            this->tokenIDs[tokenIndex] = labelIndex;
            return labelIndex;
        }
    }
    // exception
    return -1;
}

void TParser::initializeFirstset() {   
    std::map<std::string, DFASet *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        std::pair<std::string, DFASet *> ip = *ite;
        string name = ip.first;
        if (m_first.find(name) != m_first.end()) {
          //  getFirstSet(name, ip.second);
        }
    }
   
}

vector<string>* TParser::makeFirstSet(string &name) {
    return NULL;
}

void TParser::getFirstSet(string &name, DFASet *dfas, vector<string> &newset) {
    vector<string> allLabels;
    map<string, vector<string> > overlaps;
    DFA *dfa = dfas->at(0);
    
    map<string, DFA *>::iterator ite;
    for (ite = dfa->m_arcs.begin(); ite != dfa->m_arcs.end(); ite++) {
        std::pair<string, DFA *> ip = *ite;
        string label = ip.first;
        
        // check to see wether the label
        if (m_dfas.find(label) != m_dfas.end()) {
            vector<string> *newLabels = NULL;
            
            if (m_first.find(label) != m_first.end()) {
                newLabels = &m_first[label];
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
        std::pair<string, vector<string> > ip = *it;
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
    m_first[name] = allLabels;
        
}



