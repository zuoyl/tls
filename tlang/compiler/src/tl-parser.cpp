//
//  Parser.cpp
//  A toyable language compiler (like a simple c++)


#include "tl-common.h"
#include "tl-error.h"
#include "tl-lexer.h"
#include "tl-token.h"
#include "tl-parser.h"
#include "tl-compile.h"
#include <algorithm>

using namespace tlang;

Node::Node()
{}

Node::Node(const string &name, int type, const std::string &value, Location &location) 
{
    this->name = name;
    this->type = type;
    this->assic = value;
    this->location = location;
}

Node::~Node() 
{}

size_t
Node::count() 
{
    return (int)this->childs.size();
}

void 
Node::addChild(Node *node) 
{
    this->childs.push_back(node);
}


Parser::Parser(const string &path, const std::string &file) 
{
    m_path = path;
    m_file = file;
    m_grammar = &Grammar::getInstance();
    m_root = NULL;
    m_tokens = NULL;
}

Parser::~Parser() 
{    
    // free resource for xml
    CompileOption& option = CompileOption::getInstance();
    if (option.isOutputParseTree() && m_xmlDoc) {
        xmlFreeDoc(m_xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    }
}

bool 
Parser::prepare()
{
    
    // initialize the stack
    if (!m_grammar->build("tlang.grammar"))
        return false;
    m_start = m_grammar->getStartStateIndex(); 
    if (!m_grammar->isNonterminal(m_start)) {
        Error::complain("the first state is not nonterminal\n");
        return false;
    }
    // push the frist nonterminal into stack 
    Item item;
    item.state = m_grammar->getNonterminalState(m_start);
    Location location;
    string name = "root";
    m_root = new Node(name, TOKEN_TYPE_UNKNOWN, item.state->name, location); 
    
    item.node = m_root; 
    item.stateIndex = 0;
    item.token = NULL;
    m_items.push(item);


    // create the xml node according to wether options is specified
    CompileOption &option = CompileOption::getInstance();
    if (option.isOutputParseTree()) {
        m_xmlDoc = xmlNewDoc(BAD_CAST "1.0");
        m_xmlRootNode = xmlNewNode(NULL, BAD_CAST "ParseTree");
        xmlDocSetRootElement(m_xmlDoc, m_xmlRootNode); 
    }
    else {
        m_xmlDoc = NULL;
        m_xmlRootNode = NULL;
    }
    return true;
}

// try match a nonterminal
bool 
Parser::tryNonterminal(GrammarNonterminalState *nonterminal, Token *token)
{
    dbg("Parser:try nonterminal '%s' with token '%s'\n", 
            nonterminal->name.c_str(), token->assic.c_str()); 

    // get symbol for this token
    int symbol  = classify(token);
    if (symbol < 0)
        return false;

    // loop to try match this token
    while (!m_alternative.empty()) {
        Item item = m_alternative.top();
        GrammarNonterminalState *nonterminalState = item.state;
        int stateIndex = item.stateIndex;
        dbg("Parser: \t> nonterminal = %s, state = %d\n", 
                nonterminalState->name.c_str(),
                stateIndex); 
        GrammarState *state = &nonterminalState->states[stateIndex];
        map<int, int>::iterator ite = state->arcs.begin();
        
        bool isAccepting = false;
        for (; ite != state->arcs.end(); ite++) {
            int label = ite->first;
            int nextState = ite->second;
            
            // if the label is matched, the current nonterminal is matched 
            if (label == symbol) {
                // shift the next state into stack
                Item nitem;
                nitem.state = nonterminalState;
                nitem.stateIndex = nextState;
                m_alternative.push(nitem); 
                isAccepting = true;
                state = &nonterminalState->states[nextState];
                // if next state is final state, accept a nonterminal 
                while (state && state->isFinal) {
                    // pop the current nonterminal 
                    while (!m_alternative.empty()) {
                        if (m_alternative.top().state == nonterminalState)
                            m_alternative.pop();
                        else
                            break;
                    }
                    if (m_alternative.empty())
                        break;
                    nonterminalState = m_alternative.top().state;
                    nextState = m_alternative.top().stateIndex;
                    state = &nonterminalState->states[nextState];
                    if (!isFinalState(nonterminalState, state))
                        break;
                
                }
                if (nonterminalState == nonterminal) 
                    return true;
                break; 
            }
            // if the label is nonterminal 
            else if (m_grammar->isNonterminal(label)) {
                GrammarNonterminalState *subState =
                            m_grammar->getNonterminalState(label);
                if (subState) {
                    vector<int> &first = subState->first; 
                    if (find(first.begin(), first.end(), symbol) != first.end()) {
                        // change top nonterminal's next state
                        m_alternative.top().stateIndex = nextState;
                        // push a new nonterminal state 
                        Item nitem;
                        nitem.state = subState;
                        nitem.stateIndex = 0;
                        m_alternative.push(nitem);
                        break;
                    }
                }
            }
        }
        if (isAccepting)
            continue;
        if (ite == state->arcs.end())
            return false;
    }
    return false;
}

bool 
Parser::pushToken(Token *token) 
{
    dbg("\n=>Parser:pushToken('%s', lineno =%d)\n", 
            token->assic.c_str(), token->location.getLineno());
    // get the token index
    int symbol = classify(token);
    // make sure the token is a valid symbol 
    if (symbol  < 0) {
        Error::complain(token->location, 
                "token '%s' is unknow\n", token->assic.c_str());
        return false;
    }
    // the nonterminal state is to save current nonterminal
    // it will also be used to error recovery.
    GrammarNonterminalState *nonterminalState = NULL; 
    // loop until the parse stack is empty 
    while (!m_items.empty()) {
        // get current nonterminal state and state index 
        nonterminalState = m_items.top().state; 
        int stateIndex = m_items.top().stateIndex;
        if (!nonterminalState) {
            dbg("Parser:can not get states\n");
            return false;
        }
        dbg("Parser:=>current nonterminal:'%s', state:%d\n", 
                nonterminalState->name.c_str(), stateIndex); 
        
        // get first of the nonterminal and current state of nonterminal 
        GrammarState *state = &nonterminalState->states[stateIndex];
        
        // flag that indicate wether the symbol is accepted 
        bool isAccepting = false; 
        // nonterminals is use to savle all anternatives nonterminal 
        map<GrammarNonterminalState*, int> nonterminals; 
        // loop for current nonterminal's state 
        map<int, int>::iterator ite;
        for (ite = state->arcs.begin(); ite != state->arcs.end(); ite++) {
            int label = ite->first;
            int nextState = ite->second;
            
            // if the label is matched, the current nonterminal is matched 
            if (label == symbol) {
                // shift the next state into stack 
                shift(nextState, token);
                isAccepting = true;
                state = &nonterminalState->states[nextState];
                // if next state is final state, accept a nonterminal 
                if (state->isFinal)
                    reduce(nonterminalState);
                break; 
            }
            // if the label is nonterminal 
            else if (m_grammar->isNonterminal(label)) {
                GrammarNonterminalState *subState =
                            m_grammar->getNonterminalState(label);
                if (subState) {
                    vector<int> &first = subState->first; 
                    if (find(first.begin(), first.end(), symbol) != first.end()) {
                        dbg("Parser::found alternative nonterminal '%s'\n", 
                                subState->name.c_str());
                        nonterminals.insert(make_pair(subState, nextState));
                    }
                }
            }
        }
        // if the input symbol is accepted, match the next token 
        if (isAccepting)
            break;
        // if new nonterminals are found, 
        // find the most longest nonterminal to match 
        if (!nonterminals.empty()) { 
            GrammarNonterminalState *result = selectNonterminal(nonterminals, token);
            if (result) {
                int nextState = nonterminals[result]; 
                // if a new nonterminal is found, shift the nonterminal into stack 
                dbg("Parser:new nonterminal '%s' is found\n", 
                        result->name.c_str()); 
                push(result, nextState, token);
                continue; 
            }
            else if (!recoveryError(nonterminalState, state, token)) {
                break;
            }
        }
        // check to see wether any arcs is matched
        else {
            // for error recovery, if the error can be recovered, continue 
            if (!recoveryError(nonterminalState, state, token))
                break; 
        }
    }
    return true;
}

// recover errors
bool 
Parser::recoveryError(
        GrammarNonterminalState *nonterminal,
        GrammarState *state, 
        Token *token)
{
    // if there is only one arc in state, the expected token is not found
    // in this case, error should be reported
    if (state->arcs.size() == 1) {
        map<int,int>::iterator ite = state->arcs.begin();
       int labelIndex = ite->first;
       // get label name
       string symbol;
       m_grammar->getSymbolName(labelIndex, symbol);
       if (m_grammar->isTerminal(labelIndex)) {
           Error::complain(token->location,
                   "expected token '%s' missed",
                   symbol.c_str());
           // make a new token and insert into token stream
           // and return true indicate the recover success
           // TODO
       }
    }
    else
        Error::complain(token->location, 
                "token '%s' is not expected in %s", 
                token->assic.c_str(),
                nonterminal->name.c_str());

    return false;
}

// in some case, there are two nonterminals which have same first symbol
// the most longest nonterminal will be matched
GrammarNonterminalState* 
Parser::selectNonterminal(
        map<GrammarNonterminalState*, int> &nonterminals,
        Token* token)  
{
   
    dbg("Parser::selecting nonterminals\n");
    
    map<GrammarNonterminalState*, int>::iterator ite = nonterminals.begin();
    if (nonterminals.size() == 1)
        return ite->first;
   
    for (; ite != nonterminals.end(); ite++) {
        GrammarNonterminalState* nonterminal = ite->first; 
        // initialize a stack to try match this nonterminal 
        Item item;
        item.state = nonterminal;
        item.stateIndex = 0;
        item.node = NULL;
        // the alternative stack is only used to match next 
        // three token for this nonterminal, it muse be cleared before trying
        while (!m_alternative.empty())
                m_alternative.pop(); 
        // initialize the alternative stack by the first nonterminal
        m_alternative.push(item); 
        // mark the token stream
        m_tokens->mark();
        int step = 3;  // only match the next tree token 
        while (step > 0) { 
            if (!tryNonterminal(nonterminal, token)) {
                dbg("Parser:\t> try nonterminal '%s' failed\n", nonterminal->name.c_str()); 
                break;
            }
            m_tokens->advanceToken();
            token = m_tokens->getToken();
            step--; 
        }
        m_tokens->gotoMark(); 
        token = m_tokens->getToken(); 
        if (step == 0)
            return nonterminal; 
    }
    // if all nontermial failed, select the first nonterminal 
    return NULL;
}

// get the label index for the specified token
int 
Parser::classify(Token *token) 
{
    int symbol = -1;
    
    switch (token->type) {
        case TOKEN_TYPE_KEYWORD:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, token->assic);
            break;
        case TOKEN_TYPE_OP:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, token->assic);
            break;
        case TOKEN_TYPE_STRING:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, "string"); 
            break; 
        case TOKEN_TYPE_INT:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, "NUMBER");
            break;
        case TOKEN_TYPE_FLOAT:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, "FLOATNUMBER");
            break; 
        case TOKEN_TYPE_ID:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, "IDENTIFIER");
            break; 
        default:
            break;

    }
    return symbol;
}

