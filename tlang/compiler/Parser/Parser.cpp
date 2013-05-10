//
//  Parser.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Lexer.h"
#include "TokenStream.h"
#include "Parser.h"

Node::Node(int type, std::string &value, int lineno, int column) 
{
    this->type = type;
    this->assic = value;
    this->lineno = lineno;
    this->column = column;
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


Parser::Parser(Grammar *grammar) 
{
    m_grammar = grammar;
    m_start = grammar->getStartStateIndex();
    m_root = NULL;
    m_curNode = new Node(m_start);
    
    // initialize the stack
    vector<TStateEntry> & states = grammar->getStates();
    StackItem item;
    item.stateEntry = &states[m_start];
    item.node = m_curNode;
    item.stateIndex = 0;
    item.token = NULL;
    
    m_stack.push_back(item);
}

Parser::~Parser() 
{    
}

bool Parser::pushToken(Token *token) 
{
    assert (m_grammar != NULL);
    assert (token != NULL);

    // get the states from grammar entity
    vector<TStateEntry> &gstates = m_grammar->getStates();

    // get the token index
    int labelIndex = classify(token);
    if (labelIndex < 0) {
        Error::complain("the token is unknow\n");
        return false;
    }

    while (true) {
    
        // get the top stack item, state index and node
        StackItem &item = getStackTopReference();
        TStateEntry *stateEntry = item.stateEntry;
        int stateIndex = item.stateIndex;
        
        // get states and first for the dfa
        // int first = stateEntry->first;
        vector<TState> &states = stateEntry->states;
        
        // get current state
        TState state = states[stateIndex];
    
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
                        item = getStackTopReference();
                        state = item.stateEntry->states[item.stateIndex];
                    }
                }
                return false;
            }
            
            // then, check to see wether the token can start a child node
            else if (m_grammar->isNonterminal(labelId)) {
                
                // get the state entry according to symbol id
                TStateEntry *subStateEntry = m_grammar->getNonterminalState(labelId);
                
                // check to see wether the label index is in the arcs of the state
                if (m_grammar->isLabelInState(labelIndex, *subStateEntry)){
                    push(subStateEntry, nextState, labelId, token); 
                    break;
                }
            }
            else {
                Error::complain("Unknow parser state\n");
                return false;
            }
        }
        
        // check to see wether success to find any arc to next state
        if (isFound == false) {
            // failed to find any arcs to match another state,
            // so unless this state is accepting, it's invalid input.
            if (isAccepting == true) {
                popup();
                if (m_stack.empty()) {
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
            labelIndex = m_grammar->getKeywordLabel(token->assic);
            break;
        case T_OP:
            labelIndex = m_grammar->getOperatorLabel(token->assic);
            break;
        case T_STRING:
            labelIndex = m_grammar->getTerminalLabel(Grammar::TerminalString);
            break;
        case T_INT:
        case T_FLOAT:
            labelIndex = m_grammar->getTerminalLabel(Grammar::TerminalNumber);
            break;
        case T_ID:
            labelIndex = m_grammar->getTerminalLabel(Grammar::TerminalIdentifier);
        default:
            break;

    }
    return labelIndex;
}

Node *Parser::parse(TokenStream *tokenStream) 
{
    Token *token = NULL;
    
    while ((token = tokenStream->getToken()) != NULL) {
        pushToken(token);
    }
    return m_root;
}

Parser::StackItem &Parser::getStackTopReference() 
{
    return m_stack.back();
}

// shift a non-terminal and prepare for the next state
void Parser::shift(int nextState, Token *token) 
{
    StackItem &item = getStackTopReference();
    
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->lineno, token->column);
    item.node->addChild(newNode);
}

// push a terminal and ajust the current state
void Parser::push(TStateEntry *entry, 
                  int nextState, 
                  int labelId, 
                  Token *token) 
{
    StackItem &ref = getStackTopReference();
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->lineno, token->column);
    ref.node->addChild(newNode);
    
    // push new item
    StackItem item;
    item.stateEntry = entry;
    item.stateIndex = nextState;
    item.labelId = labelId;
    item.token = token;
    item.node = newNode;
    m_stack.push_back(item);
    
}

void Parser::popup() 
{
    StackItem &ref = getStackTopReference();
    Node *node = ref.node;
    
    // get top item
    m_stack.pop_back();
    
    // if there is item existed on stack, then add the node into parent node on stack
    if (!m_stack.empty()) {
        StackItem &item = getStackTopReference();
        item.node->addChild(node);
    }
    
    // if there is no item on stack, just as root node
    else {
        m_root = node;
    }
}
