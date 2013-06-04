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
    vector<GrammarStateEntry> &states = m_grammar->getStates();
    m_start = m_grammar->getStartStateIndex(); 
    
    Item item;
    item.stateEntry = &states[m_start];
    item.node = m_curNode;
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
    int labelIndex = classify(token);
    if (labelIndex < 0) {
        Error::complain(token->location, "the token is unknow\n");
        return false;
    }

    while (true) {
    
        // get the top stack item, state index and node
        Item item = m_items.top();
        GrammarStateEntry *stateEntry = item.stateEntry;
        int stateIndex = item.stateIndex;
        
        // get states and first for the dfa
        // int first = stateEntry->first;
        vector<GrammarState> &states = stateEntry->states;
        
        // get current state
        GrammarState state = states[stateIndex];
    
        // flag to indicate terminal match
        bool isFound = false;
        bool isAccepting = state.isFinal;
        
        // for the current state, iterate
        for (int i = 0; i < state.arcs.size(); i++) {
            
            // get labe id and next state
            int labelId = state.arcs[i].first;
            int nextState = state.arcs[i].second;
            
            // at first, check to see wether match a non-termainal
            if (labelIndex == labelId) {
                
                // setup flag to indicate that find a terminal
                isFound = true;
                // push the non-terminal into stack
                shift(nextState, token);
                // goto next state
                state = states[nextState];
                
                // while the only possible action is to accept,
                // then pop nodes off the stack.
                while (true) {
                    if (!state.arcs.empty() && !state.isFinal) {
                        popup();
                        // the parsing is done.
                        if (state.arcs.empty())
                            return true;
                        // get the top stack item
                        item = m_items.top();
                        state = item.stateEntry->states[item.stateIndex];
                    }
                }
                return false;
            }
            
            // then, check to see wether the token can start a child node
            else if (m_grammar->isNonterminal(labelId)) {
                
                // get the state entry according to symbol id
                GrammarStateEntry *subStateEntry = m_grammar->getNonterminalState(labelId);
                
                // check to see wether the label index is in the arcs of the state
                if (m_grammar->isLabelInState(labelIndex, *subStateEntry)){
                    push(subStateEntry, nextState, labelId, token); 
                    break;
                }
            }
            else {
                Error::complain(token->location, "Unknow parser state\n");
                return false;
            }
        }
        
        // check to see wether success to find any arc to next state
        if (isFound == false) {
            // failed to find any arcs to match another state,
            // so unless this state is accepting, it's invalid input.
            if (isAccepting == true) {
                popup();
                if (m_items.empty()) {
                    // throw an exception
                    return false;
                }
            }
            else {
                // throw an exception to report
            }
        }
    }
    return true;
}

// get the label index for the specified token
int Parser::classify(Token *token) 
{
    int labelIndex = -1;
    
    switch (token->type) {
        case T_KEYWORD:
            labelIndex = m_grammar->getLabel(Grammar::Terminal, token->assic);
            break;
        case T_OP:
            labelIndex = m_grammar->getLabel(Grammar::Terminal, token->assic);
            break;
        case T_STRING:
        case T_INT:
        case T_FLOAT:
        case T_ID:
            labelIndex = m_grammar->getLabel(Grammar::Terminal, token->assic);
        default:
            break;

    }
    return labelIndex;
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
    
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->location);
    item.node->addChild(newNode);
}

// push a terminal and ajust the current state
void Parser::push(GrammarStateEntry *entry, 
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
    item.stateEntry = entry;
    item.stateIndex = nextState;
    item.labelId = labelId;
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