Node* 
Parser::build(TokenStream *tokenStream) 
{
    m_tokens = tokenStream; 
    Token* token = NULL;
    while ((token = m_tokens->getToken()) != NULL) {
        pushToken(token);
        m_tokens->advanceToken(); 
    }
    CompileOption& option = CompileOption::getInstance();
    if (option.isOutputParseTree()) {
        outputParseTree(m_root, m_xmlRootNode);
        string fullFileName = m_path;
        fullFileName += "/";
        fullFileName += m_file; 
        fullFileName += ".parse";
        fullFileName += ".xml";
        xmlSaveFormatFileEnc(fullFileName.c_str(), m_xmlDoc, "UTF-8", 1);
    }
    return m_root;
}

// push a non-terminal and prepare for the next state
void 
Parser::push(GrammarNonterminalState *state, int nextState, Token *token) 
{
    Item &ref = m_items.top();
    ref.stateIndex = nextState;
    dbg("Parser:push('%s'->%d, '%s'->%d,'%s')\n", 
        ref.state->name.c_str(), nextState,
        state->name.c_str(), 0, 
        token->assic.c_str()); 
    
    // make a new node
    string nodeName = "nonterminal";
    Node *newNode = new Node(nodeName, token->type, state->name, token->location);
    ref.node->addChild(newNode);
    // push the new nonterminal into stack
    Item item;
    item.state = state;
    item.stateIndex = 0;
    item.token = token;
    item.node = newNode;
    m_items.push(item);
}

