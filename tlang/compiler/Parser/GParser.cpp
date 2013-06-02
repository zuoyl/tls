//
//  GrammarParser.cpp
//  A toyable language compiler (like a simple c++)

#include "GParser.h"
#include <assert.h>
#include <algorithm>
enum {
    TT_NONTERMINAL, // non-terminal
    TT_TERMINAL,    // termainal
    TT_STRING,      // keyword or operator 
    TT_OP,          // operator of grammar
};


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

GrammarParser::GrammarParser() 
{    
}

GrammarParser::~GrammarParser() 
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

void GrammarParser::dumpNFAs(const string &name, NFA *start, NFA *end)
{
    if (!start || !end) return;
    if (start == end) return;
    int index = 0; 
    vector<pair<string, NFA *> >::iterator ite = start->m_arcs.begin();
    for (; ite != start->m_arcs.end(); ite++) {
        pair<string, NFA*> &item = *ite;
        string label = item.first;
        NFA * nfa = item.second;
        if (nfa == end)
            break;
        string lv = (label.empty())?"null": label; 
        dbg("\t\t NFA(%d), label:%s\n", index++, lv.c_str()); 
    }
}
void GrammarParser::dumpDFA(DFA *dfa)
{
    if (!dfa) return;
    
    string final = (dfa->m_isFinal == true)?"true":"false";
    int arcs = (int)dfa->m_arcs.size();
    dbg("\t\t\tDFA: isFinal = %s, arcs = %d\n", final.c_str(), arcs); 

    int index = 0;
    map<string, DFA*>::iterator ite = dfa->m_arcs.begin();
    for (; ite != dfa->m_arcs.end(); ite++) {
        string label = ite->first;
        DFA *pdfa = ite->second;
        dbg("\t\t\tDFA(%d), label = %s, ptrDFA(0x%08x)\n", index++, label.c_str(), pdfa);
        dumpDFA(pdfa); 
    }
}
void GrammarParser::dumpDFAs(const string &name, vector<DFA *> &dfas)
{
    dbg("\tDFAS for rule  %s have %d dfa state\n", name.c_str(), (int)dfas.size()); 
    vector<DFA *>::iterator ite = dfas.begin();
    int index = 0;
    for (; ite != dfas.end(); ite++) {
        DFA * dfa = *ite;
        string final = (dfa->m_isFinal == true)?"true":"false"; 
        dbg("\t\tDFA(%d), isFinal = %s, arcs = %d\n", index++, final.c_str(), (int)dfa->m_arcs.size());
        dumpDFA(dfa); 
    }
}


bool GrammarParser::parseGrammarFile(const string & file) 
{
    bool controlFlag = false;
    int lineno = 0;
    Token *token = NULL;
    std::string atom = "";
    
    ifstream ifs;
    ifs.open(file.c_str(), ios::in);
    
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
                token->lineno = lineno;
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
                    token->lineno = lineno;
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
                        token->lineno = lineno;
                        m_tokens.pushToken(token);
                    }
                }
                break;
        }
    }
    ifs.close();
    m_tokens.dumpAllTokens();
    return true;
}

/// build the grammar file
void GrammarParser::build(const string &file, Grammar *grammar) 
{
    assert(grammar != NULL);
    m_grammar = grammar;
    
    string first;
    // parse the grammar file and get token stream
    parseGrammarFile(file);
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
        dbg("\tNFAs for rule %s\n", name.c_str()); 
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
            std::cout << "there are two same nonterminal in grammar file:" << name  << std::endl;
            delete dfaset;
        }
        if (m_grammar->m_firstNoTerminal.empty()) {
            m_grammar->m_firstNoTerminal = name;
            m_grammar->m_start = 0;
        }

	}
    // create the labels
    map<string, vector<DFA*>* >::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<std::string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        vector<DFA*> *dfaset = ip.second;
        
        // holder for all arcs
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
        m_grammar->m_nonterminalState[name] = (int)m_grammar->m_states.size();
        // place all DFAS into grammar's state table
        makeFirst(dfaset, name, &stateEntry.first);
        m_grammar->m_states.push_back(stateEntry);
        m_grammar->m_start = m_grammar->m_nonterminals[first];
    }   

}


void GrammarParser::stripLabel(string &label) 
{
    // we just want to strip the begin and end of label with a char "'"
    if (!label.empty() && label[0] == '\'')
        label.erase(std::remove(label.begin(), label.end(), '\''), label.end());
}   

void GrammarParser::advanceToken(Token **token) 
{
	m_tokens.advanceToken(token);
}

