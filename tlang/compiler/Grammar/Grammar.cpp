//
//  Grammar.cpp
//  A toyable language compiler (like a simple c++)

#include "Grammar.h"
#include <assert.h>
#include <algorithm>
#include <stack>

enum {
    TT_NONTERMINAL, // non-terminal
    TT_TERMINAL,    // termainal
    TT_STRING,      // keyword or operator 
    TT_OP,          // operator of grammar
};

bool Grammar::m_isInitialized = false;
const string Grammar::TerminalIdentifier = "IDENTIFIER";
const string Grammar::TerminalNumber =   "NUMBER";
const string Grammar::TerminalString  = "STRING";
const string Grammar::TerminalHexNumber = "HEX_NUMBER";


static void dbgprint(const char *fmt, ...)
{
    char buf[256] = {0};
    va_list list;
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << buf;
    va_end(list);
}

#define TDEBUG

#ifdef TDEBUG
#define dbg dbgprint
#else
#define dbg
#endif


Grammar::Grammar()
{
}
Grammar::~Grammar()
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
Grammar& Grammar::getInstance()
{
    static Grammar grammar;
    return grammar;
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

void Grammar::dumpNFAs(const string &name, NFA *start, NFA *end)
{
    if (!start || !end) return;
    // A nfa set is a DAG, so stack must be used
    
    dbg("NFAs for rule(%s)\n", name.c_str()); 
    vector<NFA *> nfas;
    nfas.push_back(start);
    
    for (size_t i = 0; i < nfas.size(); i++) {
       NFA *nfa = nfas.at(i); 
       dbg("\tNFA(%d), arc count = %d\n", nfa->m_index, nfa->m_arcs.size());
       vector<pair<string, NFA*> >::iterator ite = nfa->m_arcs.begin();
       int index = 0;
       for (; ite != nfa->m_arcs.end(); ite++) {
            pair<string, NFA*> item = *ite;
            string label = item.first;
            NFA *subnfa = item.second;
            string lv = (label.empty())?"null":label;
            if (subnfa)
                dbg("\t\t arc(%d): label = %s, arc to = %d\n", index++, lv.c_str(), subnfa->m_index);
            else
                dbg("\t\t arc(%d): label = %s, arc to = invalid\n", index++, lv.c_str());
            // if the nfa is not in stack, push it
            size_t j = 0;
            for (; j < nfas.size(); j++) {
                NFA *tnfa = nfas.at(j);
                if (subnfa == tnfa)
                    break;
            }
            if (j == nfas.size())
                nfas.push_back(subnfa);
       }
    }
}
void Grammar::dumpDFAs(const string &name, vector<DFA *> &dfas)
{
    dbg("DFAS for rule  %s have %d dfa state\n", name.c_str(), (int)dfas.size()); 
    vector<DFA *>::iterator ite = dfas.begin();
    int index = 0;
    for (; ite != dfas.end(); ite++) {
        DFA *dfa = *ite;
        string final = (dfa->m_isFinal == true)?"true":"false"; 
        dbg("\tDFA(%d), final = %s, arc count = %d\n", index++, final.c_str(), (int)dfa->m_arcs.size());
        map<string, DFA*>::iterator i = dfa->m_arcs.begin();
        int sindex = 0; 
        for (; i != dfa->m_arcs.end(); i++) {
            string label = i->first;
            DFA *subdfa = i->second;
            string lv = (label.empty())?"null":label;
            if (subdfa) 
                dbg("\t\tarc(%d): label = %s, arc to = %d\n", sindex++, lv.c_str(), subdfa->m_index);
            else
                dbg("\t\tarc(%d): label = %s, arc to = invalid\n", sindex++, lv.c_str());
        }
    }
}


bool Grammar::parseGrammarFile(const string & file) 
{
    bool controlFlag = false;
    int lineno = 0;
    Token *token = NULL;
    std::string atom = "";
    
    ifstream ifs;
    ifs.open(file.c_str(), ios::in);
    if (!ifs.is_open()) {
        dbg("can not open the grammra file:%s\n", file.c_str());   
        return false;
    }
    while (!ifs.eof()) {
        char ch = ifs.get();
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
                token->location = lineno;
                m_tokens.pushToken(token);
                break;
                
            case '\'':
                // indicate a string
                atom = "";
                while ((ch = ifs.get()) != EOF) {
                    if (ch == '\'') 
                        break;
                    else
                        atom += ch;
                }
                if (!atom.empty()) {
                    token = new Token();
                    token->assic = atom;
                    token->type = TT_STRING;
                    token->location = lineno;
                    m_tokens.pushToken(token);
                }
                break;
                
            case '/':
                // comments consume all line
                while ((ch = ifs.get()) != EOF) {
                    if (ch == '\r' || ch == '\n')
                        break;
                }
                controlFlag = false;
                lineno++;
                break;
            case '\r':
            case '\n':
                lineno++;
                break;
                
            default:
                if (isalpha(ch)) {
                    atom = "";
                    atom += ch;
                    while ((ch = ifs.get()) != EOF) {
                        if (isalpha(ch)) 
                            atom += ch;
                        else {
                            ifs.unget();
                            break;
                        }
                    }
                    // get a token
                    if (!atom.empty()) {
                        token = new Token();
                        if (isupper(atom[0]))
                            token->type = TT_TERMINAL;
                        else
                            token->type = TT_NONTERMINAL;
                        token->assic = atom;
                        token->location = lineno;
                        m_tokens.pushToken(token);
                    }
                }
                break;
        }
    }
    ifs.close();
    // m_tokens.dumpAllTokens();
    return true;
}

