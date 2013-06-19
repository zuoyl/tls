//
//  Parser.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Error.h"
#include "Lexer.h"
#include "Tokens.h"
#include "Parser.h"
#include "Compile.h"

Node::Node()
{
}
Node::Node(const string &type, const string &value, Location &location) 
{
    this->type = type; 
    this->assic = value;
    this->location = location;
}

Node::~Node() 
{
}

int Node::count() 
{
    return (int)this->childs.size();
}

void Node::addChild(Node *node) 
{
    this->childs.push_back(node);
}


Parser::Parser(const string &path, const string &file) 
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
    CompileOption &option = CompileOption::getInstance();
    if (option.isOutputParseTree()) {
        xmlFreeDoc(m_xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
    }
}
bool Parser::prepare()
{
    
    // initialize the stack
    m_grammar->build("Grammar/grammar.txt"); 
    m_start = m_grammar->getStartStateIndex(); 
    if (!m_grammar->isNonterminal(m_start)) {
        Error::complain("the first state is not nonterminal\n");
        return false;
    }
    // push the frist nonterminal into stack 
    Item item;
    item.state = m_grammar->getNonterminalState(m_start);
    Location location;
    string type = "nonterminal"; 
    m_root = new Node(type, item.state->name, location); 
    
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
}

bool Parser::pushToken(Token *token) 
{
    if (!token)
       return false;
    dbg("\n=>Parser:pushToken('%s', lineno =%d)\n", 
            token->assic.c_str(), token->location.getLineno());
    // get the token index
    int symbol = classify(token);
    if (symbol  < 0) {
        Error::complain(token->location, 
                "the token:%s is unknow\n", token->assic.c_str());
        return false;
    }
    if (!m_grammar->isTerminal(symbol)) {
        Error::complain(token->location, 
                "the token:%s is not an invalid terminal\n", token->assic.c_str());
        return false;
    }
    while (true) {
        // get the top stack item, state index and node
        Item item = m_items.top();
        Node *curNode = item.node; 
        
        // get current nonterminal state and state index 
        GrammarNonterminalState *nonterminalState = item.state; 
        if (!nonterminalState) {
            dbg("Parser:can not get states\n");
            return false;
        }
        dbg("Parser:=>current nonterminal:'%s', state:%d\n", 
                nonterminalState->name.c_str(), item.stateIndex); 
        int stateIndex = item.stateIndex;
        
        // get first of the nonterminal 
        vector<int> *first = &nonterminalState->first;
        GrammarState *state = &nonterminalState->states[stateIndex];
        
        // for each arc in current state
        bool isFound = false; 
        bool isAccepting = false; 
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
                    dbg("Parser:checking new nonterminal '%s'\n", subState->name.c_str()); 
                    vector<int>::iterator i = subState->first.begin();
                    for (; i != subState->first.end(); i++) {
                        if (*i == symbol) { 
                            // if a new nonterminal is found, shift the nonterminal into stack 
                            push(subState, nextState, token);
                            dbg("Parser:new nonterminal '%s' is found\n" ,subState->name.c_str()); 
                            isFound = true; 
                            break; 
                        }
                    }
                }
            }
            if (isFound)
                break;
        }
        // if new nonterminal is found, continue match 
        if (isFound) 
            continue;
        
        // if the input symbol is accepted, match the next token 
        if (isAccepting)
            break;
        // check to see wether any arcs is matched
        if (ite == state->arcs.end()) {
            string expectedSymbol = "null";
            if (state->arcs.size() == 1)
                m_grammar->getSymbolName(symbol, expectedSymbol);
            Error::complain("#####input is invalid:'%s', line:%d, expected:%s\n", 
                    token->assic.c_str(),
                    token->location.getLineno(), 
                    expectedSymbol.c_str());
            // for error recovery, just insert the expected token
                break;
        }
    }
    return true;
}

// get the label index for the specified token
int Parser::classify(Token *token) 
{
    int symbol = -1;
    
    switch (token->type) {
        case T_KEYWORD:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, token->assic);
            break;
        case T_OP:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, token->assic);
            break;
        case T_STRING:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, "STRING"); 
            break; 
        case T_INT:
        case T_FLOAT:
            break; 
        case T_ID:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, "IDENTIFIER");
            break; 
        default:
            break;

    }
    return symbol;
}