void GrammarParser::match(int type, Token **token) 
{
    if (!m_tokens.matchToken(type, token)) {
        throw NoMatchedTokenException(type);
    }
    
}
/// the next token must be matched with the specified token
void GrammarParser::match(int type, const char *name) 
{
    if(!m_tokens.matchToken(type, name)) {
        throw NoMatchedTokenException(name);
    }
}

/// check wether the next token is matched with specified token
bool GrammarParser::isMatch(int type, const char *name) 
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
void GrammarParser::parseRule(string &ruleName, NFA **start, NFA **end) 
{ 
    Token *token = NULL;
   
    match(TT_NONTERMINAL, &token);
    ruleName = token->assic;
    dbg("Parsing Rule[%s]...\n", ruleName.c_str());
    
    match(TT_OP, ":");
    parseAlternative(ruleName, start, end);
    match(TT_OP, ";");
}

/// parse the alternative, such as alternative : items (| items)*
void GrammarParser::parseAlternative(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Alternative for rule[%s]...\n", ruleName.c_str());
	assert(start != NULL);
	assert(end != NULL);
    // setup new state
    *start = new NFA();
    *end = new NFA();
    
    // parse items
    NFA *itemStartState = NULL;
    NFA *itemEndState = NULL;
    parseItems(ruleName, &itemStartState, &itemEndState);
    
    assert(itemStartState != NULL);
    assert(itemEndState != NULL);
    
    // connect the state
    (*start)->arc(itemStartState);
    itemEndState->arc(*end);

    while (isMatch(TT_OP, "|")) {
        advanceToken();
        itemStartState = NULL;
        itemEndState = NULL;
        parseItems(ruleName, &itemStartState, &itemEndState);
        
        (*start)->arc(itemStartState);
        itemEndState->arc(*end);
    }
}


/// parse the items, such as items : item+
void GrammarParser::parseItems(const string &ruleName, NFA **start, NFA **end) 
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
        NFA *itemStartState = NULL;
        NFA *itemEndState = NULL;
        parseItem(ruleName, &itemStartState, &itemEndState);
        
        // connect the state
        (*end)->arc(itemStartState);
        **end = *itemEndState;
    }
}


// item: ATOM('+'|'*'|'?')
void GrammarParser::parseItem(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Item for rule[%s]...\n", ruleName.c_str());
    parseAtom(ruleName, start, end);
    assert(start != NULL);
    assert(end != NULL);
   
    // check to see wether repeator exist?
    if (isMatch(TT_OP, "+")) {
        (*end)->arc(*start);
        advanceToken();
    } 
    else if (isMatch(TT_OP, "*")) {
        (*end)->arc(*start);
        advanceToken();
    }
    else if (isMatch(TT_OP, "?")) {
        (*start)->arc(*end);
        advanceToken();
        
    }
}
// atom: Nonterminal | Terminal | keyword | '(' atom ')'
void GrammarParser::parseAtom(const string &ruleName, NFA **start, NFA **end) 
{
    dbg("Parsing Atom for rule[%s]...\n", ruleName.c_str());
    if (isMatch(TT_OP, "(")) {
        advanceToken();
        parseAtom(ruleName, start, end);
        while (!isMatch(TT_OP, ")")) {
            NFA *subItemStart = NULL;
            NFA *subItemEnd = NULL;
            parseAtom(ruleName, &subItemStart, &subItemEnd);
            (*end)->arc(subItemStart);
            *end = subItemEnd;
        }

        match(TT_OP, ")");
        return;
    }
    
    else if (isMatch(TT_NONTERMINAL) || 
            isMatch(TT_TERMINAL) ||
            isMatch(TT_STRING)) {
        Token *token = NULL;
        advanceToken(&token);
        *start = new NFA();
        *end = new NFA();
        (*start)->arc(*end, token->assic);
        return;
    }
    
    else {
        Token *token = m_tokens.getToken();
        throw NoMatchedTokenException(token->type);
    }
}
    