// shift a terminal and ajust the current state
void 
Parser::shift(int nextState, Token *token) 
{
    Item &ref = m_items.top();
    dbg("Parser:shift('%s'->%d,'%s')\n", 
            ref.state->name.c_str(),  nextState, token->assic.c_str());
    
    string nodeName;
    int index = classify(token); 
    if (m_grammar->isNonterminal(index))
        nodeName = "nonterminal";
    else
        nodeName = "terminal";
    Node *newNode = new Node(nodeName, token->type, token->assic, token->location);
    ref.node->addChild(newNode);
    
    // push new item
    Item item;
    item.state = ref.state;
    item.stateIndex = nextState;
    item.token = token;
    item.node = ref.node;
    m_items.push(item);
}

bool 
Parser::isFinalState(
        GrammarNonterminalState *nonterminalState,
        GrammarState *state)
{
    if (!nonterminalState || !state)
        return false;
    
    
    if (!state->isFinal)
        return false;
    // check wether the next token is in arcs of the next state 
    Token *token = m_tokens->lookNextToken();
    if (!token)
        return true;
    int labelIndex = classify(token);
    if (labelIndex < 0)
        return false;
    // check wether token is in next nonterminla's first 
    map<int, int>::iterator ite = state->arcs.begin();
    for (; ite != state->arcs.end(); ite++) {
        int label = ite->first;
        // the state can not be finished
        if (label == labelIndex)
            return false;
        // if the label is nonterminal, check wether the token is in it's first 
        if (m_grammar->isNonterminal(label)) {
            GrammarNonterminalState *nstate = m_grammar->getNonterminalState(label);
            if (!nstate)
                return false;
            if (find(nstate->first.begin(), nstate->first.end(), labelIndex)
                    != nstate->first.end())
                return false;
        }
    }
    return true;
}


