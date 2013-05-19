//
//  Parser.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_PARSER_H
#define TCC_PARSER_H

#include "Common.h"
#include "TGrammar.h"
#include "Location.h"

class TokenStream;
class Token;
class ParserTree;

class Node 
{
public:
    int type;
    std::string assic;
    Location location;
    Node *parent;
    vector<Node *> childs;
public:
    Node();
    Node(int type):location(-1) { this->type = type; }
    Node(int type, std::string &value, Location &location);
    ~Node();
    int count();
    void addChild(Node *node);
};


class Parser 
{
public:
    Parser();
    ~Parser();
    Node * parse(TokenStream *tokenStream);
    
private:
    bool pushToken(Token *token);
    void shift(int nextState, Token *token);
    void push(TStateEntry *entry, int nextState, int symbolId, Token *token);
    void popup();
    int  classify(Token *token);
    bool isLabelInState(int label, TStateEntry *stateEntry);
    
private:
    // stack item
    struct StackItem {
        TStateEntry *stateEntry;
        int stateIndex;
        Node *node;
        int labelId;
        Token *token;
    };
    StackItem &getStackTopReference();
    
private:
    Grammar *m_grammar;
    Node *m_root;
    Node *m_curNode;
    int m_start;
    vector<StackItem > m_stack;
};

#endif // TCC_PARSER_H
