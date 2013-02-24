//
//  Grammar.cpp
//  A toyable language compiler (like a simple c++)

#include "compiler/Common.h"
#include "compiler/Grammar.h"
#include "compiler/TokenStream.h"
#include "compilerException.h"



enum {
    TT_NAME,
    TT_STRING,
    TT_OP,
    TT_TOKEN,
    TT_ENDMARK,
    TT_NEWLINE
};

NFA::NFA()
{
    
}

NFA::~NFA()
{
    
}

void NFA::arc(NFA *to, const std::string &label)
{
    std::pair<string, NFA *> item;
    item.first = label;
    item.second = to;
    m_arcs.push_back(item);
}

void NFA::arc(NFA *to, const char *label)
{
    std::pair<string, NFA*> item;
    if (!label)
        item.first = string("");
    else 
        item.first = label;
    item.second = to;
    m_arcs.push_back(item);
}

NFA& NFA::operator=(NFA &rhs)
{
    // delete all arcs  and insert the new arcs
    // dummy
    return *this;
    
}

void NFA::findUnlabeldState(NFASet &nfaset)
{
    // check to see wether myself is in the state
    NFASet::iterator ite;
    for (ite = nfaset.begin(); ite < nfaset.end(); ite++) {
        if (this == *ite) 
            return;
    }
    // add myself into the set
    nfaset.push_back(this);
    
    std::vector<std::pair<std::string, NFA *> >::iterator it;
    
    for (it = m_arcs.begin(); it < m_arcs.end(); it++) {
        std::pair<string, NFA *> ip = *it;
        if (!ip.first.empty())
            ip.second->findUnlabeldState(nfaset);
    }
}


DFA::DFA(NFASet &nfaset, NFA *finalState)
{
    m_nfas = nfaset;
    NFASet::iterator ite = nfaset.begin();
    for (; ite < nfaset.end(); ite++) {
        if (finalState == *ite) {
            m_isFinal = true;
            break;
        }
    }
}
DFA::~DFA()
{
}

void DFA::arc(DFA *to, string &label)
{
    m_arcs[label] = to;
}


bool DFA::operator == (DFA &rhs) 
{
    if (rhs.m_isFinal != m_isFinal)
        return false;
    if (rhs.m_first != m_first)
        return false;
    if (rhs.m_arcs.size() != rhs.m_arcs.size())
        return false;
    
    std::map<std::string, DFA*>::iterator ite;
    for (ite = m_arcs.begin(); ite != m_arcs.end(); ite++) {
        if (m_arcs[ite->first] != rhs.m_arcs[ite->first])
            return false;
    }
    
    return true;
}
bool isSameNFASet(NFASet &nfas1, NFASet &nfas2)
{
    if (nfas1.size() != nfas2.size())
        return false;
    
    for (int i = 0; i < nfas1.size(); i++) {
        if (nfas1[i] != nfas2[i])
            return false;
    }
    return true;
}

DFASet* convertNFAToDFA(NFA *start, NFA *end)
{
    NFASet baseNFAs;
    start->findUnlabeldState(baseNFAs);
    
    DFASet *stack = new std::vector<DFA *>();
    stack->push_back(new DFA(baseNFAs, end));
    
    // iterate the stack
    for (int i = 0; i < stack->size(); i++) {
        
        DFA *state = stack->at(i);
        std::vector<NFA *> &nfas = state->m_nfas;
        std::vector<NFA *>::iterator ite;
        
        // holder for arcs
        std::vector< std::pair<std::string, NFASet *> > arcs;;
        
        // iterate current DFA
        for (ite = nfas.begin(); ite < nfas.end(); ite++) {
            NFA * nfa = *ite;
            // for each NFA
            for (int arcIndex = 0; arcIndex < nfa->m_arcs.size(); arcIndex++) {
                std::pair<std::string, NFA *> ip = nfa->m_arcs[arcIndex];
                if (!ip.first.empty()) {
                    NFASet * nfaset = new NFASet();
                    arcs.push_back(make_pair(ip.first, nfaset));
                    ip.second->findUnlabeldState(*nfaset);
                }
            }
        }
        
        // for all arcs
        std::vector< std::pair<std::string, NFASet *> >::iterator it;
        for (it = arcs.begin(); it != arcs.end(); it++) {
            std::string label = (*it).first;
            NFASet *nfaset = (*it).second;
            // check to see wether the state is in stack
            int i = 0;
            for (; i < stack->size(); i++) {
                if (isSameNFASet(stack->at(i)->m_nfas, *nfaset))
                    break;
            }
            if (i > stack->size()) {
                DFA * newState = new DFA(*nfaset, end);
                stack->push_back(newState);
                state->arc(newState, label);
            }
        }
    }
    return stack;
}

