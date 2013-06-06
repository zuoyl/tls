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
    item.states = m_grammar->getStates(m_start);
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

    // get the token index
    int symbol = classify(token);
    if (symbol  < 0) {
        Error::complain(token->location, "the token is unknow\n");
        return false;
    }
    while (true) {
        // get the top stack item, state index and node
        Item item = m_items.top();
        // get current states and state index 
        GrammarStates *states = item.states; 
        int curStateIndex = item.stateIndex;
        GrammarState *state = &states->states[curStateIndex];
        vector<int> &first = states->firstset;

        Node *curNode = item.node; 
        map<int, int> &arcs = state->arcs;
        
        // for each arc in current state
        map<int, int>::iterator ite = arcs.begin();
        for (; ite != arcs.end(); ite++) {
            int label = ite->first;
            int nextState = ite->second;
            // if the labelIndex is matched, a nonterminal is matched 
            if (label == symbol) {
                shift(nextState, token);
                state = &states->states[nextState];
                while (!state->isFinal) {
                    popup();
                    if (m_items.empty()) {
                        dbg("parse is over\n"); 
                        return true;
                    }
                    Item aitem = m_items.top();
                    state = &aitem.states->states[item.stateIndex];
                }
            }
            // if the symbol is nonterminal 
            else if (m_grammar->isNonterminal(symbol)) {
                GrammarStates *subStates = m_grammar->getStates(symbol);
                if (subStates) { 
                    vector<int>::iterator i = subStates->firstset.begin();
                    for (; i != subStates->firstset.end(); i++) {
                        if (*i == symbol) { 
                            push(subStates, nextState, symbol, token);
                            break; 
                        }
                    }
                }
            }
        }
        // check to see wether any arcs is matched
        if (ite == arcs.end()) {
            popup();
            if (!m_items.empty())
                Error::complain("too many tokens are input\n");
        }
        else {
            int expectedSymbol = -1;
            if (arcs.size() == 1)
                expectedSymbol = symbol;
            Error::complain("input is invalid:%s, line:%d, expectted:%d\n", 
                    token->assic.c_str(),
                    token->location.getLineno(), 
                    expectedSymbol);
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
        case T_INT:
        case T_FLOAT:
        case T_ID:
            symbol = m_grammar->getSymbolID(Grammar::Terminal, token->assic);
        default:
            break;

    }
    return symbol;
}

Node *Parser::build(TokenStream *tokenStream) 
{
    Token *token = NULL;
    while ((token = tokenStream->getToken()) != NULL) {
        Error::complain("[Parser]Parsing token[%s]\n", token->assic.c_str()); 
        pushToken(token);
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

// shift a non-terminal and prepare for the next state
void Parser::shift(int nextState, Token *token) 
{
    Error::complain("[Parser]shift(%d, %s)\n", nextState, token->assic.c_str()); 
    Item &item = m_items.top();
#if 0 
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->location);
    item.node->addChild(newNode);
#endif
}

// push a terminal and ajust the current state
void Parser::push(GrammarStates *states, 
                  int nextState, 
                  int labelId, 
                  Token *token) 
{
    Error::complain("[Parser]push(%d,%s)\n", nextState, token->assic.c_str());
    Item &ref = m_items.top();
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->location);
    ref.node->addChild(newNode);
    
    // push new item
    Item item;
    item.states = states;
    item.stateIndex = nextState;
    item.token = token;
    item.node = newNode;
    m_items.push(item);
}

void Parser::popup() 
{
    Error::complain("[Parser]pop()\n");
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




