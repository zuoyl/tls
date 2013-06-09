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
    m_xmlDoc = xmlNewDoc(BAD_CAST "1.0");
    m_xmlRootNode = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(m_xmlDoc, m_xmlRootNode);
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
    xmlFreeDoc(m_xmlDoc);
    xmlCleanupParser();
    xmlMemoryDump();
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
        return m_states[index];
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
    
    dbg(">NFAs for rule(%s):\n", name.c_str()); 
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
    dbg(">DFAs for rule(%s):\n", name.c_str()); 
    vector<DFA *>::iterator ite = dfas.begin();
    int index = 0;
    for (; ite != dfas.end(); ite++) {
        DFA *dfa = *ite;
        string final = (dfa->m_isFinal == true)?"true":"false"; 
        dbg("\tDFA(%d), final = %s, arc count = %d\n", dfa->m_index, final.c_str(), (int)dfa->m_arcs.size());
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


bool Grammar::isFirstSymbol(DFA *dfa, int symbol)
{
    if (!dfa) return false; 
    // check wether the symbol is in the first of dfa
    map<string, DFA *>::iterator ite = dfa->m_arcs.begin();
    for (; ite != dfa->m_arcs.end(); ite++) {
        string label = ite->first;
        DFA *next = ite->second;
        if (m_symbols.find(label) == m_symbols.end()) {
            dbg("the symbol(%s) can not be recognized\n", label.c_str());
            return false;
        }
        int symbolOfLabel = m_symbols[label];
        if (symbolOfLabel == symbol)
            return true;
        if (isNonterminal(symbolOfLabel))
            return isFirstSymbol(next, symbol);
    }
    return false;
}

// first set is used to select nonterminal or production to apply
void Grammar::makeFirst(const string &name, DFA* dfa, vector<int> &result)
{
    if (!dfa)
        return;
    // name is nonterminal name
    // dfas is dfa for the nonterminal or production
    // for each grammar terminal symbol, if the state can accept the symbol
    // the dfa[0] is always the first state for the production 
    map<string, int>::iterator ite = m_terminals.begin();
    for (; ite != m_terminals.end(); ite++) {
        int symbol = ite->second;
        map<string, DFA*>::iterator m = dfa->m_arcs.begin();
        for (; m != dfa->m_arcs.end(); m++) {
            string label = m->first;
            // if the label is null(epsilon), add it to result
            if (label.empty()) {
                result.push_back(symbol);
                break;
            }
            int labelIndex = -1;
            if (m_symbols.find(label) != m_symbols.end())
                labelIndex = m_symbols[label];
            if (labelIndex < 0) {
                dbg("the label(%s) can not be recognized\n", label.c_str());
                return; 
            }
            // if the label is nontermial 
            if (isNonterminal(labelIndex)) {
                string nonterminal = m_nonterminalName[labelIndex];
                vector<DFA *> *dfas = m_dfas[nonterminal];
                if (!dfas) { 
                    dbg("can not get the dfas for nonterminal:%s\n", nonterminal.c_str());
                    return;
                }
                if (isFirstSymbol(dfas->at(0), symbol))
                    result.push_back(symbol);
            }
            // if the symbol is accepted, just look next symbol
            if (labelIndex == symbol) {
                result.push_back(symbol);
                break; 
            }
        }
    }

}
void Grammar::makeFollow(const string &name, DFA *dfa, vector<int> &result)
{

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
    m_tokens.dumpAllTokens();
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
    dumpAllBuiltinIds();

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
        // dumpNFAs(name, start, end);    
        // create a dfa accroding to the rule 
        vector<DFA *> *dfaset = convertNFAToDFA(start, end);
        simplifyDFAs(name, *dfaset);
        // dump all dfa state for the rule to debug
        // dumpDFAs(name, *dfaset);
        
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
    // save the nonterminal name and state maping
    m_start = m_nonterminals[m_firstNonterminal];
    
    // after all rule had been parsed, create first for all nonterminal 
    vector<int>::iterator symbolIndex = m_labels.begin();
    for (; symbolIndex != m_labels.end(); symbolIndex++) {
        int symbol = *symbolIndex; 
        if (symbol < 500) 
            continue;
        // get the nonterminal name and dfas
        string nonterminal = m_nonterminalName[symbol];
        vector<DFA *> *dfas = m_dfas[nonterminal];
        if (!dfas) {
            dbg("the dfas for nonterminlal %s is null\n", nonterminal.c_str());
            break;
        }
        // make first for the nonterminal
        vector<int> first; 
        makeFirst(nonterminal, dfas->at(0), first);
        m_first[nonterminal] = first; 
    }
    
    // create  state table for all nonterminal  
    map<string, vector<DFA*>* >::iterator ite;
    for (ite = m_dfas.begin(); ite != m_dfas.end(); ite++) {
        pair<string, vector<DFA*> *> ip = *ite;
        string name = ip.first;
        vector<DFA*> *dfaset = ip.second;
        
        makeStateTableForNonterminal(name, *dfaset);
    }
    dumpDFAsToXml();
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
        GrammarStates *states = new GrammarStates();
        // convert each dfa to a grammar state
        vector<DFA *>::iterator ite = dfas.begin();
        GrammarState state;
        for (; ite != dfas.begin(); ite++) {
            DFA *dfa = *ite;
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
            states->states.push_back(state); 
        }
        m_states.insert(std::make_pair(nonterminalIndex, states));
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
    
    match(TT_OP, ":");
    parseAlternative(ruleName, start, end);
    match(TT_OP, ";");
}

/// parse the alternative, such as alternative : items (| items)*
void Grammar::parseAlternative(const string &ruleName, NFA **start, NFA **end) 
{
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
    //    (*end)->arc(endState); 
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
                else {
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
                    labelIndex += 500; 
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
    // check wether the symbol exist 
    if (m_symbols.find(label) !=  m_symbols.end()) 
        return m_symbols[label];
    else {
        m_symbols[label] = labelIndex;
        m_symbolName[labelIndex] = label;
        m_labels.push_back(labelIndex); 
    }

    // check to see wether the label is terminal, keyword, operators
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


void Grammar::dumpDFAXml(xmlNodePtr node, DFA *dfa)
{
    if (!dfa) return;
    map<string, DFA*>::iterator ite = dfa->m_arcs.begin(); 
    for (; ite != dfa->m_arcs.end(); ite++) {
        string label = ite->first;
        DFA *next = ite->second;
        char buf[5];
        sprintf(buf, "%d", next->m_index);
        string arc = buf;
        xmlNewProp(node, BAD_CAST "label", BAD_CAST label.c_str()); 
        xmlNewProp(node, BAD_CAST "arc", BAD_CAST arc.c_str());
        dumpDFAXml(node, next);
    }
}

void Grammar::dumpDFAsToXml()
{
    const string fileName = "grammar_dfa.xml";
    
    vector<int>::iterator iv = m_labels.begin();
    for (; iv != m_labels.end(); iv++) {
        int label = *iv;
        // the nonterminal id must be larger than 500 
        if (label < 500) continue;
        if (m_symbolName.find(label) == m_symbolName.end()) {
            dbg("the symbol id % is not right\n", label);
            continue;
        }
        // get the nonterminal name and it's dfas
        string nonterminal = m_nonterminalName[label];
        if (m_dfas.find(nonterminal) == m_dfas.end()) {
            dbg("the nonterminal:%s can not be recognized\n", 
                    nonterminal.c_str());
            continue; 
        }
        // for each nonterminal, get the dfas for the nonterminal
        vector<DFA *> *dfas = m_dfas[nonterminal]; 
        ASSERT(dfas != NULL);
        // for all dfa for the terminal, dump it dfa and firs
        // make a new xml root node
        xmlNodePtr rootNode = xmlNewNode(NULL, BAD_CAST "nonterminal");
        xmlNewProp(rootNode, BAD_CAST "name", BAD_CAST nonterminal.c_str()); 
        xmlAddChild(m_xmlRootNode, rootNode); 
        
        // DFA sub node 
        xmlNodePtr xmlNode = xmlNewNode(NULL, BAD_CAST "DFA"); 
        xmlAddChild(rootNode, xmlNode);
        char buf[255];
        sprintf(buf, "%d", (int)dfas->size());
        xmlNewProp(xmlNode, BAD_CAST "count", BAD_CAST buf);
        
        // for all dfas for the nonterminal 
        vector<DFA *>::iterator ite = dfas->begin();
        for (; ite != dfas->end(); ite++) {
            DFA *dfa = *ite; 
            // for each dfa
            char buf[10];
            sprintf(buf, "DFA(%d)", dfa->m_index);
            xmlNodePtr dfaNode = xmlNewNode(NULL, BAD_CAST buf); 
            if (dfa->m_isFinal)
                xmlNewProp(dfaNode, BAD_CAST "final", BAD_CAST "true");
            else
                xmlNewProp(dfaNode, BAD_CAST "final", BAD_CAST "false");
            sprintf(buf, "%d", (int)dfa->m_arcs.size());
            xmlNewProp(dfaNode, BAD_CAST "count", BAD_CAST buf); 
            xmlAddChild(xmlNode, dfaNode); 
            // for all arcs in the dfa
            int index = 0;
            map<string, DFA*>::iterator ite = dfa->m_arcs.begin(); 
            for (; ite != dfa->m_arcs.end(); ite++) {
                string label = ite->first;
                DFA *next = ite->second;
                char attribute[256];
                char val[256];

                xmlNodePtr child = xmlNewNode(NULL, BAD_CAST "label"); 
                xmlAddChild(dfaNode, child); 
                sprintf(attribute, "(%d)", index);
                xmlNewProp(child, BAD_CAST attribute, BAD_CAST label.c_str()); 
                
                sprintf(attribute, "arc(%d)", index);
                sprintf(val, "%d", next->m_index);
                xmlNewProp(child, BAD_CAST "arc", BAD_CAST val);
                index++;
            }
        }
        
        // dump first for the terminal
        vector<int> &first = m_first[nonterminal];
        xmlNodePtr nxmlNode = xmlNewNode(NULL, BAD_CAST "first");
        string firstName;
        for (size_t index = 0; index < first.size(); index++) {
            firstName += m_symbolName[first[index]];
            if (index + 1 < first.size())
                firstName += ",";
        }
        xmlNewProp(nxmlNode, BAD_CAST "first", BAD_CAST firstName.c_str()); 
        xmlAddChild(rootNode, nxmlNode); 
    }
    xmlSaveFormatFileEnc(fileName.c_str(), m_xmlDoc, "UTF-8", 1);
}