/// initializeBuiltinIds
/// @brief initialized all buitin ids into maps, such as keyword, operator,terminals
void GrammarParser::initializeBuiltinIds() 
{
    Token *token = m_tokens.getToken();
    
    // iterate all tokens and get keywords and operators
    // keywords, operators, terminals all have lable index in DFA
    while (token != NULL) {
        
        int labelIndex = (int)m_grammar->m_labels.size();

        // if the token type is TT_STRING, it must be keyword and operator
        string name = token->assic;
        stripLabel(name);
      
        if (token->type == TT_STRING) {
            // keywords
            if (isalpha(name[0])) {
                if (m_grammar->m_keywords.find(name) == m_grammar->m_keywords.end()) {
                    m_grammar->m_keywords[name] = labelIndex;
                    m_grammar->m_labels.push_back(labelIndex);
                }
            }
        }
            // operator maps
        else if (token->type == TT_OP){
            if (m_grammar->m_operators.find(name) == m_grammar->m_operators.end()) {
                m_grammar->m_operators[name] = labelIndex;
                m_grammar->m_labels.push_back(labelIndex);
            }
        }
        // terminals, such as IDENTIFIER
        else if (token->type == TT_TERMINAL) {
            if (m_grammar->m_terminals.find(name) == m_grammar->m_terminals.end()) {
                m_grammar->m_terminals[name] = labelIndex;
                m_grammar->m_terminalName[labelIndex] = name;
                m_grammar->m_labels.push_back(labelIndex);
            }
        }
        // non-terminals
        else if (token->type == TT_NONTERMINAL) {
            if (m_grammar->m_nonterminals.find(name) == m_grammar->m_nonterminals.end()) {
                m_grammar->m_nonterminals[name] = labelIndex;
                m_grammar->m_nonterminalName[labelIndex] = name;
                m_grammar->m_labels.push_back(labelIndex);
            }
        }
        else {
            // do nothing
        }
        // get next token
        m_tokens.advanceToken();
        token = m_tokens.getToken();
    }
    
    // reset token stream index to
    m_tokens.reset();
}

int GrammarParser::makeLabel(string &label) 
{
    int labelIndex = (int)m_grammar->m_labels.size();
    
    // at first, check to see wether the label is terminal, keyword, operators
    // if the label is terminal
    if (isalpha(label[0]) && isupper(label[0])) {
        // get the label index by terminal ID
        map<string, int>::iterator ite = m_grammar->m_terminals.find(label);
        if (ite != m_grammar->m_terminals.end()) {
            return m_grammar->m_terminals[label];
        }
        else {
            // add a new label index in label set
            m_grammar->m_labels.push_back(labelIndex);
            m_grammar->m_terminals[label] = labelIndex;
            m_grammar->m_terminalName[labelIndex] = label;
            return labelIndex;
        }
    }
    if (isalpha(label[0])) {
        // if the label is keyword
       if (m_grammar->m_keywords.find(label) != m_grammar->m_keywords.end()) {
           return m_grammar->m_keywords[label];
         }
        // if the label is nonterminal
        map<string, int>::iterator ite = m_grammar->m_nonterminals.find(label);
        if (ite != m_grammar->m_nonterminals.end()) {
                return m_grammar->m_nonterminals[label];
        }
        else {
            m_grammar->m_labels.push_back(labelIndex);
            m_grammar->m_nonterminals[label] = labelIndex;
            return labelIndex;
        }
    }
    // if the label is operator
    if (m_grammar->m_operators.find(label) != m_grammar->m_operators.end()) {
        return m_grammar->m_operators[label];
    }
    return -1;
}

void GrammarParser::initializeFirstset() 
{  
/*
    map<string, vector<DFA*> *>::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        if (m_grammar->m_first.find(name) != m_grammar->m_first.end()) {
          //  getFirstSet(name, ip.second);
        }
    }
  */ 
}

void GrammarParser::makeFirst(vector<DFA*> *dfas, string &lable, vector<int> *firstSet) 
{
}

// get the state index of dfa in dfa set
int  GrammarParser::getStateIndex(vector<DFA*> *dfas, DFA *dfa)
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


void GrammarParser::getFirstSet(string &name, vector<DFA*> *dfas, vector<string> &newset)
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
                newLabels = &m_grammar->m_first[label];
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
    m_grammar->m_first[name] = allLabels;
#endif   
}

void GrammarParser::dumpAllBuiltinIds()
{
    std::cout << "#####------------Nonterminals--------------#####" << std::endl;
    map<string, int>::iterator itn = m_grammar->m_nonterminals.begin();
    for (; itn != m_grammar->m_nonterminals.end(); itn++) {
        std::cout << "\t" << itn->first << "\t\t\t" << itn->second << std::endl;
    }
    
    std::cout << "#####--------------Terminals--------------#####" << std::endl;
    map<string, int>::iterator itt = m_grammar->m_terminals.begin();
    for (; itt != m_grammar->m_terminals.end(); itt++) {
        std::cout << "\t" <<  itt->first << "\t\t\t" << itt->second << std::endl;
    }
    
    std::cout << "#####--------------Operators--------------#####" << std::endl;
    map<string, int>::iterator ito = m_grammar->m_operators.begin();
    for (; ito != m_grammar->m_operators.end(); ito++) {
        std::cout << "\t" << ito->first << "\t\t\t" << ito->second << std::endl;
    }

}