// reduce a nonterminal from stack
void 
Parser::reduce(GrammarNonterminalState *nonterminalState)
{
    dbg("Parser:reduce('%s')\n", nonterminalState->name.c_str()); 
    if (m_items.empty())
        return;
    
    Item item = m_items.top();
    GrammarState *state = &item.state->states[item.stateIndex];
    if (!isFinalState(nonterminalState, state))
        return;
    GrammarNonterminalState *rootState = m_grammar->getNonterminalState(m_start);
    
    while (state && state->isFinal) {
        // if input symbol cause a nonterminal to be accepted,
        // pop off the current whole nonterminal from stack
        while (!m_items.empty()) {
            item = m_items.top();
            if (item.state == nonterminalState)
                popup();
            else
                break;
        }
        if (m_items.empty())
            break;
        item = m_items.top(); 
        if (item.state == rootState)
            break;
        // wether the next state can be poped off
        nonterminalState = item.state;
        state = &nonterminalState->states[item.stateIndex];
        // if the next state is final state, however there is next token in it's first
        // don't pop off the state
        if (!isFinalState(nonterminalState, state))
           return;
    }
}

void 
Parser::popup() 
{
    Item ref = m_items.top();
    string tokenName = "null";
    if (ref.token && !ref.token->assic.empty()) 
        tokenName = ref.token->assic.c_str(); 
    dbg("Parser:pop('%s',%d, '%s')\n", 
            ref.state->name.c_str(), ref.stateIndex, tokenName.c_str());
    // get top item
    m_items.pop();
}

const char* tokenTypeNames[] = {
    "UNKNOWN", 
    "keword",
    "operator",
    "identifier",
    "int",
    "float",
    "string"
};

void 
Parser::outputParseTree(Node* node, xmlNodePtr xmlNode)
{
    if (!node)
        return;
    
    // iterate the node and create the sub node     
    xmlNodePtr nxmlNode = xmlNewNode(NULL, BAD_CAST node->name.c_str());
    char buf[100];
    sprintf(buf, "%s", node->assic.c_str());
    xmlNewProp(nxmlNode, BAD_CAST "name", BAD_CAST buf);
    if (node->name != "nonterminal") 
        xmlNewProp(nxmlNode, BAD_CAST "type", BAD_CAST tokenTypeNames[node->type]);  
    sprintf(buf, "%d", node->location.getLineno());
    xmlNewProp(nxmlNode, BAD_CAST "line", BAD_CAST buf);
    xmlAddChild(xmlNode, nxmlNode); 
    
    for (size_t index = 0; index < node->count(); index++) {
        // create sub node 
        outputParseTree(node->childs[index], nxmlNode); 
    }

}
