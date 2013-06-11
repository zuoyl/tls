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
    this->type = -1;
}
Node::Node(int type, std::string &value, Location &location) 
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
    m_curNode = new Node();
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
    item.node = new Node(); 
    item.stateIndex = 0;
    item.token = NULL;
    m_items.push(item);

    // create the xml node according to wether options is specified
    CompileOption &option = CompileOption::getInstance();
    if (option.isOutputParseTree()) {
        m_xmlDoc = xmlNewDoc(BAD_CAST "1.0");
        m_xmlRootNode = xmlNewNode(NULL, BAD_CAST "root");
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
    dbg("Parser:pushToken(%s)\n", token->assic.c_str());
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
        dbg("Parser:current nonterminal:%s, state:%d\n", 
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
                if (state->isFinal) {
                    // if input symbol cause a nonterminal to be accepted,
                    // pop off the whole nonterminal from stack
                    while (!m_items.empty()) {
                        item = m_items.top();
                        if (item.state == nonterminalState)
                            popup();
                        else
                            break;
                    }
                }
                break; 
            }
            // if the label is nonterminal 
            else if (m_grammar->isNonterminal(label)) {
                GrammarNonterminalState *subState =
                            m_grammar->getNonterminalState(label);
                if (subState) {
                   // dbg("Parser:: checking new nonterminal %s\n", subState->name.c_str()); 
                    vector<int>::iterator i = subState->first.begin();
                    for (; i != subState->first.end(); i++) {
                        if (*i == symbol) { 
                            // if a new nonterminal is found, shift the nonterminal into stack 
                            push(subState, nextState, token);
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
        if (isFound) {
            dbg("Parser: new nonterminal is found\n"); 
            continue;
        }
        // if the input symbol is accepted, match the next token 
        if (isAccepting)
            break;
        // check to see wether any arcs is matched
        if (ite == state->arcs.end()) {
            int expectedSymbol = -1;
            if (state->arcs.size() == 1)
                expectedSymbol = symbol;
            Error::complain("input is invalid:%s, line:%d, expectted:%d\n", 
                    token->assic.c_str(),
                    token->location.getLineno(), 
                    expectedSymbol);
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
    Token *token = NULL;
    while ((token = tokenStream->getToken()) != NULL) {
        if (!pushToken(token))
            return NULL;
        tokenStream->advanceToken(); 
    }
    CompileOption &option = CompileOption::getInstance();
    if (!option.isOutputParseTree()) {
        outputParseTree(m_root, m_xmlRootNode);
        unsigned found = m_file.find_last_of(".");
        string fileName = m_file.substr(0, found);
        fileName += "_parse";
        fileName += ".xml";
        xmlSaveFile(fileName.c_str(), m_xmlDoc);
    }
    return m_root;
}

// push a non-terminal and prepare for the next state
void Parser::push(GrammarNonterminalState *state, int nextState, Token *token) 
{
    dbg("Parser:shift(%s,%d,%s)\n", state->name.c_str(), nextState, token->assic.c_str()); 
    Item &ref = m_items.top();
    ref.stateIndex = nextState;
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->location);
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
    dbg("Parser:push(%s, %d,%s)\n", ref.state->name.c_str(),  nextState, token->assic.c_str());
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->location);
    ref.node->addChild(newNode);
    
    // push new item
    Item item;
    item.state = ref.state;
    item.stateIndex = nextState;
    item.token = token;
    item.node = ref.node;
    m_items.push(item);
}

void Parser::popup() 
{
    dbg("Parser:pop()\n");
    Item ref = m_items.top();
    Node *node = ref.node;
    
    // get top item
    m_items.pop();
    
    // if there is item existed on stack, then add the node into parent node on stack
    if (!m_items.empty()) {
        Item &item = m_items.top();
        item.node->addChild(node);
    }
    
    // if there is no item on stack, just as root node
    else {
        m_root = node;
    }
}

void Parser::outputParseTree(Node *node, xmlNodePtr xmlNode)
{
    if (!node)
        return;
    
    // iterate the node and create the sub node     
    xmlNodePtr nxmlNode = xmlNewNode(NULL, BAD_CAST node->assic.c_str());
    string location;
    location = node->location.getLineno();
    xmlNewProp(nxmlNode, BAD_CAST "location", BAD_CAST location.c_str());
    xmlAddChild(xmlNode, nxmlNode); 
    
    vector<Node *>::iterator ite = node->childs.begin();
    for (; ite != node->childs.end(); ite++) {
        Node *snode = static_cast<Node *>(*ite);
        // create sub node 
        outputParseTree(snode, nxmlNode); 
    }

}




