//
//  Parser.cpp
//  A toyable language compiler (like a simple c++)


#include "compiler/Common.h"
#include "compiler/Lexer.h"
#include "compiler/TokenStream.h"
#include "compiler/Parser.h"
#include "compiler/TGrammar.h" 

Node::Node(int type, std::string &value, int lineno, int column) {
    this->type = type;
    this->assic = value;
    this->lineno = lineno;
    this->column = column;
}

Node::~Node() {
}

int Node::count() {
    return (int)this->childs.size();
}

void Node::addChild(Node *node) {
    this->childs.push_back(node);
}


Parser::Parser(TGrammar *grammar) {
    m_grammar = grammar;
    m_start = grammar->start;
    m_root = NULL;
    m_curNode = new Node(m_start);
    
    // initialize the stack
    StackItem item;
    item.stateEntry = grammar->states[m_start - 255];
    item.node = m_curNode;
    item.stateIndex = 0;
    item.token = NULL;
    
    m_stack.push_back(item);
}

Parser::~Parser() {    
}

bool Parser::pushToken(Token *token) {
    // get the token index
    int labelIndex = classify(token);
    
    while (true) {
    
        // get the top stack item, state index and node
        StackItem &item = getStackTopReference();
        GrammarStateEntry *stateEntry = &item.stateEntry;
        int stateIndex = item.stateIndex;
        
        // get states and first for the dfa
        // int first = stateEntry->first;
        std::vector<GrammarState> &states = stateEntry->states;
        
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
            
            // get the symbol id
            int symbolId = m_grammar->labels[labelId];
            
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
                    if (state.arcs.size() != 0 && state.isFinal) {
                        popup();
                        // the parsing is done.
                        if (state.arcs.size() == 0)
                            return true;
                        // get the top stack item
                        item = getStackTopReference();
                        state = item.stateEntry.states[item.stateIndex];
                    }
                }
                return false;
            }
            
            // then, check to see wether the token can start a child node
            else if (symbolId >= 255) {
                
                // get the state entry according to symbol id
                GrammarStateEntry subStateEntry = m_grammar->states[symbolId - 255];
                
                // check to see wether the label index is in the arcs of the state
                if (isLabelInState(labelIndex, &subStateEntry)){
                    push(subStateEntry, nextState, symbolId, token); 
                    break;
                }
            }
            else {
                // no logic herer
            }
        }
        
        // check to see wether success to find any arc to next state
        if (isFound == false) {
            // failed to find any arcs to match another state,
            // so unless this state is accepting, it's invalid input.
            if (isAccepting == true) {
                popup();
                if (m_stack.size() == 0) {
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

int Parser::classify(Token *token) {
    int labelIndex = -1;
    
    if (token->type == T_KEYWORD) {
        labelIndex = m_grammar->keywordIDs[token->assic];
        if (labelIndex != -1)
            return labelIndex;
    }
    
    labelIndex = m_grammar->tokenIDs[token->type];
    return labelIndex;
}

bool Parser::isLabelInState(int label, GrammarStateEntry *stateEntry) {
    std::vector<GrammarState> &states = stateEntry->states;
    std::vector<GrammarState>::iterator ite;
    
    for (ite = states.begin(); ite < states.end(); ite++) {
        GrammarState state = *ite;
        
        std::vector<std::pair<int, int> > &arcs = state.arcs;
        for (int i = 0; i < arcs.size(); i++) {
            if (label == arcs[i].first)
                return true;
        }
    }
    return false;
}

Node *Parser::parse(TokenStream *tokenStream) {
    Token *token = NULL;
    
    while ((token = tokenStream->getToken()) != NULL) {
        pushToken(token);
    }
    return m_root;
}

Parser::StackItem &Parser::getStackTopReference() {
    return m_stack[m_stack.size() - 1];
}

// shift a non-terminal and prepare for the next state
void Parser::shift(int nextState, Token *token) {
    StackItem &item = getStackTopReference();
    
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->lineno, token->column);
    item.node->addChild(newNode);
}

// push a terminal and ajust the current state
void Parser::push(GrammarStateEntry entry, 
                  int nextState, 
                  int symbolId, 
                  Token *token) {
    StackItem &ref = getStackTopReference();
    // make a new node
    Node *newNode = new Node(token->type, token->assic, token->lineno, token->column);
    ref.node->addChild(newNode);
    
    // push new item
    StackItem item;
    item.stateEntry = entry;
    item.stateIndex = nextState;
    item.symbolId = symbolId;
    item.token = token;
    item.node = newNode;
    m_stack.push_back(item);
    
}
void Parser::popup() {
    StackItem &ref = getStackTopReference();
    Node *node = ref.node;
    
    // get top item
    m_stack.pop_back();
    
    // if there is item existed on stack, then add the node into parent node on stack
    if (m_stack.size() > 0) {
        StackItem &item = getStackTopReference();
        item.node->addChild(node);
    }
    
    // if there is no item on stack, just as root node
    else {
        m_root = node;
    }
}