/// build the grammar file
bool Grammar::build(const string &fullFileName) 
{
    // the grammar file should be only build once  
    if (m_isInitialized == true) 
        return true; 
    m_isInitialized = true; 
    
    // parse the grammar file and get token stream
    parseGrammarFile(fullFileName);
    // initialize the builtin ids
    initializeBuiltinIds();
    // dumpAllBuiltinIds();

    // parse the all tokens to get DFAs
    while (true) {
        // get ahead token from token stream
        Token *token = m_tokens.getToken();
        if (!token) {
            dbg("grammar file parse is finished\n");
            break;
        }

        // parse a rule and get the NFAs
        NFA *start = NULL;
        NFA *end = NULL;
        string name;
        parseRule(name, &start, &end);
        // dump all nfa state for the rule to debug
        dumpNFAs(name, start, end);    
        // create a dfa accroding to the rule
        vector<DFA *> *dfaset = convertNFAToDFA(start, end);
        // dump all dfa state for the rule to debug
        simplifyDFAs(name, *dfaset);
        dumpDFAs(name, *dfaset);

        // save the dfa by name and first nonterminal
        // till now, the first nontermianl is start
        if (m_dfas.find(name) == m_dfas.end())
            m_dfas[name] = dfaset;
        else {
            dbg("there are two same nonterminal in grammar file %s\n", name.c_str());
            delete dfaset;
        }
        if (m_firstNonterminal.empty()) {
            m_firstNonterminal = name;
        }

	}
    
    // after all rules are parsed, create the labels for all DFA
    map<string, vector<DFA*>* >::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<std::string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        vector<DFA*> *dfaset = ip.second;
        
        Grammar::StateEntry stateEntry;
        
        // for each DFA
        vector<DFA *>::iterator it;
        for (it = dfaset->begin(); it != dfaset->end(); it++ ) {
            DFA *dfa = *it;
            
            Grammar::State state;
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
        // save the nonterminal name and state maping
        m_nonterminalState[name] = (int)m_states.size();
        // place all DFAS into grammar's state table
        makeFirst(dfaset, name, &stateEntry.first);
        m_states.push_back(stateEntry);
    }   
    m_start = m_nonterminals[m_firstNonterminal];
    return true;
}


void Grammar::stripLabel(string &label) 
{
    // we just want to strip the begin and end of label with a char "'"
    if (!label.empty() && label[0] == '\'')
        label.erase(std::remove(label.begin(), label.end(), '\''), label.end());
}   

void Grammar::advanceToken(Token **token) 
{
	m_tokens.advanceToken(token);
}

void Grammar::match(int type, Token **token) 
{
    if (!m_tokens.matchToken(type, token)) {
        throw Exception::NoMatchedToken();
    }
    
}
/// the next token must be matched with the specified token
void Grammar::match(int type, const char *name) 
{
    if(!m_tokens.matchToken(type, name)) {
        throw Exception::NoMatchedToken(name);
    }
}

/// check wether the next token is matched with specified token
bool Grammar::isMatch(int type, const char *name) 
{
    Token *token = m_tokens.getToken();
    if (token != NULL && token->type == type) { 
        if (name && token->assic != name)
            return false;
        return true;
    }        
    return false;
}

