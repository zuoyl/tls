//
//  Grammar.cpp
//  A toyable language compiler (like a simple c++)

#include "Grammar.h"
#include <assert.h>
#include <algorithm>
#include <stack>

enum {
    TT_NONTERMINAL, // non-terminal
    TT_ID,    // identifier 
    TT_STRING,      // keyword or operator 
    TT_OP,          // operator of grammar
};

bool Grammar::m_isInitialized = false;

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

bool Grammar::isKeyword(const string &w)
{
    return (m_keywords.find(w) != m_keywords.end());
}

GrammarStates* Grammar::getStates(int index)
{
    if (index < m_states.size())
        return &m_states[index];
    else
        return NULL;
}

bool Grammar::isNonterminal(int id)
{
    return (m_nonterminalName.find(id) != m_nonterminalName.end());
}

bool Grammar::isTerminal(int id)
{
    return (m_terminalName.find(id) != m_nonterminalName.end());
}

bool Grammar::isKeyword(int id)
{
    return (m_keywordName.find(id) != m_keywordName.end());
}

int Grammar::getSymbolID(int kind, const string &name)
{
   switch (kind) {
       case Terminal:
           if (m_terminals.find(name) != m_terminals.end())
               return m_terminals[name];
           break;
       case Nonterminal:
           if (m_nonterminals.find(name) != m_nonterminals.end())
               return m_nonterminals[name];
           break;
       default:
           break;
   }
   return -1;
}


// get symbol name by index
void Grammar::getSymbolName(int label, string &name)
{
    if (m_symbolName.find(label) != m_symbolName.end())
        name = m_symbolName[label];
}

int Grammar::getSymbolID(const string &name)
{
    if (m_symbols.find(name) != m_symbols.end())
        return m_symbols[name];
    else
        return -1;
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
                            token->type = TT_ID;
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
        if (m_dfas.find(name) == m_dfas.end()) {
            m_dfas[name] = dfaset;
        } 
        else {
            dbg("there are two same nonterminal in grammar file %s\n", name.c_str());
            delete dfaset;
        }
        if (m_firstNonterminal.empty()) {
            m_firstNonterminal = name;
        }
	}
    // after all rule had been parsed, the parse table should be constructed
    map<string, vector<DFA*>* >::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        vector<DFA*> *dfaset = ip.second;
        
        makeStateTableForNonterminal(name, *dfaset);
    }
    // save the nonterminal name and state maping
    m_start = m_nonterminals[m_firstNonterminal];
}

void Grammar::makeStateTableForNonterminal(const string &name, vector<DFA *> &dfas)
{
    // get label for the rule name,it is nonterminal
    int nonterminalIndex = getSymbolID(Nonterminal, name);
    if (nonterminalIndex < 255) {
        dbg("the label is invalid for nonterminal %s\n", name.c_str());
        return;
    }
    if (m_states.find(nonterminalIndex) != m_states.end()) {
        GrammarStates &states = m_states[nonterminalIndex];
        // convert each dfa to a grammar state
        vector<DFA *>::iterator ite = dfas.begin();
        for (; ite != dfas.begin(); ite++) {
            DFA *dfa = *ite;
            GrammarState state;
            state.isFinal = dfa->m_isFinal;
            map<string, DFA*>::iterator i = dfa->m_arcs.begin();
            for (; i != dfa->m_arcs.end(); i++) {
                string label = i->first;
                DFA *subdfa = i->second;
                int nlabel = makeLabel(label);
                int next = -1; 
                for (size_t j = 0; j < dfas.size(); j++) {
                    if (dfas[j] == subdfa) {
                        next = j;
                        break;
                    }
                }
                state.arcs.insert(make_pair(nlabel, next)); 
            }
        }
    }
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
            isMatch(TT_ID) || 
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
            isMatch(TT_ID) ||
            isMatch(TT_STRING)) {
        Token *token = NULL;
        advanceToken(&token);
        *start = new NFA();
        *end = new NFA();
        (*start)->arc(*end,token->assic);
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
      
        switch (token->type) {
            case TT_STRING:
                // keywords
                if (isalpha(name[0])) {
                    if (m_keywords.find(name) == m_keywords.end()) {
                        // update symbols
                        m_symbols[name] = labelIndex;
                        m_symbolName[labelIndex] = name;
                        // update keywords
                        m_keywords[name] = labelIndex;
                        m_labels.push_back(labelIndex);
                        // update the terminal
                        m_terminals.insert(make_pair(name, labelIndex));
                        m_terminalName.insert(make_pair(labelIndex, name));
                    }
                }
                break;
            // operator maps
            case TT_OP:
                if (m_operators.find(name) == m_operators.end()) {
                    // update symbols
                    m_symbols[name] = labelIndex;
                    m_symbolName[labelIndex] = name;
                    // update operators
                    m_operators[name] = labelIndex;
                    m_labels.push_back(labelIndex);
                    // update the terminal
                    m_terminals.insert(make_pair(name, labelIndex));
                    m_terminalName.insert(make_pair(labelIndex, name));
                }
                break;

            // terminals, such as IDENTIFIER
            case TT_ID:
                if (m_terminals.find(name) == m_terminals.end()) {
                    // update symbols
                    m_symbols[name] = labelIndex;
                    m_symbolName[labelIndex] = name;
                    // update terminals 
                    m_terminals[name] = labelIndex;
                    m_terminalName[labelIndex] = name;
                    m_labels.push_back(labelIndex);
                }
                break; 
            // non-terminals
            case TT_NONTERMINAL:
                if (m_nonterminals.find(name) == m_nonterminals.end()) {
                    labelIndex += 256; 
                    // update symbols 
                    m_symbols[name] = labelIndex;
                    m_symbolName[labelIndex] = name;
                    // update nonterminals
                    m_nonterminals[name] = labelIndex;
                    m_nonterminalName[labelIndex] = name;
                    m_labels.push_back(labelIndex);
                }
                break; 
            default:
                dbg("unknown token (%d,%s) in grammar file\n", 
                   token->location.getLineno(), token->assic.c_str()); 
                break;
        }
        // get next token
        m_tokens.advanceToken();
        token = m_tokens.getToken();
    }
    m_tokens.reset();
}

// after all dfas are created, the labes must be assiged
int Grammar::makeLabel(string &label) 
{
    int labelIndex = m_labels.size();
   
    if (m_symbols.find(label) !=  m_symbols.end()) 
        return m_symbols[label];
    else {
        m_symbols[label] = labelIndex;
        m_symbolName[labelIndex] = label;
        m_labels.push_back(labelIndex); 
        return labelIndex;
    }

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