void simplifyDFA(const std::string &name, DFA *)
{
    
}




void Grammar::stripLabel(string &label, const char *chars, string &newLabel)
{
    if (chars) {
        newLabel = label.replace(label.begin(), label.end(), chars, "");
    }
}

Token* Grammar::advanceToken()
{
    return m_tokens.getToken();
}

int Grammar::getStateIndex(DFASet *dfas, DFA *dfa)
{
    return 0; 
}

void Grammar::expectToken(int type, const char*name)
{
    if (m_tokens.matchToken(type, name)) {
        throw Exception::NoMatchedToken(name);
    }
    
}

bool Grammar::parseGrammarFile(const char *file)
{
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


void Grammar::initializeBuiltinIds()
{
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

int Grammar::makeLabel(string &label)
{
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

void Grammar::initializeFirstset()
{   
    std::map<std::string, DFASet *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        std::pair<std::string, DFASet *> ip = *ite;
        string name = ip.first;
        if (m_first.find(name) != m_first.end()) {
          //  getFirstSet(name, ip.second);
        }
    }
   
}

vector<string>* Grammar::makeFirstSet(string &name)
{
    return NULL;
}

void Grammar::getFirstSet(string &name, DFASet *dfas, vector<string> &newset)
{
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

void Grammar::build(const char *file)
{
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
        DFASet *dfaset = convertNFAToDFA(start, end);
        //simplifyDFA(name, dfa);
        m_dfas[name] = dfaset;
        
        if (first.empty())
            first = name;
    }
    
    // symbol and id mapping
    std::map<std::string, DFASet *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        std::pair<std::string, DFASet *> ip = *ite;
        name = ip.first;
        int index = (int)this->symbolIDs.size() + 255;
        this->symbolIDs[name] = index;
        this->symbolNames[index] = name;
    }
    
    
    // create the labels
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        std::pair<std::string, DFASet *> ip = *ite;
        name = ip.first;
        DFASet *dfaset = ip.second;
        
        // holder for all arcs
        GrammarStateEntry stateEntry;
        
        // for each DFA
        DFASet::iterator it;
        for (it = dfaset->begin(); it != dfaset->end(); it++ ) {
            DFA *dfa = *it;
            
            GrammarState state;
            // get all arcs for the dfa
            std::map<std::string, DFA *>::iterator iac;
            for (iac = dfa->m_arcs.begin(); iac != dfa->m_arcs.end(); iac++) {
                std::pair<std::string, DFA *> ipc = *iac;
                std::string label = ipc.first;
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
        this->states.push_back(stateEntry);
        this->start = this->symbolIDs[first];
    }   

}


bool Grammar::match(int type, const char *name)
{
    return m_tokens.matchToken(type, name);
}

// production: alternative 
void Grammar::parseRule(string &name, NFA **start, NFA **end)
{   
    Token *token = advanceToken();
    if (token != NULL && token->type == TT_NAME) {
        name = token->assic;
        
        match(TT_OP, ":");
        parseAlternatie(start, end);
        match(TT_OP, ";");
    }
}



// alternative : items (| items)*
void Grammar::parseAlternatie(NFA **start, NFA **end)
{    
    // setup new state
    *start = new NFA();
    *end = new NFA();
    
    // parse items
    NFA *subStartState = NULL;
    NFA *subCloseState = NULL;
    parseItems(&subStartState, &subCloseState);
    
    // connect the state
    (*start)->arc(subStartState);
    subCloseState->arc(*end);
    **end = *subCloseState;
    
    while (match(TT_OP, "|")) {
        advanceToken();
        
        NFA *subStartState = NULL;
        NFA *subCloseState = NULL;
        
        parseItems(&subStartState, &subCloseState);
        
        (*start)->arc(subStartState);
        subCloseState->arc(*end);
        **end = *subCloseState;
    }
}


// items : items+
void Grammar::parseItems(NFA **start, NFA **end)
{
    // setup new state
    parseItem(start, end);
    
    while (match(TT_NAME) || match(TT_STRING) || match(TT_OP, "[") || match(TT_OP, "(")) {
        // parse item
        NFA *subStartState = NULL;
        NFA *subCloseState = NULL;
        parseItem(&subStartState, &subCloseState);
        
        // connect the state
        (*end)->arc(subStartState);
        **end = *subCloseState;
    }
    
}


// item: ATOM('+'|'*'|'?')
void Grammar::parseItem(NFA **start, NFA **end)
{
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
        // do nothing
    }
}

void Grammar::parseAtom(NFA **start, NFA **end)
{
    if (match(TT_OP, "(")) {
        advanceToken();
        parseAtom(start, end);
        expectToken(TT_OP, ")");
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
        // throw exception
    }
}
    