/// parse a rule, such as production: alternative 
void Grammar::parseRule(string &ruleName, NFA **start, NFA **end) 
{ 
    Token *token = NULL;
   
    match(TT_NONTERMINAL, &token);
    ruleName = token->assic;
    dbg("\nParsing Rule[%s]...\n", ruleName.c_str());
    
    match(TT_OP, ":");
    parseAlternative(ruleName, start, end);
    match(TT_OP, ";");
}

/// parse the alternative, such as alternative : items (| items)*
void Grammar::parseAlternative(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Alternative for rule[%s]...\n", ruleName.c_str());
	assert(start != NULL);
	assert(end != NULL);
    // parse items
    parseItems(ruleName, start, end);
   
    if (isMatch(TT_OP, "|")) {
        // make a closing state 
        NFA *closingStartState = new NFA();
        NFA *closingEndState = new NFA;
        closingStartState->arc(*start);
        (*end)->arc(closingEndState);

        while (isMatch(TT_OP, "|")) {
            advanceToken();
            NFA *startState = NULL;
            NFA *endState = NULL;
            parseItems(ruleName, &startState, &endState);
            closingStartState->arc(startState);
            endState->arc(closingEndState); 
        }
        *start = closingStartState;
        *end = closingEndState;
    }
}


/// parse the items, such as items : item+
void Grammar::parseItems(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Items for rule[%s]...\n", ruleName.c_str());
    // setup new state
    parseItem(ruleName, start, end);
    assert(*start != NULL);
    assert(*end != NULL);
    
    while (isMatch(TT_NONTERMINAL) || 
            isMatch(TT_TERMINAL) || 
            isMatch(TT_STRING) ||
            isMatch(TT_OP, "(")) {
        // parse item
        NFA *startState = NULL;
        NFA *endState = NULL;
        parseItem(ruleName, &startState, &endState);
        
        // connect the state
        (*end)->arc(startState);
        *end = endState;
    }
}


// item: ATOM('+'|'*'|'?')
void Grammar::parseItem(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Item for rule[%s]...\n", ruleName.c_str());
    parseAtom(ruleName, start, end);
    assert(*start != NULL);
    assert(*end != NULL);
   
    // check to see wether repeator exist?
    if (isMatch(TT_OP, "+")) {
        (*end)->arc(*start);
        advanceToken();
    } 
    else if (isMatch(TT_OP, "*")) {
        NFA *endState = new NFA(); 
        (*end)->arc(*start);
        (*end)->arc(endState); 
        (*start)->arc(endState); 
        advanceToken();
        *end = endState;
    }
    else if (isMatch(TT_OP, "?")) {
        NFA *endState = new NFA(); 
        (*end)->arc(endState);
        (*start)->arc(endState); 
        *end = endState;
        advanceToken();
    }
}
// atom: Nonterminal | Terminal | keyword | '(' atom ')'
void Grammar::parseAtom(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Atom for rule[%s]...\n", ruleName.c_str());
    if (isMatch(TT_OP, "(")) {
        advanceToken();
        parseAlternative(ruleName, start, end);
        match(TT_OP, ")");
    }
    else if (isMatch(TT_NONTERMINAL) || 
            isMatch(TT_TERMINAL) ||
            isMatch(TT_STRING)) {
        Token *token = NULL;
        advanceToken(&token);
        *start = new NFA();
        *end = new NFA();
        (*start)->arc(*end, token->assic);
    }
    else {
        Token *token = m_tokens.getToken();
        throw Exception::NoMatchedToken(token->assic);
    }
}
    