Node *Parser::build(TokenStream *tokenStream) 
{
    m_tokens = tokenStream; 
    Token *token = NULL;
    while ((token = m_tokens->getToken()) != NULL) {
        if (!pushToken(token))
            return NULL;
        m_tokens->advanceToken(); 
    }
    CompileOption &option = CompileOption::getInstance();
    if (option.isOutputParseTree()) {
        outputParseTree(m_root, m_xmlRootNode);
        unsigned found = m_file.find_last_of(".");
        string fileName = m_file.substr(0, found);
        string fullFileName = m_path;
        fullFileName += "/";
        fullFileName += fileName; 
        fullFileName += "_parse";
        fullFileName += ".xml";
        xmlSaveFormatFileEnc(fullFileName.c_str(), m_xmlDoc, "UTF-8", 1);
    }
    return m_root;
}

// push a non-terminal and prepare for the next state
void Parser::push(GrammarNonterminalState *state, int nextState, Token *token) 
{
    dbg("Parser:push('%s',%d,'%s')\n", state->name.c_str(), nextState, token->assic.c_str()); 
    Item &ref = m_items.top();
    ref.stateIndex = nextState;
    // make a new node
    string type = "nonterminal"; 
    Node *newNode = new Node(type, state->name, token->location);
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
void Parser::shift(int nextState, Token *token) 
{
    Item &ref = m_items.top();
    dbg("Parser:shift('%s', %d,'%s')\n", ref.state->name.c_str(),  nextState, token->assic.c_str());
    // make a new node
    string type;
    if (m_grammar->isKeyword(token->assic))
        type = "keyword";
    else if (m_grammar->isOperator(token->assic))
        type = "operator";
    else
        type = "identifier";

    Node *newNode = new Node(type, token->assic, token->location);
    ref.node->addChild(newNode);
    
    // push new item
    Item item;
    item.state = ref.state;
    item.stateIndex = nextState;
    item.token = token;
    item.node = ref.node;
    m_items.push(item);
}

bool Parser::isStateFinish(GrammarNonterminalState *nonterminalState, int nextState)
{
    if (!nonterminalState)
        return false;
    if (nextState > nonterminalState->states.size())
        return false;
    GrammarState *state = &nonterminalState->states[nextState]; 
    if (!state->isFinal)
        return false;
    // if next state is final and there is no arcs, it must be final state 
    if (state->isFinal && state->arcs.empty())
        return true;
    // check wether the next token is in arcs of the next state 
    Token *token = m_tokens->lookNextToken();
    if (token) {
        int labelIndex = classify(token);
        if (labelIndex < 0)
            return false;
        map<int, int>::iterator ite = state->arcs.begin();
        for (; ite != state->arcs.end(); ite++) {
            int label = ite->first;
            int next = ite->second;
            // if the next token can match the arcs in current state, 
            // the state can not be finished
            if (label == labelIndex)
                return false;
            // if the label is nonterminal, check wether the token is in it's first 
            if (m_grammar->isNonterminal(label)) {
                GrammarNonterminalState *nstate = m_grammar->getNonterminalState(label);
                if (!nstate)
                    return false;
                if (find(nstate->first.begin(), nstate->first.end(), labelIndex) != nstate->first.end())
                    return false;
            }
        }
    }
    return true;
}


// reduce a nonterminal from stack
void Parser::reduce(GrammarNonterminalState *nonterminalState)
{
    if (m_items.empty())
        return;
    
    Item item = m_items.top();
    if (!isStateFinish(nonterminalState, item.stateIndex))
        return;
    GrammarState *state = &item.state->states[item.stateIndex];
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
        if (!isStateFinish(nonterminalState, item.stateIndex))
               return;
         
    }

}

void Parser::popup() 
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

void Parser::outputParseTree(Node *node, xmlNodePtr xmlNode)
{
    if (!node)
        return;
    
    // iterate the node and create the sub node     
    xmlNodePtr nxmlNode = xmlNewNode(NULL, BAD_CAST node->type.c_str());
    char buf[100];
    sprintf(buf, "%s", node->assic.c_str());
    xmlNewProp(nxmlNode, BAD_CAST "name", BAD_CAST buf);
    sprintf(buf, "%d", node->location.getLineno());
    xmlNewProp(nxmlNode, BAD_CAST "line", BAD_CAST buf);
    xmlAddChild(xmlNode, nxmlNode); 
    
    for (size_t index = 0; index < node->count(); index++) {
        // create sub node 
        outputParseTree(node->childs[index], nxmlNode); 
    }

}