/// initializeBuiltinIds
/// @brief initialized all buitin ids into maps, such as keyword, operator,terminals
void Grammar::initializeBuiltinIds() 
{
    Token *token = m_tokens.getToken();
    
    // iterate all tokens and get keywords and operators
    // keywords, operators, terminals all have lable index in DFA
    while (token != NULL) {
        // all labels are placed into m_labes vector 
        int labelIndex = (int)m_labels.size();

        // if the token type is TT_STRING, it must be keyword and operator
        string name = token->assic;
        stripLabel(name);
      
        if (token->type == TT_STRING) {
            // keywords
            if (isalpha(name[0])) {
                if (m_keywords.find(name) == m_keywords.end()) {
                    m_keywords[name] = labelIndex;
                    m_labels.push_back(labelIndex);
                }
            }
        }
        // operator maps
        else if (token->type == TT_OP){
            if (m_operators.find(name) == m_operators.end()) {
                m_operators[name] = labelIndex;
                m_labels.push_back(labelIndex);
            }
        }
        // terminals, such as IDENTIFIER
        else if (token->type == TT_TERMINAL) {
            if (m_terminals.find(name) == m_terminals.end()) {
                m_terminals[name] = labelIndex;
                m_terminalName[labelIndex] = name;
                m_labels.push_back(labelIndex);
            }
        }
        // non-terminals
        else if (token->type == TT_NONTERMINAL) {
            if (m_nonterminals.find(name) == m_nonterminals.end()) {
                m_nonterminals[name] = labelIndex;
                m_nonterminalName[labelIndex] = name;
                m_labels.push_back(labelIndex);
            }
        }
        else {
           dbg("unknown token (%d,%s) in grammar file\n", 
                   token->location.getLineno(), token->assic.c_str()); 
        }
        // get next token
        m_tokens.advanceToken();
        token = m_tokens.getToken();
    }
   
    // after the grammar file is parsed, the tokens should be released
    m_tokens.clear();
}

// after all dfas are created, the labes must be assiged
int Grammar::makeLabel(string &label) 
{
    int labelIndex = m_labels.size();
    
    // first, check to see wether the label is terminal, keyword, operators
    // if the label is terminal
    if (isalpha(label[0]) && isupper(label[0])) {
        // get the label index by terminal ID
        map<string, int>::iterator ite = m_terminals.find(label);
        if (ite != m_terminals.end()) 
            return m_terminals[label];
        else {
            // add a new label index in label set
            m_labels.push_back(labelIndex);
            m_terminals[label] = labelIndex;
            m_terminalName[labelIndex] = label;
            return labelIndex;
        }
    }
    if (isalpha(label[0])) {
        // if the label is keyword
        if (m_keywords.find(label) != m_keywords.end())
           return m_keywords[label];
        // if the label is nonterminal
        map<string, int>::iterator ite = m_nonterminals.find(label);
        if (ite != m_nonterminals.end())
            return m_nonterminals[label];
        else {
            m_labels.push_back(labelIndex);
            m_nonterminals[label] = labelIndex;
            return labelIndex;
        }
    }
    // if the label is operator
    if (m_operators.find(label) != m_operators.end()) {
        return m_operators[label];
    }
    return -1;
}

void Grammar::initializeFirstset() 
{  
/*
    map<string, vector<DFA*> *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        if (m_first.find(name) != m_first.end()) {
          //  getFirstSet(name, ip.second);
        }
    }
  */ 
}

void Grammar::makeFirst(vector<DFA*> *dfas, string &lable, vector<int> *firstSet) 
{
}

// get the state index of dfa in dfa set
int  Grammar::getStateIndex(vector<DFA*> *dfas, DFA *dfa)
{
    int index = -1;
    vector<DFA*>::iterator ite = dfas->begin();
    for (;  ite != dfas->end(); ite++) {
        if (*ite == dfa) {
            index++;
            break;
        }
        index++;
    }
    return index;
}


void Grammar::getFirstSet(string &name, vector<DFA*> *dfas, vector<string> &newset)
{
#if 0
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
    m_first[name] = allLabels;
#endif   
}

void Grammar::dumpAllBuiltinIds()
{
    std::cout << "#####------------Nonterminals--------------#####" << std::endl;
    map<string, int>::iterator itn = m_nonterminals.begin();
    for (; itn != m_nonterminals.end(); itn++) {
        std::cout << "\t" << itn->first << "\t\t\t" << itn->second << std::endl;
    }
    
    std::cout << "#####--------------Terminals--------------#####" << std::endl;
    map<string, int>::iterator itt = m_terminals.begin();
    for (; itt != m_terminals.end(); itt++) {
        std::cout << "\t" <<  itt->first << "\t\t\t" << itt->second << std::endl;
    }
    
    std::cout << "#####--------------Operators--------------#####" << std::endl;
    map<string, int>::iterator ito = m_operators.begin();
    for (; ito != m_operators.end(); ito++) {
        std::cout << "\t" << ito->first << "\t\t\t" << ito->second << std::endl;
    }

}


